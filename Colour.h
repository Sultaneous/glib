//pragma once
#include <string>
#include <list>
#include <vector>

namespace gamzia
{
   class Colour
   {
   public:
      // Foreground(Text) Colours
      inline static const std::string cbl = "\033[30m";  // Colour Black
      inline static const std::string cdr = "\033[31m";  // Colour Dark Red
      inline static const std::string cdg = "\033[32m";  // Colour Dark Green
      inline static const std::string cdy = "\033[33m";  // Colour Dark Yellow
      inline static const std::string cdb = "\033[34m";  // Colour Dark Blue
      inline static const std::string cdm = "\033[35m";  // Colour Dark Magenta
      inline static const std::string cdc = "\033[36m";  // Colour Dark Cyan
      inline static const std::string cdgy = "\033[90m"; // Colour Dark Gray

      inline static const std::string clgy = "\033[37m"; // Colour Light Gray
      inline static const std::string clr = "\033[91m";  // Colour Light Red
      inline static const std::string clg = "\033[92m";  // Colour Light Green
      inline static const std::string cly = "\033[93m";  // Colour Light Yellow
      inline static const std::string clb = "\033[94m";  // Colour Light Blue
      inline static const std::string clm = "\033[95m";  // Colour Light Magenta
      inline static const std::string clc = "\033[96m";  // Colour Light Cyan
      inline static const std::string cwh = "\033[97m";  // Colour White

      // Turns off all formatting
      inline static const std::string coff = "\033[0m"; // Colour Off

      // Background Colours
      inline static const std::string bbl = "\033[40m";   // Background Colour Black
      inline static const std::string bdr = "\033[41m";   // Background Colour Dark Red
      inline static const std::string bdg = "\033[42m";   // Background Colour Dark Green
      inline static const std::string bdy = "\033[43m";   // Background Colour Dark Yellow
      inline static const std::string bdb = "\033[44m";   // Background Colour Dark Blue
      inline static const std::string bdm = "\033[45m";   // Background Colour Dark Magenta
      inline static const std::string bdc = "\033[46m";   // Background Colour Dark Cyan
      inline static const std::string bdgy = "\033[100m"; // Background Colour Dark Gray

      inline static const std::string blgy = "\033[47m";  // Background Colour Light Gray
      inline static const std::string blr = "\033[101m";  // Background Colour Light Red
      inline static const std::string blg = "\033[102m";  // Background Colour Light Green
      inline static const std::string bly = "\033[103m";  // Background Colour Light Yellow
      inline static const std::string blb = "\033[104m";  // Background Colour Light Blue
      inline static const std::string blm = "\033[105m";  // Background Colour Light Magenta
      inline static const std::string blc = "\033[106m";  // Background Colour Light Cyan
      inline static const std::string bwh = "\033[107m";  // Background Colour White

      // Turns off only the background Colour
      inline static const std::string boff = "\033[49m"; // Background Colour Off

      // Macro strings
      // Master off switch turns off Colourand bacground in one mnemonic
      inline static const std::string off = coff + boff;
      inline static const std::string no = cly + bdr;
      inline static const std::string yes = clg + bdg;
      inline static const std::string old = cly + bdb;
      inline static const std::string retro = cwh + bbl;
      inline static const std::string paper = cbl + bwh;

      // Styles
      inline static const std::string sbo = "\033[1m";  // Style Bold
      inline static const std::string sdi = "\033[2m";  // Style Dim
      inline static const std::string sun = "\033[4m";  // Style Underline
      inline static const std::string sbl = "\033[5m";  // Style Blink
      inline static const std::string sre = "\033[7m";  // Style Reverse
      inline static const std::string shi = "\033[8m";  // Style Hidden

      inline static const std::string sbof = "\033[21m"; // Style Bold Off
      inline static const std::string sdif = "\033[22m"; // Style Dim Off
      inline static const std::string sunf = "\033[24m"; // Style Underline Off
      inline static const std::string sblf = "\033[25m"; // Style Blink Off
      inline static const std::string sref = "\033[27m"; // Style Reverse Off
      inline static const std::string shif = "\033[28m"; // Style Hidden Off

      // Lists of colours and backgrounds
      // Names of all colours
      inline static const std::vector<std::string> cnames = 
      {
         "cbl", "cdr", "cdg", "cdy", "cdb", "cdm", "cdc", "cdgy",  
         "clgy", "clr", "clg", "cly", "clb", "clm", "clc", "cwh"
      };

      // Names of all backgrouds
      inline static const std::vector<std::string> bnames = 
      {
         "bbl", "bdr", "bdg", "bdy", "bdb", "bdm", "bdc", "bdgy",
         "blgy", "blr", "blg", "bly", "blb", "blm", "blc", "bwh"
      };

      // List all colour formats
      inline static const std::vector<std::string> listcall = 
      {
         cbl, cdr, cdg, cdy, cdb, cdm, cdc, cdgy,
         clgy, clr, clg, cly, clb, clm, clc, cwh, coff
      };

      // List of all backround formats
      inline static const std::vector<std::string> listball =
      {
         bbl, bdr, bdg, bdy, bdb, bdm, bdc, bdgy,
         blgy, blr, blg, bly, blb, blm, blc, bwh, boff
      };

      // List of all style formats
      inline static const std::vector<std::string> listsall = 
      {
         sbo, sdi, sun, sbl, sre, shi,
         sbof, sdif, sunf, sblf, sref, shif
      };

      // Hash of all macro formats
      inline static const std::vector<std::string> listmall =
      {
         "'NO': "+no, "'YES': "+yes, "'OLD': "+old, 
         "'RETRO': "+retro, "'PAPER': "+paper, off
      };

      // Static methods
      std::string static colourStrip(std::string label);
      std::string static showColourGrid();
      std::string static showMacros();
      std::string static showStyles();
   }; // Class

} // Namespace
