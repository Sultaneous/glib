# Gamzia Library for C++

A C++ library of various useful classes including ANSI colours, a stopwatch, a dice resolver, an SQLite wrapper, and more...
This is a module for C++, containing various specialized utilities.  This document describes the contents and provide examples of usage.
This is similar to the Gamzia library for Python, but it is much faster.  Syntax differeneces are detailed here.

## Install

Installing is simple.  Clone the directory onto your harddrive:

```dos
prompt> git clone https://github.com/Sultaneous/glib.git
```

Next, start a C++ project and include the header file and C++ file as per the import section below.
Then, use the API calls detailed in this document. Happy Coding!
NOTE: If you are using the **sqlite** class, you will require the **sqlite3.dll** library as well.
The gamzia library makes use of the most modern C++ version.  At the time of this writing, this was
version 0x20.

## Import

This is not an all-in-one type library.  To use a module in your Python program, simply import the module's ".h" file, and ".cpp" file.  You will 
need to include the ".cpp" file as well as the header.

(Example to use the timer module; see below for details)

```c++
#include "timer.h"
#include "timer.cpp"
```

From this point on, you may use the Timer API in your code.

## Modules

| Module | Classes | Summary |
| :------ | :------- | :------- |
| [timer](#info_timer) | Timer | A high performance timer, stopwatch style, for timing code execution and the like |
| [colour](#info_colour) | Colour | Contains ANSI colour codes for adding colour to text |
| [accountmanager](#info_accountmanager)  | AccountManager | An SQLITE based user/password manager, using salted hashes, for authentication purposes. |
| [resolver](#info_resolver) | Resolver | A very fast Reverse Polish Notation generator and resolver, with order of operations. |
| [diceresolver](#info_diceresolver) | DiceResolver | An example of how to subclass Resolver. This module implements dice rolls ("1d6", "2d8", "3d17") into the order of operations. |
| [sha256](#info_sha256) | SHA256 | An implemntation of the SHA256 algorithm. |
| [sqlite](#info_sqlite) | Sqlite | A fast, sqlite3 wrapper for databases. |

---

## API Documentation

### <a id="info_timer">Timer</a>

The timer module provides the Timer class, a simple to use high performance timer.

#### Usage examples:
``` c++
#include "Timer.h"
#include "Timer.cpp"

Timer my_timer=Timer()
my_timer.start();
// ... do logic here...
my_timer.stop();

// Report seconds used:
std::cout << "Time taken: " << my_timer.elapsed() << std::endl;

// Report microseconds; could also use unit=micros/
std::cout << "Time taken: " << my_timer.elapsed("µs") << std::endl;

// Report hours; could also use unit=h
std::cout << "Time taken: " << my_timer.elapsed("hours") << std::endl;
```

#### Methods
| Method | Parameters | Returns | Summary |
|:-----|:--------|:-------|:-------|
| Timer() | None | Class instance | Constructor |
| start() | None | nothing | Starts the timer |
| stop() | None | nothing | Stops the timer |
| double peek() | **Optional** unit=seconds; choices are "ns\|µs\|ms\|s\|m\|h\|d" or "nanoseconds\|microseconds\|milliseconds\|seconds\|minutes\|hours\|days" | Returns the current elapsed time in seconds without stopping the timer.  Returns 0 if timer hasn't been started. | Peeks at current time elapsed on timer |
| double elapsed() | **Optional** unit=seconds; choices are "ns\|µs\|ms\|s\|m\|h\|d" or "nanoseconds\|microseconds\|milliseconds\|seconds\|minutes\|hours\|days" | Returns the final elapsed time in seconds, or in the requested unit type. | Stops timer if it is still running. Use peek() to get time interval without stopping timer. |

---

### <a id="info_colour">Colour</a>

Colour provides ANSI colour codes for formatting text strings with colour adornments. 

**NOTE: In Windows 10, ANSI colour is NOT supported in DOS CMD nor PowerShell terminals - but it can be enabled. 
Not all styles are supported by terminals; YMMV.**

Colours are identified with a 3-4 letter foreground code (**c**xx), background code (**b**xx), or style code (**s**xx).  Colours
can be turned off (reverting to terminal default) with "**coff**" (Colour OFF), "**boff**" (Background OFF), or the generic "**off**" (foreground
& background off). Styles need to be turned off individually.

**NOTE:** ANSI colouring supports **16 colours** as listed below:

![Colour List](https://github.com/Sultaneous/gamziatools/blob/main/docs/colour_list.png)

**NOTE:** You can have 1 active foreground colour, 1 active background colour, and multiple active styles (for example, bold and underline).

#### Usage examples:
``` c++
#include "Colour.h"
#include "Colour.cpp"

Colour C = Colour()
std::cout << C.clc << "This is in light cyan." << C.off << "This is back to normal." << std::endl;
std::cout << C.cly << C.bdr << "This is light yellow foreground on a dark red background." << std::endl;

s= C.clb + "This is blue text and this is "+C.clg + C.bdg + "green on dark green."
std::cout << "With colour: " << s << std::endl; 
std::cout << "Without colour: " << Colours.cstrip(s) << std::endl;
```

Note that to effectively use text colouring, you simply insert the text format string in the appropriate location in the string to enable it.
One does not need to turn off the colour before applying a new colour; the previous colour/background/style remains in effect until overwritten or turned off.

**Note:** All format strings can be accessed statically, ie: Colours.clr, etc... and no instantiation of the Colours class is necessary.

#### Methods

| Method | Parameters | Summary |
|:-----|:--------|:-------|
| cstrip | Colour formatted string | This method strips all ANSI colour codes out of a string, returning the 'clean' string. It is a static method so can be called without a class instance, ie: Colour.cstrip()|

#### Foreground (Text) Colour Strings
| String | Meaning |
|:------ | :------ |
| cbl | Foreground colour black |
| cdr | Foreground colour dark red |
| cdg | Foreground colour dark green |
| cdy | Foreground colour dark yellow / brown |
| cdb | Foreground colour dark blue |
| cdm | Foreground colour dark magenta |
| cdc | Foreground colour dark cyan |
| cdgy | Foreground colour dark gray |
| clgy | Foreground colour light gray |
| clr | Foreground colour light red |
| clg | Foreground colour light green |
| cly | Foreground colour light yellow |
| clb | Foreground colour light blue |
| clm | Foreground colour light magenta |
| clc | Foreground colour light cyan |
| cwh | Foreground colour light white |
| **coff** | Foreground colour **off** |

#### Background (Reverse) Colour Strings
| String | Meaning |
|:------ | :------ |
| bbl | Background colour black |
| bdr | Background colour dark red |
| bdg | Background colour dark green |
| bdy | Background colour dark yellow / brown |
| bdb | Background colour dark blue |
| bdm | Background colour dark magenta |
| bdc | Background colour dark cyan |
| bdgy | Background colour dark gray |
| blgy | Background colour light gray |
| blr | Background colour light red |
| blg | Background colour light green |
| bly | Background colour light yellow |
| blb | Background colour light blue |
| blm | Background colour light magenta |
| blc | Background colour light cyan |
| bwh | Background colour light white |
| **boff** | Background colour **off** |

#### Style (Effect) Colour Strings
| String | Meaning |
|:------ | :------ |
| sbo | Style Bold |
| sdi | Style Dim |
| sun | Style Underline |
| sbl | Style Blink |
| sre | Style Reverse |
| shi | Style Hidden |
| sbof | Style Bold Off |
| sdif | Style Dim Off |
| sunf | Style Underline Off |
| sblf | Style Blink Off |
| sref | Style Reverse Off |
| shif | Style Hidden Off |
| **soff** | Style effect **off** |

#### Other (Macro) Colour Strings
| String | Meaning |
|:------ | :------ |
| off | Foreground & Background off |
| no | Foreground light yellow, background dark red |
| yes | Foreground light green, background dark green |
| old | Foreground light yellow, background dark blue |
| retro | Foreground white, background black |
| paper | Foreground black, background white |

![Colour List](https://github.com/Sultaneous/gamziatools/blob/main/docs/colour_list.png)

---

### <a id="info_accountmanager">AccountManager</a>

The AccountManager class is intended to provide a simple way to incorporate authentication into an app. For example, in a client-server paradigm,
a login may be required prior to providing services.  Account management is a repetitive pattern that is covered with this class.

The programmer can quickly incorporate a user database with salted hash passwords.  For security reasons, the actual or "plain-text" password is not
stored. Instead, the SHA256 secure hash algorithm is used to create a hash, which is formed from the input of the user name (the salt) and the password.
This hash is stored in the database, and for future comparisons, the salted hash is first recreated and then the hashes are compared.

AccountManager uses an sqlite database, which is a local binary file.  Queries employ the use of parameterization to harden it against SQL injection
attacks.  The programmer provides the database name, hence a database can be isolated to a single app or shared among several apps.

The **sqlite3 library (sqlite3.dll)** must be available to your program first to use this class.

#### Usage examples:
``` c++
#include "AccountManager.h"
#inlcude "AccountManager.cpp"

// Create a schema / or open existing one
mgr = AccountManager("users.db");

// Add three test users
mgr.addUser("Admin","Adminpassword");
mgr.addUser("Guest","Anonymous");
mgr.addUser("Test","debug");

// Load all users
std::cout << mgr.listUsers() << std::endl;

// Create a saltedhash to compare input with DB and verify password
saltedhash = AccountManager.saltPassword("Guest","Anonymous");
if (mgr.verifyPassword("Guest",saltedhash))
   std::cout << "Password verified" << std::endl;
else
   std::cout << "Verification failed" << std::endl;

// Change the password on account "Test", load before and after
std::cout << mgr.getUser("Test") << std::endl;
mgr.updatePassword("Test","newpassword");
std::cout << mgr.getUser("Test") << std::endl;

// Remove user "Test" from db
mgr.deleteUser("Test")
```

**NOTE:** User names are unique, case insensitively. Thus "User" and "user" are considered the same and not permitted.  This constraint is
enforced at the database level as part of the schema.  User name cannot be NULL.  Password cannot be NULL.

#### Methods

| Method | Parameters | Returns | Summary |
|:-----|:--------|:-------|:-------|
| AccountManager() | **_(Optional)_** Database file name.  If not provided, uses default, "**accounts.db**" | Class instance | Constructor.  Takes optional db file name.  If the db does not exist, it creates the schema, otherwise it opens it for read/write access. |
| doesUserExist() | string user name | True if user exists in database, False otherwise | Checks if a user already exists in database. |
| addUser() | string user name; string password | True on success, False otherwise | Adds a new user record with salted, hashed password to database. |
| listUsers() | **None** | Returns a list of strings, where each line is the equivalent of 1 record from the accounts table. | The fields in a record are ID (integer), Username (String), Password (String) and CreationDate (String, date/time format).  If the table is empty, an empty list is returned. |
| getUser() | string user name | Returns a string containing the complete user record, or **NULL** if no record found. | Always check for **NULL** in case user wasn't in the db. This can be avoided with a call to doesUserExist() before calling getUser(). |
| getPassword() | string user name | Returns the password entry for the specified user, or **NULL** if user not found. |  Password value is the salted, SHA256 hash value of the original password. Plain-text passwords are not stored.  Passwords are salted with the User name (the salt value is not secret). |
| updatePassword() | string user name, string newpassword | **True** on success, **False** otherwise. | **NOTE** The class will salt and hash the password, so you need only provide the plain-text version of the password to this method. |
| deleteUser() | string user name | **True** if successful, **False** otherwise | Deletes user from database.  **NOTE** If user doesn't exist, it returns False as nothing was deleted. Again, the programmer can avoid ambiguity on the meaning of a False return value (ie, user didn't exist, or DB error occurred) by first calling doesUserExist(). |
| verifyPassword() | string user name, string value to test | Returns True if user's salted hash password value matches the value provided, False if not. | **NOTE** This requires that the value you test is the salted pasword hash and not the plain text password.  See saltPassword() below for how to do this, and see the usage examples above for a code example. |
| **Static** AccountManager.saltPassword() | string user name, string plain-text password | Returns a string containing the salted, SHA256 password hash | **NOTE** This is a static method that can be used anytime without a class instance.  The user name is required, as it is the salt value that is used (the salt value is not secret).|

---

