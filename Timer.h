#pragma once
#include <Windows.h>
#include <string>

namespace gamzia
{

   class Timer
   {
   public:
	  Timer();
	  void start(void);
	  void stop(void);
	  double peek(std::string units);
	  double elapsed(std::string units);

   private:
	  LARGE_INTEGER timeStart = {static_cast<LONGLONG>(0)};
	  LARGE_INTEGER timeStop = {static_cast<LONGLONG>(0)};
	  LARGE_INTEGER frequency = {static_cast<LONGLONG>(0) };;
	  LARGE_INTEGER timeElapsed = {static_cast<LONGLONG>(0) };;
	  bool isRunning = false;

	  double convertUnits (double delta, std::string units);
   }; // class

}; // namespace