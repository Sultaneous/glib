/*
* Class Sqlite
* ============
* Karim Sultan, September 2022.
* 
* This class is an API wrapper class in C++ for the sqlite3 database.
* Using a simplified object oriented wrapper, one can easily manipulate
* the sqlite3 database, including parametrical query substitution.
* 
* Most of the read/execute functionality comes in the Cursor class.
* You can obtain a cursor from an open database, done via the connect() method:
* 
-->
gamzia::Sqlite* db = new (std::nothrow) gamzia::Sqlite("mydb_name.db");

db->connect();
gamzia::Cursor k = db->getCursor();
<--

*
* with the cursor you can write and read:
* 
-->
std::string sql = "CREATE TABLE employees (ID INTEGER PRIMARY KEY AUTOINCREMENT, name, salary)";
k.execute(sql);
k.commit;

// add an employees ...
sql = "INSERT INTO employees (name, salary) VALUES (?, ?)";
std::string name, salary;
std::vector<std::string> params;
name="Bob";
salary="100,000";
params.push_back(name);
params.push_back(salary);
k.execute(sql, params);

// Read table (first row is column headers)
sql = "SELECT * FROM employees";
k.execute(sql);

std::vector<std::string> table;
table = k.fetchAll();

// or ... read as single row
// Row pointer will increment for the next read, and return empty on no more data.
std::vector<std::string> row;
row = k.fetchOne();

// Close the DB
db->close();

<--
*
* NOTE: Field types -> regardless of field type, the data is read as and returned as 
* text (std::string).  You can provide conversions after the fact as required.
* 
* DEPENDENCY: sqlite3.dll
*/

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include "Sqlite.h"
#include "sqlite3.h"


/*****************
*  Class Sqlite  *
*****************/

gamzia::Sqlite::Sqlite()
{
   mydbname = "";
   mydb = nullptr;
   isConnected = false;
}

gamzia::Sqlite::Sqlite(std::string dbname)
{
   // Store database name
   mydbname = dbname;
   mydb = nullptr;
   isConnected = false;
}

gamzia::Sqlite::~Sqlite()
{
   if (isConnected == true)
      sqlite3_close (mydb);
}

std::string gamzia::Sqlite::getLastError()
{
   std::stringstream ss;
   ss << "[" << sqlite3_errcode(mydb) << "]  " << sqlite3_errmsg(mydb);
   return (ss.str());
}

bool gamzia::Sqlite::connect()
{
   int rc=sqlite3_open(mydbname.c_str(), &mydb);
   if (rc != SQLITE_OK)
   {
      isConnected=false;
      myerror = sqlite3_errmsg(mydb);
   }
   else
      isConnected = true;

   return(isConnected);
}

gamzia::Cursor gamzia::Sqlite::getCursor()
{
   if (!isConnected)
      // DB not open, can't setup cursor
      return nullptr;

   Cursor mycursor = gamzia::Cursor(mydb);
   return (mycursor);
}

void gamzia::Sqlite::close()
{
   sqlite3_close (mydb);
   isConnected = false;
}

/*****************
*  Class Cursor  *
*****************/

gamzia::Cursor::Cursor(sqlite3* db)
{
   mydb = db;
   statement = nullptr;
}

gamzia::Cursor::~Cursor()
{
   sqlite3_reset (statement);
   sqlite3_finalize (statement);
   statement = nullptr;
}

bool gamzia::Cursor::doesTableExist(std::string table)
{
   std::string sql;
   std::vector<std::string> params;

   // Sanity
   if (table.empty())
     return false;

   // Our pragma select, finds the table or not.
   sql = "select * from pragma_table_list where name = ?";
   params.clear();
   params.push_back(table);
   execute(sql, params);
   if (fetchOne().empty())
      return false;
   
   // Table exists
   return true;
}

/// <summary>
/// Helper method to perform a Commit (finalizes the db changes).
/// </summary>
/// <returns>True on success.</returns>
bool gamzia::Cursor::commit()
{
   return(execute("COMMIT;"));
}

/// <summary>
/// Helper method to peform a rollback (reverses the transaction).
/// </summary>
/// <returns>True on success.</returns>
bool gamzia::Cursor::rollback()
{
   return(execute("ROLLBACK;"));
}

/// <summary>
/// Helper method to perform a vacuum (compacts the database reclaiming free space).
/// </summary>
/// <returns></returns>
bool gamzia::Cursor::vacuum()
{
   return(execute("VACUUM;"));
}

/// <summary>
/// Executes a query.  See the params version for information
/// on how command vs query queries are handled.
/// </summary>
/// <param name="sql">The SQL command to execute.</param>
/// <returns>True if success, false otherwise.</returns>
bool gamzia::Cursor::execute(std::string sql)
{
   int rc;

   // Free the old statement to prevent memory leaks, if one exists
   //if (statement != nullptr)
   //   sqlite3_finalize(statement);

   rc = sqlite3_prepare_v2(mydb, sql.c_str(), -1, &statement, NULL);
   if (rc != SQLITE_OK)
      return false;

   // Execute; reset in case rows are available
   sqlite3_step(statement);
   sqlite3_reset(statement);
   return true;
}

