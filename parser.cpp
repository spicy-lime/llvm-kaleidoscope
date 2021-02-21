#include "parser.hpp"
#include "lexer.hpp"



int getNextToken(std::string& str)
{
   return CurTok = gettok(str);
}

std::unique_ptr<ExprAST> logError(char const* str)
{
   fprintf(stderr, "LogError: %s\n", str);
   return nullptr;
}

std::unique_ptr<PrototypeAST> logErrorP(char const* str)
{
   logError(str);
   return nullptr;
}


std::unique_ptr<ExprAST> parsePrimary()
{
   switch(CurTok)
   {
      case tok_identifier:
      {
         return parseIndentifierExpr();
      }
      case tok_number:
      {
         return parseNumberExpr();
      }
      case '(':
      {
         return parseParenExpr();
      }
      default:
      {
         return logError("Unknown token encountered");
      }
   }
}

std::unique_ptr<ExprAST> parseNumberExpr()
{
   auto res = std::make_unique<NumberExprAST>(NumVal);
   getNextToken();
   return std::move(res);
}

std::unique_ptr<ExprAST> parseParenExpr()
{
   getNextToken();
   auto V = parseExpression();
   if(!V)
   {
      return nullptr;
   }
   if(CurTok != ')')
   {
      return logError("Expected ')'");
   }
   getNextToken();
   return V;
}

std::unique_ptr<ExprAST> parseIndentifierExpr()
{
   std::string idName = identifierStr;
   getNextToken();

   if(CurTok != '(')
   {
      return std::make_unique<VariableExprAST>(idName);
   }

   getNextToken();

   std::vector<std::unique_ptr<ExprAST>> args;
   if(CurTok != ')')
   {
      while(1)
      {
         if(auto arg = parseExpression())
         {
            args.push_back(std::move(arg));
         }
         else
         {
            return nullptr;
         }
         if(CurTok == ')')
         {
            break;
         }
         if(CurTok != ',')
         {
            return logError("Expected ')' or ',' in argument list");
         }
         getNextToken();
      }
   }
   getNextToken();
   return std::make_unique<CallExprAST>(idName, std::move(args));
}

std::unique_ptr<ExprAST> parseExpression()
{
   auto lhs = parsePrimary();
   if(!lhs)
   {
      return nullptr;
   }
   return parseBinOpRhs(0, std::move(lhs));
}

std::unique_ptr<ExprAST> parseBinOpRhs(int exprPrec, std::unique_ptr<ExprAST> lhs)
{
   while(1)
   {
      int tokPrec = getTokPrecedence();
      if(tokPrec < exprPrec)
      {
         return lhs;
      }
      int binOp = CurTok;
      getNextToken();
      auto rhs = parsePrimary();
      if(!rhs)
      {
         return nullptr;
      }
      int nextPrec = getTokPrecedence();
      if(tokPrec < nextPrec)
      {

      }
      lhs = std::make_unique<BinaryExprAST>(binOp, std::move(lhs), std::move(rhs));
   }
}

int getTokPrecedence()
{
   if(!isascii(CurTok))
   {
      return -1;
   }
   int tokPrec = BinopPrecedence[CurTok];
   if(tokPrec <= 0) return -1;
   return tokPrec;
}

std::unique_ptr<PrototypeAST> parsePrototype(int& curTok, std::string& proto)
{
   if(curTok != tok_identifier)
   {
      return logErrorP("Expected function name in protoype");
   }
   std::string fnName = proto;
   curTok = getNextToken(proto);
   if(curTok != '(')
   {
      return logErrorP("Expected '(' in prototype");
   }
   std::vector<std::string> argNames;
   while((curTok = getNextToken(proto)) == tok_identifier)
   {
      argNames.push_back(proto);
   }
   if(curTok != ')')
   {
      return logErrorP("Exprect ')' in prototype");
   }
   getNextToken();
   return std::make_unique<PrototypeAST>(fnName, std::move(argNames));
}

std::unique_ptr<FunctionAST> parseDefinition(int& curTok, std::string& def)
{
   curTok = getNextToken(def);
   auto proto = parsePrototype();
   if(!proto)
   {
      return nullptr;
   }
   if(auto expr = parseExpression())
   {
      return std::make_unique<FunctionAST>(std::move(proto), std::move(expr));
   }
   return nullptr;
}

std::unique_ptr<PrototypeAST> parseExtern()
{
   getNextToken();
   return parsePrototype();
}

std::unique_ptr<FunctionAST> parseTopLevelExpr()
{
   if(auto expr = parseExpression())
   {
      auto proto = std::make_unique<PrototypeAST>("", std::vector<std::string>());
      return std::make_unique<FunctionAST>(std::move(proto), std::move(expr));
   }
   return nullptr;
}
