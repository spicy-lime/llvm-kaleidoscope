#include "parser.hpp"
#include "lexer.hpp"




//int getNextToken(std::string& str, std::istream& stream)
//{
//   return CurTok = gettok(str, stream);
//}


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


std::unique_ptr<ExprAST> parsePrimary(Token& tok, std::istream& stream)
{
   switch(tok.type)
   {
      case TokenType::id:
      {
         return parseIndentifierExpr(tok, stream);
      }
      case TokenType::num:
      {
         return parseNumberExpr(tok, stream);
      }
      case TokenType::sym:
      {
         if(tok.ch == '(')
         {
            return parseParenExpr(tok, stream);
         }
      }
      default:
      {
         return logError("Unknown token encountered");
      }
   }
}

std::unique_ptr<ExprAST> parseNumberExpr(Token& tok, std::istream& stream)
{
   auto res = std::make_unique<NumberExprAST>(tok.num);
   gettok(tok, stream);
   return std::move(res);
}

std::unique_ptr<ExprAST> parseParenExpr(Token& tok, std::istream& stream)
{
   gettok(tok, stream);
   auto V = parseExpression(tok, stream);
   if(!V)
   {
      return nullptr;
   }
   if(tok.ch != ')')
   {
      return logError("Expected ')'");
   }
   gettok(tok, stream);
   return V;
}

std::unique_ptr<ExprAST> parseIndentifierExpr(Token& tok, std::istream& stream)
{
   std::string idName = tok.id;
   gettok(tok, stream);

   if(tok.ch != '(')
   {
      return std::make_unique<VariableExprAST>(idName);
   }

   gettok(tok, stream);

   std::vector<std::unique_ptr<ExprAST>> args;
   if(tok.ch != ')')
   {
      while(1)
      {
         if(auto arg = parseExpression(tok, stream))
         {
            args.push_back(std::move(arg));
         }
         else
         {
            return nullptr;
         }
         if(tok.ch == ')')
         {
            break;
         }
         if(tok.ch != ',')
         {
            return logError("Expected ')' or ',' in argument list");
         }
         gettok(tok, stream);
      }
   }
   gettok(tok, stream);
   return std::make_unique<CallExprAST>(idName, std::move(args));
}

std::unique_ptr<ExprAST> parseExpression(Token& tok, std::istream& stream)
{
   auto lhs = parsePrimary(tok, stream);
   if(!lhs)
   {
      return nullptr;
   }
   return parseBinOpRhs(tok, stream, 0, std::move(lhs));
}

std::unique_ptr<ExprAST> parseBinOpRhs(Token& tok, std::istream& stream, int exprPrec, std::unique_ptr<ExprAST> lhs)
{
   while(1)
   {
      int tokPrec = getTokPrecedence(tok);
      if(tokPrec < exprPrec)
      {
         return lhs;
      }
      int binOp = tok.ch;
      gettok(tok, stream);
      auto rhs = parsePrimary(tok, stream);
      if(!rhs)
      {
         return nullptr;
      }
      int nextPrec = getTokPrecedence(tok);
      if(tokPrec < nextPrec)
      {

      }
      lhs = std::make_unique<BinaryExprAST>(binOp, std::move(lhs), std::move(rhs));
   }
}

int getTokPrecedence(Token& tok)
{
   if(!isascii(tok.ch))
   {
      return -1;
   }
   int tokPrec = BinopPrecedence[tok.ch];
   if(tokPrec <= 0) return -1;
   return tokPrec;
}

std::unique_ptr<PrototypeAST> parsePrototype(Token& tok, std::istream& stream)
{
   if(tok.type != TokenType::id)
   {
      return logErrorP("Expected function name in protoype");
   }
   std::string fnName = tok.id;
   gettok(tok, stream);
   if(tok.ch != '(')
   {
      return logErrorP("Expected '(' in prototype");
   }
   std::vector<std::string> argNames;
   while((gettok(tok, stream), tok.type) == TokenType::id)
   {
      argNames.push_back(tok.id);
   }
   if(tok.ch != ')')
   {
      return logErrorP("Exprect ')' in prototype");
   }
   gettok(tok, stream);
   return std::make_unique<PrototypeAST>(fnName, std::move(argNames));
}

std::unique_ptr<FunctionAST> parseDefinition(Token& tok, std::istream& stream)
{
   gettok(tok, stream);
   auto proto = parsePrototype(tok, stream);
   if(!proto)
   {
      return nullptr;
   }
   if(auto expr = parseExpression(tok, stream))
   {
      return std::make_unique<FunctionAST>(std::move(proto), std::move(expr));
   }
   return nullptr;
}

std::unique_ptr<PrototypeAST> parseExtern(Token& tok, std::istream& stream)
{
   gettok(tok, stream);
   return parsePrototype(tok, stream);
}

std::unique_ptr<FunctionAST> parseTopLevelExpr(Token& tok, std::istream& stream)
{
   if(auto expr = parseExpression(tok, stream))
   {
      auto proto = std::make_unique<PrototypeAST>("", std::vector<std::string>());
      return std::make_unique<FunctionAST>(std::move(proto), std::move(expr));
   }
   return nullptr;
}

#ifdef BUILD_TESTS
#include <gtest/gtest.h>

#endif
