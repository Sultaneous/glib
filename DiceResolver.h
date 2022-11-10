
#pragma once
#include "Resolver.h"

namespace gamzia
{
   class DiceResolver : public Resolver
   {
   public:
      DiceResolver();
      std::string getHistogram(std::string expression, int trials);
      long int calculateHook(long int left, long int right, std::string op);

      int COLUMNS = 70;

   private:
      inline static const int MILLION = 1000000;

   }; // class

}; // namespace