/// <summary>
/// Scans SQL Query for ? and replaces them with params.  Performs mild sanitization
/// on params.  Only replaces '?' placeholders; does not handle variable names.
/// </summary>
/// <param name="sql">An SQL query with ? placeholders.</param>
/// <param name="params">A vector of params.  Must have enough params to replace all '?'s.</param>
/// <returns>True if statement prepared, false otherwise.</returns>
bool gamzia::Cursor::execute(std::string sql, const std::vector<std::string> params)
{
   int        numParams = 0;
   int                x = 0;
   int               rc;
   std::string variable;

   // Count number of '?'s
   while ((x = (int)sql.find('?', x+1)) != std::string::npos)
      numParams++;

   // Sanity
   if ((int)params.size() < numParams)
      return false;

   // Bind
   for (int i = 0; i < numParams; i++)
   {
      // Sanitize
      variable = params[i];
      while ((x = (int)variable.find(';', 0)) != std::string::npos)
         variable.erase(x, 1);
      variable = "'" + variable + "'";

      // Get location in string
      x = (int)sql.find('?', 0);
      
      // Replace the ? with variable
      sql.replace(x, 1, variable);
   }

   // Terminate with semicolon if needed
   if (sql[sql.size() - 1] != ';')
      sql += ';';

   // Prepare statement
   rc = sqlite3_prepare_v2(mydb, sql.c_str(), -1, &statement, NULL);
   if (rc != SQLITE_OK)
      return false;

   // We need to process prepared statement for any immediate actions;
   // ie, such as dropping a table, inserting a value, or anything that
   // doesn't return a row.
   sqlite3_step(statement);

   // However, had we just had a SELECT, or any query that returns rows,
   // we would miss the first row in fetchOne() as it will do another step.
   // Fortunately, the sqlite API has a 'reset' which just repositions the
   // cursor but does not undo bindings.
   sqlite3_reset(statement);

   return true;
}

/*
* // Leaving this in as a starting point for resumption; however, the API was 
* // unreliable when binding parameters through the sqlite engine, hence
* // this was replaced.
bool gamzia::Cursor::execute (std::string sql, const std::vector<std::string> params)
{
   unsigned int numParams;
   int          rc;

   rc = sqlite3_prepare_v2(mydb, sql.c_str(), -1, &statement, NULL);
   if (rc != SQLITE_OK)
      return false;

   numParams = sqlite3_bind_parameter_count(statement);
   if (params.size() < numParams)
      return false;

   for (unsigned int i = 1; i <= numParams; i++)
   {
      sqlite3_bind_text(statement, i, params[i-1].c_str(), -1, SQLITE_STATIC);
   }

   return (true);
}
*/

/// <summary>
/// If available, fetches a row of data as a vector of string.  This version
/// does not return a map, so no column names are available.
/// </summary>
/// <returns>A vector representing a row of data.</returns>
std::vector<std::string> gamzia::Cursor::fetchOne()
{
   std::vector<std::string> row;

   int rc = sqlite3_step(statement);
   if (rc != SQLITE_DONE)
   {
      int columns = sqlite3_column_count(statement);
      for (int i= 0; i<columns; i++)
      {
         std::stringstream ss;

         const unsigned char* value = sqlite3_column_text(statement, i);

         // Unfortunately, values can be NULL in db.
         // Intercept and substitute space
         if (value == nullptr)
            row.push_back("");
         else
         {
            // Convert to string and vectorize
            ss << value;
            row.push_back(ss.str());
         }
      }
   }
   else
   {
      // No data. Send empty row back.
      row.clear();
   }

   return (row);
}

/// <summary>
/// Returns all records read in a single vector of string.  If the vector
/// has multiple rows, it will continue to place it in the vector.
/// So, field 1 in a 4 field table would be at index 0, 4, 8, 12 etc...
/// Column IDs are included as the first row.
/// </summary>
/// <returns>The entire table as a vector of string.</returns>
std::vector<std::string> gamzia::Cursor::fetchAll()
{
   std::vector<std::string> table;
   std::string name;

   table.clear();

   // Sanity
   if (statement == nullptr)
      return (table);

   int rc = sqlite3_step(statement);
   if (rc != SQLITE_DONE)
   {
      // Get column IDs, but just once
      int columns = sqlite3_column_count(statement);
      for (int i = 0; i < columns; i++)
      {
         name = std::string(sqlite3_column_name(statement, i));
         table.push_back(name);
      }
   }

   while (rc != SQLITE_DONE)
   {
      int columns = sqlite3_column_count(statement);
      for (int i = 0; i < columns; i++)
      {
         std::stringstream ss;

         const unsigned char* value = sqlite3_column_text(statement, i);

         // Unfortunately, values can be NULL in db.
         // Intercept and substitute space
         if (value == nullptr)
            table.push_back("");
         else
         {
            // Convert to string, vectorize
            ss << value;
            table.push_back(ss.str());
         }
      }

      rc = sqlite3_step(statement);
   }

   return (table);
}

/// <summary>
/// Returns the column names in a vector of string.
/// </summary>
/// <returns></returns>
std::vector<std::string> gamzia::Cursor::getColumnNames()
{
   std::vector<std::string> names;

   int rc = sqlite3_step(statement);
   while (rc != SQLITE_DONE)
   {
      int columns = sqlite3_column_count(statement);
      for (int i = 0; i < columns; i++)
      {
         const char* columnName = sqlite3_column_name(statement, i);
         names.push_back(std::string(columnName));
      }
   }

   // Reset for row reading with fetchOne().
   sqlite3_reset(statement);
   return(names);
}




