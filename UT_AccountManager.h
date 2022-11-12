#pragma once
#include "AccountManager.h"

namespace gamzia::unitest
{
   bool UT_CreateSchema();
   bool UT_UserExists1(gamzia::AccountManager am);
   bool UT_AddUser(gamzia::AccountManager am);
   bool UT_AddDuplicateUser(gamzia::AccountManager am);
   bool UT_UserExists2(gamzia::AccountManager am);
   bool UT_GetUser(gamzia::AccountManager am);
   bool UT_GetPassword(gamzia::AccountManager am);
   bool UT_VerifyBadPassword(gamzia::AccountManager am);
   bool UT_VerifyGoodPassword(gamzia::AccountManager am);
   bool UT_UpdatePassword(gamzia::AccountManager am);
   bool UT_ListUsers(gamzia::AccountManager am);
   bool UT_DeleteUser(gamzia::AccountManager am);

   void UT_AccountManager_main();
}