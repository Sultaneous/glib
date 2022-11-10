#pragma once
#include <string>
#include <ostream>
#include "sqlite3.h"
#include <map>
#include <string>
#include <vector>

// Forward declaration
class Sqlite;
class Cursor;

namespace gamzia
{
   class Cursor
   {

   public:
      Cursor(sqlite3 *db);
      ~Cursor();
      bool execute(std::string sql);
      bool execute(std::string sql, const std::vector<std::string> params);
      std::vector<std::string> fetchOne();
      std::vector<std::string> fetchAll();
      std::vector<std::string> getColumnNames();
      bool doesTableExist(std::string table);
      bool commit();
      bool rollback();
      bool vacuum();

   private:
      sqlite3      *mydb;
      sqlite3_stmt *statement;
   };

   class Sqlite
   {

   public:
      Sqlite();
      Sqlite(std::string dbname);
      ~Sqlite();
      bool connect();
      void close();
      std::string getLastError();
      Cursor getCursor();
   
   private:
      std::string mydbname;
      sqlite3     *mydb;
      bool        isConnected;
      std::string myerror;
   }; // class

}; // Namespace

