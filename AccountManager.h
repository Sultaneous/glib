#pragma once
#include "Sqlite.h"


namespace gamzia
{

   class AccountManager
   {
   public:
      AccountManager();
      AccountManager(std::string dbname);
      ~AccountManager();

      bool doesUserExist(std::string user);
      bool addUser(std::string user, std::string password);
      std::vector<std::string> listUsers();
      std::vector<std::string> getUser(std::string user);
      std::string getPassword(std::string user);
      bool updatePassword(std::string user, std::string password);
      bool deleteUser(std::string user);
      bool verifyPassword(std::string user, std::string saltedPassword);

      static std::string saltPassword(std::string user, std::string password);

      inline static const std::string DBNAME = "accounts.db";
      inline static const std::string TABLENAME = "accounts";
      inline static const std::string TESTDB = "testam.db";

   public:
      

   private:
      Sqlite mydb;
      bool doesTableExist();
      bool createAccountsTable();
   
   };  // Class
   void doUnitTests();
}; // Namespace

void doAMUnitTests();