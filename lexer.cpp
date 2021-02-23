#include "lexer.hpp"

#include <cstdlib>


void gettok(Token& tok, std::istream& stream)
{
   char LastChar = ' ';
   tok.type = TokenType::eof;
   tok.id.clear();

   if(stream)
   {

      stream >> std::noskipws, LastChar = stream.peek();
      if(stream.good())
      {
         if(std::isspace(LastChar))
         {

            do
            {
               stream >> LastChar;
            }
            while((LastChar = stream.peek()) && (stream.good()) && std::isspace(LastChar));
            if(!stream.good())
            {
               return;
            }
         }
         if(isalpha(LastChar))
         {

            tok.type = TokenType::id;
            do
            {
               stream >> LastChar, tok.id += LastChar;
            } while(isalnum(LastChar = stream.peek()) && (stream.good()));

            if(tok.id == "def")
            {
               tok.type = TokenType::def;
            }
            else if(tok.id == "extern")
            {
               tok.type = TokenType::ext;
            }
            return;
         }
         else if((std::isdigit(LastChar) || LastChar == '.'))
         {

            tok.type = TokenType::num;
            std::string NumStr{};
            do
            {
               NumStr += LastChar;
               stream >> LastChar;
            } while((LastChar = stream.peek()) && (stream.good()) && (std::isdigit(LastChar) || LastChar == '.'));
            tok.num = strtod(NumStr.c_str(), nullptr);
            return;
         }
         else if(LastChar == '#')
         {

            while((stream >> LastChar) && (stream) && LastChar != '\n' && LastChar != '\r');
            if(stream)
            {
               return gettok(tok, stream);
            }
         }
         else
         {
            stream >> LastChar;
            tok.ch = LastChar;
            tok.type = TokenType::sym;
         }

      }
   }
   return;
}

#ifdef BUILD_TESTS
#include "gtest/gtest.h"
#include <sstream>

class lexer_test : public ::testing::Test
{
   public:
      std::stringstream io;
      Token tok{};
};

TEST_F(lexer_test, id_empty)
{
   io << "";
   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::eof);
   EXPECT_EQ(tok.id, "");
}

TEST_F(lexer_test, id_spaces)
{
   io << "   \t\r\n";
   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::eof);
   EXPECT_EQ(tok.id, "");
}

TEST_F(lexer_test, id_single)
{
   std::stringstream io;
   io << "A";
   Token tok{};
   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::id);
   EXPECT_EQ(tok.id, "A");
}

TEST_F(lexer_test, id_multiple)
{
   std::stringstream io;
   io << "ABC";
   Token tok{};
   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::id);
   EXPECT_EQ(tok.id, "ABC");
}

TEST_F(lexer_test, id_lead_space)
{
   std::stringstream io;
   io << " ABC";
   Token tok{};
   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::id);
   EXPECT_EQ(tok.id, "ABC");
}

TEST_F(lexer_test, id_trail_space)
{
   std::stringstream io;
   io << "ABC ";
   Token tok{};
   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::id);
   EXPECT_EQ(tok.id, "ABC");
}

TEST_F(lexer_test, id_definition)
{
   std::stringstream io;
   io << "def ";
   Token tok{};
   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::def);
   EXPECT_EQ(tok.id, "def");
}

TEST_F(lexer_test, id_extern)
{
   std::stringstream io;
   io << "extern ";
   Token tok{};
   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::ext);
   EXPECT_EQ(tok.id, "extern");
}

TEST_F(lexer_test, digit)
{
   std::stringstream io;
   io << "1";
   Token tok{};
   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::num);
   EXPECT_EQ(tok.num, double(1));
}

TEST_F(lexer_test, digits)
{
   std::stringstream io;
   io << "1234567";
   Token tok{};
   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::num);
   EXPECT_EQ(tok.num, double(1234567));
}

TEST_F(lexer_test, decimal)
{
   std::stringstream io;
   io << "1234.567";
   Token tok{};
   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::num);
   EXPECT_EQ(tok.num, double(1234.567));
}

TEST_F(lexer_test, comment)
{
   std::stringstream io;
   io << "# this is a comment \n foo";
   Token tok{};
   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::id);
   EXPECT_EQ(tok.id, "foo");
}

