/*
* Class Resolver
* Karim Sultan, September 11, 2022 - Ported from Python implementation
* 
* NOTE:
* The C++ version separates a mathematical resolver and the specialized
* dice case.  The DiceResolver is implemented as a derived class.
* 
* From Python source:
* +++++++++++++++++++
* Ported from Python 2021 08 12
* Adapted from C# version, DiceResolver.cs by Karim Sultan, 2019.
* Integer based Reverse Polish Notation resolver.
* Converts infix notation to RPN notation.
* Evaluates outcome.
* Enforces order of operations, and handles brackets.
* Does not work with floating point. Although trivial to adjust for floating point,
* the use of integers only was a design decision.
*
* Typical use:
* Math: "(12+2^3)/10*8%5" would resolve to 1.
* Faulty: "(9*7" = 0 or "*oas" = 0 (error always returns 0).
*
* The Resolver class is an infix expression resolver,
* which handles order of operations (Using the Canadian BEDMAS rule
* and not the American PEMDAS rule - although the outcomes are
* necessarily the same under either rule, there is an implemntation
* difference).  NOTE: Current implementation doesn't care - uses both.
*
* History (Python)
* KSU 210814 Added '!' (factorial) operator; precedence is debatable,
*            since it isn't in the BEDMAS rule, but mathematicians peg
*            it between brackets and exponents.
*
* KSU 210814 Added "C" for choose (combinatorics).  Set the priority
*            to the same as multiplication (via formula expansion).
*
* KSU 210816 Added getHistogram() which provides a heuristic analysis
*            of a particular dice roll.
*/

#include "Resolver.h"
#include <string>
#include <stack>
#include <queue>
#include <ctype.h>
#include <math.h>


/*
* Constructor
* 
* Clears working data structrures.
*/
gamzia::Resolver::Resolver()
{
   while (!s.empty())
      s.pop();

   while (!q.empty())
      q.pop();

   error=false;
}

/*
* Converts a valid infix expression(mathematical expression)
* to postfix using Reverse Polish Notation(RPN).Infix exp -
* ression must be valid; this function can not check validi -
* ty.Note that by design, this only supports integer expr -
* ession(no floating point support).FP support can be add -
* ed if while building numbers, the '.' character is accepted.
*
* Example: Expression = "1 + 2 * 3"  -- > 7, NOT 9
* RPN = "1 2 3 * +"  -- > 7
* Note that the order of operations is preserved in the RPN.
*/

std::string gamzia::Resolver::infixToRPN(std::string expression)
{
   // Since a number may be multiple characters, we start with an empty string,
   // and while each character is numeric, we append the number until a non -
   // numeric value is encountered.
   std::string num = "";

   // Tokenize expression character by character
   for (char c : expression)
   {
      char token=c;

      // Case : we had accumulated a number but this character is not a
      // numeric value; so save accumulated number, and reset accumulator.
      if (num != "" and !isdigit(token))
      {
         q.emplace(num);
         num = "";
      }

      // We aren't a number; so handle the token
      // '(' start brackets are simply markers of what point to return to when
      //  # a ')' close bracket is encountered.
      if (token=='(')
         s.push(token);

      // Special case; we look for this first->it means we have to pop all
      // previous values off stack into the RPN queue until we find the '('
      else if (token==')')
      {
         // pop up until the bracket
         while (s.top() != '(')
         {
            q.push(std::string(1, s.top()));
            s.pop();
         }

         // pop the bracket / throw it away(it was just a marker, we're done with it)
         s.pop();
      }

      // Case: Operator handling
      // We are done handling brackets, check for a valid operator
      else if (precedence.find(std::string(1,token)) != precedence.end())
      {
         while (!s.empty() && precedence[std::string(1, token)] <= precedence[std::string(1, s.top())])
         {
            q.push (std::string(1, s.top()));
            s.pop();
         }
         s.push(token);
      }

      // Case: character is numeric.
      // Append to accumulator and continue parsing
      else if(isdigit(token))
         num += token;
   } // for

   // Did token end on a number ? If so store accumulated number in RPN queue
   if (num != "")
      q.push(num);

   // Now pop items from stack to the queue to cleanup
   while (!s.empty())
   {
      q.push(std::string(1, s.top()));
      s.pop();
   }

   // At this point, we have a valid RPN in the 'q' queue
   // (if the infix expression was valid)
   // Let's return a string version:
   std::queue<std::string> q_copy(q);
   std::string rpn = "";

   while (!q_copy.empty())
   {
      rpn += q_copy.front() + " ";
      q_copy.pop();
   }

   return (rpn);
}

