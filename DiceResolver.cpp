/*
* Class DiceResolver
* Karim Sultan, September 18, 2022
* 
* This class derives from Resolver (an infix to rpn evaluator).
* It introduces the 'd' operator (dice). It is used as such:
* 
* 3d6   (Roll 3 6-sided dice)
* 1d100 (Roll percentile)
* 17d4  (Roll 17 4-sided dice)
* etc..
* 
* First, this class inserts the 'd' operator at precedence 90 in the
* constructor, and sets the random seed. Second, this class overrides
* "calculateHook" to do the dice calculation. 
* 
* Note: This results in a non-deterministic outcome; reevaluating the
* expression can achieve different results.  For this reason, a new
* method has been introduced, "getHistogram", which provides a statiscal
* report, and a pictorial histogram, of the results.  This is done
* through a heuristic approach based on the number of trials.
*/

#include "DiceResolver.h"
#include <time.h>
#include <sstream>
#include <iostream>
#include <iomanip>

/// <summary>
/// Constructor
/// Use the constructor to add our 'd' - dice - operator to the precedence
/// table, and set the random seed.
/// </summary>
gamzia::DiceResolver::DiceResolver()
{
   srand((int)time(NULL));
   precedence["d"] = 90;
   precedence["D"] = 90;
}

/// <summary>
/// Override
/// Write the virtual method required by base class to handle our dice operator.
/// This is a non-deterministice operation.
/// </summary>
/// <param name="left">The left operand (number of dice)</param>
/// <param name="right">The right operand (dice faces)</param>
/// <param name="op">The dice operator</param>
/// <returns>Long int result</returns>
long int gamzia::DiceResolver::calculateHook(long int left, long int right, std::string op)
{  
   int x=0;

   switch (op[0])
   {
      case 'd':
      case 'D':
         // Dice roll; handled with random number generator
         // NOTE: expressions without 'd' are deterministic;
         // expressions with 'd' are non - deterministic (variable
         // outcomes).

         // Left value is number of rolls; right value is die
         // IE 3d6 = 3 rolls of a 6 sided die, summed.
         for (int i = 0; i < left; i++)
            x += rand() % right + 1;  // 1 to 'right'
         break;

      default:
         break;
   }

   return(x);
}

/// <summary>
/// Heuristic algorithm to calculate expression distribution, ment to be
/// used with dice rolls (ie, 2d6).This is done by repeating rolls
/// to a cap of n trials, then assessing the results.
/// Returns a histogram report with trial results, mean and mode.
/// </summary>
/// <param name="expression">The infix expression</param>
/// <param name="trials">The number of trials (larger = more accurate, but longer).  Max 1,000,000</param>
/// <returns>A report (histogram with mean, mode) as a std::string.</returns>
std::string gamzia::DiceResolver::getHistogram(std::string expression, int trials)
{
   // Sanity bound checking
   if (trials < 0)
      trials = 1;
   else if (trials > MILLION)
      trials = MILLION;
   
   // Initialize
   std::stringstream ss;
   std::map<long int, long int> rolls;
   long int sum = 0, roll = 0;
   double pct=1.0;
   long int mean=0;

   // Mode has two components,
   struct
   {
      long int max = 0;
      long int roll = 0;
   } mode;

   // Statistical report
   for (int i = 0; i < trials; i++)
   {
      if (i==0)
         roll = resolve(expression);
      else
         // We already built the RPN, don't waste cycles
         // reuilding it on every iteration, set repeat = true
         roll = resolve(expression, true);
      
      // We can count on C++ having initialized all map members to zero
      rolls[roll]++;

      // Calculate the mode on the fly
      if (rolls[roll] > mode.max)
      {
         mode.max = rolls[roll];
         mode.roll = roll;
      }

      // Advance sum
      sum += roll;

      // Calculate percentage of all rolls this is (total roll / total rolls)
      pct = float(rolls[roll]) / float(trials) * 100.0;
   }

   mean=(sum/trials);//+0.5;

   ss << "DISTRIBUTION HISTOGRAM (" << trials << " trials):" << std::endl;
   ss << "Mean: " << std::setprecision(2) << mean << std::endl;
   ss << "Mode: " << mode.roll << std::endl;
   
   for (auto& it : rolls)
   {
      // Percentage is occurrence of roll in the number of rolls
      pct=((float)it.second / (float)trials)*100.0;

      ss << "[" << std::setw(3) << it.first << "] ==> " << it.second;
      ss << " (" << std::setprecision(2) << std::fixed << pct << "%)" << std::endl;
   }
   ss << std::endl;

   // Pictorial Report
   // Determine horizontal scale factor, based on 70 columns
   float scale = COLUMNS/((float)mode.max);

   ss << "PICTORIAL HISTOGRAM" << std::endl;

   for (auto &x : rolls)
   {
      ss << "[" << std::setw(3) << x.first << "] ";
      for (int i=0; i<int(x.second*scale); i++)
         ss << "*";
      ss << std::endl;
   }

   // Report is done
   return(ss.str());
}
