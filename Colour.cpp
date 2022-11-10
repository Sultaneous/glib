#include "Colour.h"
#include <string>
#include <list>
#include <sstream>
#include <iomanip>

/// <summary>
/// statice method which produces a colour grid.
/// Useful for selecting colour/background pairs which
/// are effective.
/// </summary>
/// <returns>A string containing the colour grid.</returns>
std::string gamzia::Colour::showColourGrid()
{
   std::stringstream ss;

   ss.clear();

   // Build our grid
   ss << sre << cwh << "Colour Grid:" << sref << std::endl;
   ss << "    ";

   for (auto &x : bnames)
      ss << std::setw(5) << x;
   ss << std::endl;
   
   int j=0;
   for (auto& c : listcall)
   {
      if (!(c == coff))
      {
         ss << cwh << std::setw(5) << cnames[j];
         j++;
      }
      for (auto& b : listball)
      {
         if (!(c == coff) && !(b == boff))
            ss << c << b << "  *  " << coff;
      }

      ss << std::endl;
   }

   ss << std::endl;

   return (ss.str());
}

/// <summary>
/// Static method which shows a list of macros (name and effect).
/// </summary>
/// <returns>A string containing the macro list.</returns>
std::string gamzia::Colour::showMacros()
{
   std::stringstream ss;

   ss.clear();
   ss << sre << "Macro Formats" << sref << std::endl;
   for (auto &m : listmall)
      if (!(m == off))
         ss << m << "Sultaneous Sends Salutations" << off << std::endl;
   
   return (ss.str());
}

/// <summary>
/// A static method which shows a list of available styles.
/// </summary>
/// <returns>A string containing the styles list.</returns>
std::string gamzia::Colour::showStyles()
{
   std::stringstream ss;

   ss.clear();
   ss << cwh << sre << "Style Formats" << sref << std::endl;
   ss << "(Note: not all styles work on all terminals)" << std::endl;
   ss << cwh << sbo << "This is BOLD" << sbof << std::endl;
   ss << cwh << sdi << "This is DIM" << sdif << std::endl;
   ss << cwh << sun << "This is UNDERLINE" << sunf << std::endl;
   ss << cwh << sbl << "This is BLINK" << sblf << std::endl;
   ss << cwh << sre << "This is REVERSE" << sref << std::endl;
   ss << cwh << shi << "This is HIDDEN" << shif << std::endl;

   return (ss.str());
}

/// <summary>
/// Strips out all colour (foreground, background, style) format codes
/// from a string.
/// </summary>
/// <param name="label">The string to scan for colour codes.</param>
/// <returns>A modified string without codes. Affects original string.</returns>
std::string gamzia::Colour::colourStrip(std::string label)
{
   int x;

   // Strip foreground colours
   for (auto& c : listcall)
   {
      while ((x = (int)label.find(c, 0)) != std::string::npos)
      {
         label.erase(x, c.length());
      }
   }

   // Strip background colours
   for (auto& b : listball)
   {
      while ((x = (int)label.find(b, 0)) != std::string::npos)
      {
         label.erase(x, b.length());
      }
   }

   // Strip styles
   for (auto& st : listsall)
   {
      while ((x = (int)label.find(st, 0)) != std::string::npos)
      {
         label.erase(x, st.length());
      }
   }

   return (label);
}
