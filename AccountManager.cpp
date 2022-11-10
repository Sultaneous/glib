#include "AccountManager.h"
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <time.h>
#include <iomanip>
#include <boost/algorithm/string.hpp>
#include <filesystem>
#include "Sqlite.h"
#include "SHA256.h"


/// <summary>
/// Default constructor.  Use this one for general use.
/// </summary>
gamzia::AccountManager::AccountManager()
{
   mydb = gamzia::Sqlite(DBNAME);
   mydb.connect();
   if (!doesTableExist())
      createAccountsTable();
}


/// <summary>
/// Optional constructor takes the name of a DB to use.
/// </summary>
/// <param name="dbname">The name of the database to use.</param>
gamzia::AccountManager::AccountManager(std::string dbname)
{
   mydb = gamzia::Sqlite(dbname);
   mydb.connect();
   if (!doesTableExist())
      createAccountsTable();
}


/// <summary>
/// Deconstructor does cleanup.
/// </summary>
gamzia::AccountManager::~AccountManager()
{
   mydb.close();
   mydb.~Sqlite();
}


/// <summary>
/// An internal test to look for the accounts table.
/// </summary>
/// <returns>True if table schema exists.</returns>
bool gamzia::AccountManager::doesTableExist()
{
   gamzia::Cursor k = mydb.getCursor();
   return (k.doesTableExist(TABLENAME));
}


/// <summary>
/// Creates the accounts table.  Only used for new dbs.
/// Sets username column to unique, case insensitive.
/// </summary>
/// <returns>True on success.</returns>
bool gamzia::AccountManager::createAccountsTable()
{
   std::string sql;

   sql = "CREATE TABLE accounts (id INTEGER PRIMARY KEY AUTOINCREMENT, " \
      "user TEXT UNIQUE COLLATE NOCASE, password, created)";
   gamzia::Cursor k = mydb.getCursor();
   if (!k.execute(sql))
      return false;
   return (k.commit());
}

/// <summary>
/// Returns true if user account exists in table.
/// </summary>
/// <param name="user">The user name to verify.</param>
/// <returns>True if user exists, false otherwise.</returns>
bool gamzia::AccountManager::doesUserExist(std::string user)
{
   std::string sql;
   std::vector<std::string> params;
   std::vector<std::string> row;

   sql = "SELECT count(user) FROM accounts WHERE user=?";
   params.clear();
   params.push_back(user);
   gamzia::Cursor k = mydb.getCursor();
   k.execute(sql, params);
   row = k.fetchOne();
   if (row.empty())
      return false;

   if (row[0] == "0")
      return(false);

   return true;
}

/// <summary>
/// C-RUD Adds a user. Salts the password.
/// </summary>
/// <param name="user">User name to add.</param>
/// <param name="password">Plaintext password.</param>
/// <returns>True on success, false otherwise.</returns>
bool gamzia::AccountManager::addUser(std::string user, std::string password)
{
   std::string sql;
   std::string saltedPassword;
   std::vector<std::string> params;
   time_t now;

   if (doesUserExist(user))
      return false;

   now = time(0);
   saltedPassword = AccountManager::saltPassword(user, password);
   sql = "INSERT INTO accounts (user, password, created) VALUES (?, ?, ?)";
   params.clear();
   params.push_back(user);
   params.push_back(saltedPassword);
   params.push_back(std::to_string(now));
   gamzia::Cursor k = mydb.getCursor();
   if (!k.execute(sql, params))
      return (false);
   return (k.commit());
}


/// <summary>
/// C - R - UD -> Returns all records in account database as a vector. 
/// </summary>
/// <returns>The table; or an empty list if no records available.</returns>
std::vector<std::string> gamzia::AccountManager::listUsers()
{
   std::string sql;
   std::vector<std::string> table;

   gamzia::Cursor k = mydb.getCursor();
   sql = "SELECT * FROM " + TABLENAME;
   k.execute(sql);
   table = k.fetchAll();
   return (table);
}


