#pragma once
#include <string>
#include <stack>
#include <queue>
#include <map>

namespace gamzia
{

   class Resolver
   {

   public:
      bool error;
      Resolver();
      std::string infixToRPN (std::string expression);
      long int evaluateRPN (void);
      long int resolve (std::string expression, bool repeat=false);
      virtual long int calculateHook (long int left, long int right, std::string op);

   private:
      std::stack<char> s;
      std::queue<std::string> q;
      long int calculate (long int left, long int right, std::string op);
      long int factorial (long int x);
      long int choose (long int n, long int r);

   protected:
      std::map<std::string, int> precedence =
      {
         { "(", 0},
         { "-", 30},
         { "+", 30},
         { "/", 50},
         { "*", 50},
         { "%", 50},
         { "C", 50},
         { "c", 50},
         { "^", 70},
         { "!", 80}
      };
   };  // class

}; // namespace