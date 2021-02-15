#ifndef __LEXER_H_
#define __LEXER_H_

#include <string>

enum Token
{
   tok_eof = -1,

   //commands
   tok_def = -2,
   tok_extern = -3,

   //primary
   tok_identifier = -4,
   tok_number = -5
};

static std::string IdentifierStr;
static double NumVal;



#endif // __LEXER_H_