/// <summary>
/// C - R - UD -> returns a user record as a vector of string.
/// </summary>
/// <param name="user">The user name to load.</param>
/// <returns>The user record, if it exists; empty otherwise.</returns>
std::vector<std::string> gamzia::AccountManager::getUser(std::string user)
{
   std::string sql;
   std::vector<std::string> params;

   gamzia::Cursor k = mydb.getCursor();
   sql = "SELECT * FROM ? WHERE user=?";
   params.clear();
   params.push_back(TABLENAME);
   params.push_back(user);
   k.execute(sql, params);
   return (k.fetchOne());
}


/// <summary>
/// # C-R-UD -> Loads password field only, for a given user account.
/// </summary>
/// <param name="user">The user account to access.</param>
/// <returns>The password, or empty string if none.</returns>
std::string gamzia::AccountManager::getPassword(std::string user)
{
   std::string sql;
   std::vector<std::string> params;
   std::vector<std::string> record;

   gamzia::Cursor k = mydb.getCursor();
   sql = "SELECT password FROM ? WHERE user=?";
   params.clear();
   params.push_back(TABLENAME);
   params.push_back(user);
   k.execute(sql, params);
   record = k.fetchOne();
   if (!record.empty())
      return (record[0]);
   
   // No password, or user not found
   return "";
}


/// <summary>
/// CR - U - D -> Updates user record password
/// </summary>
/// <param name="user">The user account to update.</param>
/// <param name="password">The plaintext password.</param>
/// <returns></returns>
bool gamzia::AccountManager::updatePassword(std::string user, std::string password)
{
   std::string sql;
   std::string saltedPassword;
   std::vector<std::string> params;

   saltedPassword = AccountManager::saltPassword(user, password);
   gamzia::Cursor k = mydb.getCursor();
   sql = "UPDATE ? SET password=? WHERE user=?";
   params.clear();
   params.push_back(TABLENAME);
   params.push_back(saltedPassword);
   params.push_back(user);
   if (!k.execute(sql, params))
      return false;
   return (k.commit());
}


/// <summary>
/// CRU - D -> Deletes record by user name
/// </summary>
/// <param name="user">The user name.</param>
/// <returns>True on success.</returns>
bool gamzia::AccountManager::deleteUser(std::string user)
{
   std::string sql;
   std::vector<std::string> params;

   gamzia::Cursor k = mydb.getCursor();
   sql = "DELETE FROM ? WHERE user=?";
   params.clear();
   params.push_back(TABLENAME);
   params.push_back(user);
   if (!k.execute(sql, params))
      return (false);

   // Commit and reclaim the file space
   if (!k.commit())
      return (false);
   k.vacuum();

   // Success
   return true;
}


/// <summary>
/// Compares a salted password hash to the one in the DB.
/// DO NOT PROVIDE A PASSWORD AS THE PARAMETER.
/// A PRE - SALTED, PRE - HASHED STRING IS EXPECTED (ie, the client program
/// should never provide an actual password; everything is done with
/// irreversible salted hashes for security purposes.
/// </summary>
/// <param name="user">The user account to test.</param>
/// <param name="saltedPassword">The salted password to test.</param>
/// <returns>True on match, false otherwise.</returns>
bool gamzia::AccountManager::verifyPassword(std::string user, std::string saltedPassword)
{
   std::string hash;

   // Get password from DB, check it against supplied
   hash = getPassword(user);
   if (hash == saltedPassword)
      return true;

   // Anything else is a failure
   return false;
}


/// <summary>
/// Applies the salt formula and produces the SHA256 hash.
/// </summary>
/// <param name="user">The user account.  Used in salt formula.</param>
/// <param name="password">The plaintext password.</param>
/// <returns>A salted SHA526 hash.</returns>
std::string gamzia::AccountManager::saltPassword(std::string user, std::string password)
{
   std::string presalt;
   std::string saltedHash;

   presalt = user + password + user;
   saltedHash = sha256(presalt);
   boost::algorithm::to_lower(saltedHash);
   return (saltedHash);
}