/// <summary>
/// Calculates a factorial to x.
/// </summary>
/// <param name="x">Factorial to calculate</param>
/// <returns>The factorial</returns>
long int gamzia::Resolver::factorial(long int x)
{
   long int product=1;

   if (x<0)
      return 1;
   else if (x==0 || x==1)
      return 1;

   while (x>1)
   {
      product *= x;
      x--;
   }

   return (product);
}

/// <summary>
/// Routine to calculate "choose" (combinatorics)
/// Formula:
/// nCr(n Choose r) = n!/ r!(n - r)!
/// </summary>
/// <param name="n">the number of options</param>
/// <param name="r">the value to choose</param>
/// <returns>The combinatorics result; 0 on error</returns>
long int gamzia::Resolver::choose(long int n, long int r)
{
    // Sanity
   if (n<r)
      return (0);

   long int numerator = factorial(n);
   long int denominator = factorial(r) * factorial(n-r);

   // Sanity
   if (denominator == 0)
      return (0);

   // Finalize
   return (numerator / denominator);
}

/// <summary>
/// Given the left hand operand, the right hand operand, and an operator,
/// calculates the result.
/// </summary>
/// <param name="left">The left operand</param>
/// <param name="right">The right operand</param>
/// <param name="op">The operator</param>
/// <returns>The result of the calculation left(op)right</returns>
long int gamzia::Resolver::calculate(long int left, long int right, std::string op)
{
   long int x = 0;
   char o = op[0];

   switch (o)
   {
      case '+':
         x = left + right;
         break;

      case '-':
         x = left - right;
         break;

      case '*':
         x = left * right;
         break;

      case '/':
         x = left / right;
         break;

      case '^':
         x = (long int)pow(left, right);
         break;

      case '%':
         x = left % right;
         break;

      case '!':
         x = factorial(left);
         break;

      case 'c':
      case 'C':
         x = choose (left, right);
         break;

      default:
         x=0;
         break;
   }

   return (x);
}

/// <summary>
/// calculateHook is a virtual function that must be overridden by any
/// derived class.  It allows the derived class to implement a calculation
/// method, which is called after the main calculations are done.
/// </summary>
/// <param name="left">The left operand</param>
/// <param name="right">The right operand</param>
/// <param name="op">The operator</param>
/// <returns>A long int response to the calculation, 0 on error</returns>
long int gamzia::Resolver::calculateHook (long int left, long int right, std::string op)
{
   return (0);
}

/// <summary>
/// Nifty little stack and queue algorithm for evaluating
/// the RPN.  Expects a valid RPN expression.
/// </summary>
/// <returns>The integer response</returns>
long int gamzia::Resolver::evaluateRPN ()
{
   std::stack<long int> workstack;

   // As we pull tokens from the queue, we validate them and if neither
   // a number nor an operator, we abort with an exception.
   std::queue<std::string> q_copy(q);
   std::string t;
   long int right, left;

   while (!q_copy.empty())
   {
      t = q_copy.front();
      q_copy.pop();

      if (precedence.find(t) != precedence.end())
      {
         // As we work backwards, right value is first, then left
         right = workstack.top();
         workstack.pop();

         // Special case: ! factorial only requires one term
         if (t == "!")
            left = right;
         else
         {
            left = workstack.top();
            workstack.pop();
         }

         // Return the result of the calculation, plus call the hook method
         // (derived class responsibility)
         workstack.push (calculate(left, right, t) + calculateHook(left, right, t));
      }
      else
      {
         workstack.push (atol(t.c_str()));
      }
   }

   // Answer is now on stack
   return (workstack.top());
}

/// <summary>
/// One method to handle it all. Very 'modern c++'.
/// </summary>
/// <param name="expression">A mathematical expression to resolve, in infix notation.</param>
/// <returns>The answer, or 0 on error</returns>
long int gamzia::Resolver::resolve(std::string expression, bool repeat)
{
   if (!repeat)
   {
      // Clear our queue
      while (!q.empty())
         q.pop();

      // Clear the stack
      while (!s.empty())
         s.pop();

      infixToRPN (expression);
      return (evaluateRPN());
   }
   else
      // Repeat = true
      // This allows repeat dice rolls / calculations, without rebuilding
      // the RPN queue each time.
      return (evaluateRPN());
}


