/*
* Class Timer (c++ version).
* Karim Sultan, ported September 8 2022 from Python.
* A high resolution timer (stopwatch) for performance timing.
* 
* Why this was ported: I find myself doing a lot of C++
* for quick tasks over Python; and a high resolution timer
* is very handy.
* 
* From Python source:
* +++++++++++++++++++
* Timer class - acts as performance stopwatch.
* Use as follows within code:
* *
* Usage:
* from timer import Timer
* Timer timer;
* timer.start()
* ... do measurable stuff ...
* timer.stop()
* std::cout << "Time taken: " << timer.elapsed() << std::endl;
* 
* PYTHON HISTORY:
* KSU 201010 Made field attributes private.  Made elapsed() stop
*            the timer if it is still running.  Added sanity checks.
*            Added test cases.  Added some robustness.
* KSU 201025 Added units to elapsed(). Updated test cases.  Fixed a previously
*            uknown and  rather insidious bug in timer.stop() which didn't check
*            if it had already been stopped, and would update elapsed time errantly.
* KSU 201028 Added support for nanoseconds and microseconds in timer.elapsed().
*            Updated unit test for elapsed time accordingly.
*            NOTE: The µ utf-8 character in windows is ALT-230.
* KSU 201103 Two steps forward one step back... units function broke peek().
*            Unit tests caught error. Fixed.
* 
* A note on timers...
* Timers are OS dependent.  This will not work on UNIX.
* The OS is responsible for setting up a high resolution timer that is stable.
* The timer frequency may vary. The OS measures passage of time in ticks.  The 
* frequency provides the number of ticks per second.  Dividing the high resolution
* counter (ticks) by the number of ticks per second will give you the number of
* seconds.  It is possible to take a measurement at the nanosecond level, but
* drift due to the execution time of the start() and stop() method become noticeable.
* The windows API routines QueryPerformanceCounter and QueryPerformanceFrequency
* provide these values.  This Timer class queries a start and stop time and converts
* elapsed time to a unit (ns, µs, ms, s, m, h, d) with minimal code.
* The peek method lets you view elapsed time without stopping the timer first.
* The elapsed method requires you stop the timer prior or it returns 0.
*/

#include "Timer.h"
#include <iostream>
#include <string>
#include <time.h>
#include <stdlib.h>
#include <Windows.h>

/// <summary>
/// Constructor resets key members.
/// </summary>
gamzia::Timer::Timer()
{
   isRunning = false;
   timeStart.QuadPart = 0;
   timeStop.QuadPart = 0;
   frequency.QuadPart = 0;
   timeElapsed.QuadPart = 0;
}

/// <summary>
/// Starts the timer
/// </summary>
void gamzia::Timer::start(void)
{
   isRunning = true;
   QueryPerformanceFrequency(&frequency);
   QueryPerformanceCounter(&timeStart);
}

/// <summary>
/// Stops the timer
/// </summary>
void gamzia::Timer::stop(void)
{
   QueryPerformanceCounter(&timeStop);
   isRunning=false;
}

/// <summary>
/// Returns elapsed time in your choice of units.The timer
/// must be stopped first before calling this, or it returns 0.
/// Specifying a unit returns the elapsed time in that format.
/// Format specifiers(long, medium and short form are valid) :
///    ns - nano - nanoseconds
///    µs - micro - microseconds
///    ms - milli - milliseconds
///    s - sec - seconds
///    m - min - minutes
///    h - hrs - hours
///    d - days
/// </summary>
/// <param name="units">A format prefix</param>
/// <returns>The elapsed time in the specified units</returns>
double gamzia::Timer::elapsed(std::string units)
{
   if (isRunning)
	  return (0);

   timeElapsed.QuadPart = timeStop.QuadPart - timeStart.QuadPart;

   // We have to divide the number of ticks by the number of ticks per second.
   double delta;
   delta = (double)timeElapsed.QuadPart;
   delta = delta / frequency.QuadPart;

   return(convertUnits(delta, units));
}

/// <summary>
/// Peeks at the current timer value.Timer does not need to be stopped,
/// but must have been started, or it returns 0.
/// Specifying a unit returns the elapsed time in that format.
/// Format specifiers(long, medium and short form are valid) :
///    ns - nano - nanoseconds
///    µs - micro - microseconds
///    ms - milli - milliseconds
///    s - sec - seconds
///    m - min - minutes
///    h - hrs - hours
///    d - days
/// </summary>
/// <param name="units">A format prefix</param>
/// <returns>The time elapsed *so far*</returns>
double gamzia::Timer::peek(std::string units)
{
   if (isRunning)
   {
      LARGE_INTEGER interim;
	  QueryPerformanceCounter(&interim);
	  timeElapsed.QuadPart = interim.QuadPart - timeStart.QuadPart;

	  // Divide tick count by number of ticks per second
	  double delta;
	  delta = (double)timeElapsed.QuadPart;
	  delta = delta / frequency.QuadPart;

      return (convertUnits(delta, units));
   }
   else return(0);
}

/// <summary>
/// Private method which does the heavy lifting to format to a unit.
/// Called by elapsed() and peek().
/// </summary>
/// <param name="delta">The elapsed time, in seconds (ie, ticks divided
/// by ticks per second)</param>
/// <param name="units">The unit prefix</param>
/// <returns>The converted seconds value</returns>
double gamzia::Timer::convertUnits(double delta, std::string units)
{
   double nanoseconds = (double)delta * 1000 * 1000 * 1000;
   double microseconds = (double)delta * 1000 * 1000;
   double milliseconds = (double)delta * 1000;
   double seconds = (double)delta;
   double minutes = (double)delta / 60;
   double hours = minutes / 60;
   double days = hours / 24;

   if (units == "ns" || units == "nano" || units == "nanoseconds")
	  return (nanoseconds);
   else if (units == "µs" || units == "micro" || units == "microseconds")
	  return (microseconds);
   else if (units == "ms" || units == "milli" || units == "milliseconds")
	  return (milliseconds);
   else if (units == "s" || units == "sec" || units == "seconds")
	  return (seconds);
   else if (units == "m" || units == "min" || units == "minutes")
      return (minutes);
   else if (units == "h" || units == "hrs" || units == "hours")
      return (hours);
   else if (units == "d" || units == "days")
      return (days);
   else
	  return(0);
}