TEST_F(lexer_test, sym_paren)
{
   std::stringstream io;
   io << "(";
   Token tok{};
   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::sym);
   EXPECT_EQ(tok.ch, '(');
}

TEST_F(lexer_test, chain0)
{
   std::stringstream io;
   io << "def foo";
   Token tok{};
   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::def);
   EXPECT_EQ(tok.id, "def");

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::id);
   EXPECT_EQ(tok.id, "foo");
}

TEST_F(lexer_test, chain1)
{
   std::stringstream io;
   io << "4)";
   Token tok{};
   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::num);
   EXPECT_EQ(tok.num, double(4));

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::sym);
   EXPECT_EQ(tok.ch, ')');
}

TEST_F(lexer_test, chain2)
{
   std::stringstream io;
   io << "(4";
   Token tok{};
   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::sym);
   EXPECT_EQ(tok.ch, '(');

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::num);
   EXPECT_EQ(tok.num, double(4));
}

TEST_F(lexer_test, chain3)
{
   std::stringstream io;
   io << "def fib(x";
   Token tok{};
   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::def);
   EXPECT_EQ(tok.id, "def");

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::id);
   EXPECT_EQ(tok.id, "fib");

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::sym);
   EXPECT_EQ(tok.ch, '(');

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::id);
   EXPECT_EQ(tok.id, "x");
}

TEST_F(lexer_test, chain)
{
   std::stringstream io;
   io << "def foo(4)";
   Token tok{};
   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::def);
   EXPECT_EQ(tok.id, "def");

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::id);
   EXPECT_EQ(tok.id, "foo");

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::sym);
   EXPECT_EQ(tok.ch, '(');

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::num);
   EXPECT_EQ(tok.num, double(4));

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::sym);
   EXPECT_EQ(tok.ch, ')');
}

TEST_F(lexer_test, program)
{
   std::stringstream io;

   io << \
      "# Compute the x'th fibonacci number. \n \
      def fib(x) \n \
         if x < 3 then \n \
            1 \n \
         else \n \
            fib(x-1)+fib(x-2) \
      # This expression will compute the 40th number. \n \
      fib(40)";
   Token tok{};
   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::def);
   EXPECT_EQ(tok.id, "def");

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::id);
   EXPECT_EQ(tok.id, "fib");

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::sym);
   EXPECT_EQ(tok.ch, '(');

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::id);
   EXPECT_EQ(tok.id, "x");

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::sym);
   EXPECT_EQ(tok.ch, ')');

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::id);
   EXPECT_EQ(tok.id, "if");

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::id);
   EXPECT_EQ(tok.id, "x");

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::sym);
   EXPECT_EQ(tok.ch, '<');

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::num);
   EXPECT_EQ(tok.num, double(3));

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::id);
   EXPECT_EQ(tok.id, "then");

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::num);
   EXPECT_EQ(tok.num, double(1));

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::id);
   EXPECT_EQ(tok.id, "else");

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::id);
   EXPECT_EQ(tok.id, "fib");

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::sym);
   EXPECT_EQ(tok.ch, '(');

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::id);
   EXPECT_EQ(tok.id, "x");

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::sym);
   EXPECT_EQ(tok.ch, '-');

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::num);
   EXPECT_EQ(tok.num, double(1));

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::sym);
   EXPECT_EQ(tok.ch, ')');

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::sym);
   EXPECT_EQ(tok.ch, '+');

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::id);
   EXPECT_EQ(tok.id, "fib");

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::sym);
   EXPECT_EQ(tok.ch, '(');

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::id);
   EXPECT_EQ(tok.id, "x");

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::sym);
   EXPECT_EQ(tok.ch, '-');

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::num);
   EXPECT_EQ(tok.num, double(2));

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::sym);
   EXPECT_EQ(tok.ch, ')');

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::id);
   EXPECT_EQ(tok.id, "fib");

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::sym);
   EXPECT_EQ(tok.ch, '(');

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::num);
   EXPECT_EQ(tok.num, double(40));

   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::sym);
   EXPECT_EQ(tok.ch, ')');
}

#endif
