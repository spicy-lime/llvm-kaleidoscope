#include <iostream>

#include "parser.hpp"
#include "lexer.hpp"




//int getNextToken(std::string& str, std::istream& stream)
//{
//   return CurTok = gettok(str, stream);
//}



//std::unique_ptr<PrototypeAST> logErrorP(char const* str)
//{
//   logError(str);
//   return nullptr;
//}


std::unique_ptr<ExprAST> parsePrimary(Token& tok, std::istream& stream)
{
   switch(tok.type)
   {
      case TokenType::id:
      {
         return parseIdentifierExpr(tok, stream);
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
         std::cerr << "Unexpected token encountered";
         return nullptr;
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
      std::cerr << "Expected ')'";
      return nullptr;
   }
   gettok(tok, stream);
   return V;
}

std::unique_ptr<ExprAST> parseIdentifierExpr(Token& tok, std::istream& stream)
{

   std::string idName = tok.id;
   gettok(tok, stream);

   // variable name
   if(tok.ch != '(')
   {
      return std::make_unique<VariableExprAST>(idName);
   }

   // else it's a function call
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
            std::cerr << "Expected ')' or ',' in argument list";
            return nullptr;
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
         rhs = parseBinOpRhs(tok, stream, tokPrec + 1, std::move(rhs));
         if(!rhs)
         {
            return nullptr;
         }

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
   // precondition - tok is function id
   if(tok.type != TokenType::id)
   {
      std::cerr << "Expected function name in prototype";
      return nullptr;
   }
   // memoize function name
   std::string fnName = tok.id;
   gettok(tok, stream);
   // should be start of args now
   if(tok.ch != '(')
   {
      std::cerr << "Expected '(' in prototype";
      return nullptr;
   }
   // memoize args
   std::vector<std::string> argNames;
   while((gettok(tok, stream), tok.type) == TokenType::id)
   {
      argNames.push_back(tok.id);
   }
   // end args
   if(tok.ch != ')')
   {
      std::cerr << "Expected ')' in prototype";
      return nullptr;
   }
   return std::make_unique<PrototypeAST>(fnName, std::move(argNames));
}

std::unique_ptr<FunctionAST> parseDefinition(Token& tok, std::istream& stream)
{
   // precondition - tok is "def"
   if(tok.type != TokenType::def)
   {
      std::cerr << "Expected 'def' keyword";
      return nullptr;
   }

   // advance to next token (should be function prototype starting with id)
   gettok(tok, stream);
   auto proto = parsePrototype(tok, stream);
   if(!proto)
   {
      return nullptr;
   }
   // advance to next token (should be function definition expression)
   gettok(tok, stream);
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

class parser_test : public testing::Test
{
   public:
      std::stringstream io;
      Token tok{};
};

TEST_F(parser_test, def1)
{
   io << "def foo(x y) x+foo(y, 4.0);"; // function def
   gettok(tok, io);
   ASSERT_EQ(tok.type, TokenType::def);
   auto p = parseDefinition(tok, io);
   EXPECT_TRUE(p != nullptr);


   // manually build AST, starting with "y, 4.0"
   auto y = std::make_unique<VariableExprAST>("y");
   auto num4 = std::make_unique<NumberExprAST>(4.0);

   VariableExprAST* uutFooArg0 = dynamic_cast<VariableExprAST*>(uutFooCall->args[0].get());
   NumberExprAST* uutFooArg1 = dynamic_cast<NumberExprAST*>(uutFooCall->args[1].get());

   // test y, 4.0
   auto test = *(uutFooArg0) == *y;
   test = *(uutFooArg1) == *num4;
   EXPECT_EQ(*uutFooArg0, *y);
   EXPECT_EQ(*uutFooArg1, *num4);


   // test foo(y, 4.0)
   std::vector<std::unique_ptr<ExprAST>> fooArgs{};
   fooArgs.push_back(std::move(y));
   fooArgs.push_back(std::move(num4));
   CallExprAST* uutFooCall = dynamic_cast<CallExprAST*>(dynamic_cast<BinaryExprAST*>(p->body.get())->rhs.get());
   auto fooCall = std::make_unique<CallExprAST>("foo", std::move(fooArgs));
   EXPECT_EQ(*(uutFooCall), *(fooCall.get()));

   // test x+foo(y, 4.0)
   auto x = std::make_unique<VariableExprAST>("x");
   auto binExp = std::make_unique<BinaryExprAST>('+', std::move(x), std::move(fooCall));
   BinaryExprAST* uutBinExp = dynamic_cast<BinaryExprAST*>(p->body.get());
   EXPECT_EQ(*binExp, *uutBinExp);

   // test def foo(x y)
   std::vector<std::string> defFooArgs{};
   defFooArgs.push_back("x");
   defFooArgs.push_back("y");
   auto proto = std::make_unique<PrototypeAST>("foo", std::move(defFooArgs));
   PrototypeAST* uutProto = p->proto.get();
   EXPECT_EQ(*proto, *uutProto);

   // test def foo(x y) x+foo(y, 4.0);
   FunctionAST func(std::move(proto), std::move(binExp));

   EXPECT_EQ(func, *p);
}

TEST_F(parser_test, def2)
{
   io << "def foo(x y) x+y" // function def
      << " y;"; // primary expression
   gettok(tok, io);
   ASSERT_EQ(tok.type, TokenType::def);
   auto p = parseDefinition(tok, io);
   EXPECT_TRUE(p != nullptr);
   auto p1 = parseTopLevelExpr(tok, io);
   EXPECT_TRUE(p1 != nullptr);

}

TEST_F(parser_test, invalid1)
{
   io << "def foo(x y) x+y" // function def
      << " );"; // bad token expression
   gettok(tok, io);
   ASSERT_EQ(tok.type, TokenType::def);
   auto p = parseDefinition(tok, io);
   EXPECT_TRUE(p != nullptr);
   auto p1 = parseTopLevelExpr(tok, io);
   EXPECT_TRUE(p1 == nullptr);
}

TEST_F(parser_test, extern1)
{
   io << "extern sin(a)"; // function def
   gettok(tok, io);
   ASSERT_EQ(tok.type, TokenType::ext);
   auto p = parseExtern(tok, io);
   EXPECT_TRUE(p != nullptr);
}

TEST_F(parser_test, program)
{
   io << \
      "# Compute the x'th fibonacci number. \n \
      def fib(x) \n \
         if x < 3 then \n \
            1 \n \
         else \n \
            fib(x-1)+fib(x-2) \
      # This expression will compute the 40th number. \n \
      fib(40)";

   gettok(tok, io);
   ASSERT_EQ(tok.type, TokenType::def);
   ASSERT_EQ(tok.id, "def");

   auto def = parseDefinition(tok, io);
   EXPECT_TRUE(def != nullptr);



   auto p1 = parseTopLevelExpr(tok, io);
   EXPECT_TRUE(p1 != nullptr);

}

#endif
