#include <stdio.h>
#include "lexer.hpp"
#include "parser.hpp"

void handleDefinition(int& curTok, std::string& startDef)
{
   if(parseDefinition(curTok))
   {
      fprintf(stderr, "parsed definition\n");
   }
   else
   {
      getNextToken();
   }
}

void handleExtern()
{
   if(parseExtern())
   {
      fprintf(stderr, "parsed an extern\n");
   }
   else
   {
      getNextToken();
   }
}

void handleTopLevelExpression()
{
   if(parseTopLevelExpr())
   {
      fprintf(stderr, "parsed a top level expr\n");
   }
}

int main()
{
   fprintf(stderr, "ready>");
   std::string tok;
   int curTok = getNextToken(tok);
   while(1)
   {
      fprintf(stderr, "ready>");
      switch(curTok)
      {
         case(tok_eof):
         {
            return 1;
         }
         case(';'):
         {
            curTok = getNextToken(tok);
            break;
         }
         case(tok_def):
         {
            handleDefinition();
            break;
         }
         case(tok_extern):
         {
            handleExtern();
            break;
         }
         default:
         {
            handleTopLevelExpression();
            break;
         }
      }
   }
   return 1;
}
