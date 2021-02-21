#ifndef __LEXER_H_
#define __LEXER_H_

#include <string>
#include <istream>

enum class TokenType
{
   eof = -1,

   //commands
   def = -2,
   ext = -3,

   //primary
   id = -4,
   num = -5,
   sym = -6
};

struct Token
{
   TokenType type{TokenType::eof};
   std::string id{}; // identifiers
   double num{}; // number types (doubles)
   char ch; // single character
};


void gettok(Token& tok, std::istream& stream);

#endif // __LEXER_H_
