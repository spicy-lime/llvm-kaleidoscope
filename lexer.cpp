#include "lexer.hpp"

#include <cstdlib>

static int gettok()
{
   static int LastChar = ' ';

   while(std::isspace(LastChar))
   {
      LastChar = std::getchar();
   }

   if(isalpha(LastChar))
   {
      IdentifierStr = LastChar;
      while(isalnum((LastChar = std::getchar())))
      {
         IdentifierStr += LastChar;
      }
      if(IdentifierStr == "def")
      {
         return tok_def;
      }
      if(IdentifierStr == "extern")
      {
         return tok_extern;
      }
      return tok_identifier;
   }

   if(std::isdigit(LastChar) || LastChar == '.')
   {
      std::string NumStr;
      do
      {
         NumStr += LastChar;
         LastChar = std::getchar();
      } while(std::isdigit(LastChar) || LastChar == '.');
      NumVal = std::strtod(NumStr.c_str(), 0);
      return tok_number;
   }

   if(LastChar == '#')
   {
      do
      {
         LastChar = std::getchar();
      } while(LastChar != EOF && LastChar != '\n' && LastChar != '\r');

      if(LastChar != EOF)
      {
         return gettok();
      }
   }

   if(LastChar == EOF)
   {
      return tok_eof;
   }

   int ThisChar = LastChar;

   LastChar = std::getchar();
   return ThisChar;

}
