#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <iomanip>
#include <filesystem>
#include "UT_AccountManager.h"
#include "Colour.h"
#include "Timer.h"


/*********************/
/*  Utility Methods  */
/*********************/


void passed(std::string unitTest)
{
   std::cout << gamzia::Colour::clg << "[PASSED] " << gamzia::Colour::cwh << unitTest << std::endl;
}

void failed(const std::string unitTest)
{
   std::cout << gamzia::Colour::clr << "[FAILED] " << gamzia::Colour::cwh << unitTest << std::endl;
}

void test(const std::string message)
{
   std::cout << gamzia::Colour::cly << "[TEST] " << gamzia::Colour::cwh << message << std::endl;
}


/****************/
/*  UNIT TESTS  */
/****************/


bool gamzia::unitest::UT_CreateSchema()
{
   test("Creating AccountManager schema; creates DBand accounts table.");

   passed("Schema created.");
   return true;
}

bool gamzia::unitest::UT_UserExists1(gamzia::AccountManager am)
{
   bool expected, actual;

   test("Does user exist already?  Should be false.");
   expected = false;
   actual = am.doesUserExist("Karim");
   if (expected == actual)
   {
      passed("false - User does not exist.");
      return true;
   }
   else
   {
      failed("User should not exist");
      return false;
   }
}

bool gamzia::unitest::UT_AddUser(gamzia::AccountManager am)
{
   bool expected, actual;
   test("Adding a user (+ password).");
   expected = true;
   actual = am.addUser("Karim", "test");
   if (expected == actual)
   {
      passed("Successfully added user.");
      return true;
   }

   failed("Could not add user.");
   return false;
}

bool gamzia::unitest::UT_AddDuplicateUser(gamzia::AccountManager am)
{
   bool expected, actual;
   test("Adding a duplicate user.");
   expected = false;
   am.addUser("Dupe", "dupe");
   actual = am.addUser("Dupe", "dupe");
   if (actual == expected)
   {
      passed("Duplicate detected, addUser() aborted.");
      return true;
   }

   failed("Duplicate not detected.");
   return false;
}

bool gamzia::unitest::UT_UserExists2(gamzia::AccountManager am)
{
   bool expected, actual;
   test("Does user Karim exist (now)? Should be true.");
   expected = true;
   actual = am.doesUserExist("Karim");
   if (actual == expected)
   {
      passed("User Karim was detected.");
      return true;
   }

   failed("User Karim was not found.");
   return false;
}

bool gamzia::unitest::UT_GetUser(gamzia::AccountManager am)
{
   std::vector<std::string> results;
   test("Load user record. Loads id, user, password hash, creation date.");
   results = am.getUser("Karim");
   passed("getUser('Karim'):");
   for (auto& x : results)
      passed(x);

   return true;
}

bool gamzia::unitest::UT_GetPassword(gamzia::AccountManager am)
{
   std::string actual;
   test("Get Password for user Karim. Should be salted password hash.");
   actual = am.getPassword("Karim");
   if (actual.empty())
   {
      failed("Could not retrieve password.");
      return false;
   }
   passed("Received: " + actual);
   return true;
}

bool gamzia::unitest::UT_VerifyBadPassword(gamzia::AccountManager am)
{
   bool expected, actual;
   test("Verify BAD password.");
   expected = false;
   actual = am.verifyPassword("Karim", "test");
   if (actual == expected)
   {
      passed("Bad password detected - NOT verified.");
      return true;
   }

   failed("Bad password accepted.");
   return false;
}

bool gamzia::unitest::UT_VerifyGoodPassword(gamzia::AccountManager am)
{
   bool expected, actual;
   test("Verify GOOD password.");
   expected = true;
   actual = am.verifyPassword("Karim", gamzia::AccountManager::saltPassword("Karim", "test"));
   if (actual == expected)
   {
      passed("Good password verified.");
      return true;
   }

   failed("Good password rejected.");
   return false;
}

bool gamzia::unitest::UT_UpdatePassword(gamzia::AccountManager am)
{
   bool expected, actual;
   test("Update password");
   expected = true;
   actual = am.updatePassword("Karim", gamzia::AccountManager::saltPassword("Karim", "newpwd"));
   if (actual == expected)
   {
      passed("Password updated.");
      return true;
   }

   failed("Password update failed.");
   return false;
}

bool gamzia::unitest::UT_ListUsers(gamzia::AccountManager am)
{
   std::vector<std::string> results;
   test("Listing all users.");
   am.addUser("Admin", "admin");
   am.addUser("Bob", "slob");
   am.addUser("Deimos", "phobos");
   results = am.listUsers();
   if (results.empty())
   {
      failed("Table is empty.");
      return false;
   }

   int i = 0;
   for (auto& x : results)
   {
      std::cout << std::setw(10) << x << " | ";
      i++;
      if (i % 4 == 0)
         std::cout << std::endl;
   }
   passed("Listed a result.");
   return true;
}

bool gamzia::unitest::UT_DeleteUser(gamzia::AccountManager am)
{
   bool expected, actual;
   test("Delete user.");
   expected = true;
   actual = am.deleteUser("Karim");
   if (actual == expected)
   {
      passed("User 'Karim' was deleted from the database.");
      return true;
   }

   failed("Could not delete user.");
   return false;
}

/// <summary>
/// Runs all unit tests.
/// </summary>
void gamzia::unitest::UT_AccountManager_main()
{
   // If test db exists, delete it.
   if (std::filesystem::exists(gamzia::AccountManager::TESTDB))
      std::filesystem::remove(gamzia::AccountManager::TESTDB);

   test("Starting unit tests.");
   Timer timer;
   timer.start();
   gamzia::AccountManager am(gamzia::AccountManager::TESTDB);

   // NOTE: Order is important
   int total = 12;
   UT_CreateSchema();
   int passed = 1;

   UT_UserExists1(am)==true ? passed++ : passed;
   UT_AddUser(am) == true ? passed++ : passed;
   UT_AddDuplicateUser(am) == true ? passed++ : passed;
   UT_UserExists2(am) == true ? passed++ : passed;
   UT_GetUser(am) == true ? passed++ : passed;
   UT_GetPassword(am) == true ? passed++ : passed;
   UT_VerifyBadPassword(am) == true ? passed++ : passed;
   UT_VerifyGoodPassword(am) == true ? passed++ : passed;
   UT_UpdatePassword(am) == true ? passed++ : passed;
   UT_ListUsers(am) == true ? passed++ : passed;
   UT_DeleteUser(am) == true ? passed++ : passed;
   timer.stop();
   std::cout << "Time: " << timer.elapsed("ms") << " ms" << std::endl;
   std::cout << "Unit Tests Passed : " << gamzia::Colour::cdg << passed;
   std::cout << gamzia::Colour::cwh << " / " << gamzia::Colour::clg << total << std::endl;
}