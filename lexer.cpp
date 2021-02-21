#include "lexer.hpp"

#include <cstdlib>


void gettok(Token& tok, std::istream& stream)
{
   char LastChar = ' ';
   tok.type = TokenType::eof;

   if(stream)
   {

      if((stream >> LastChar) && (stream) && std::isspace(LastChar))
      {

         while((stream >> LastChar) && (stream) && std::isspace(LastChar));
      }
      else if((stream) && isalpha(LastChar))
      {

         tok.type = TokenType::id;
         tok.id = LastChar;
         while((stream >> LastChar) && (stream))
         {
            if(isalnum(LastChar))
            {
               tok.id += LastChar;
            }
            else
            {
               break;
            }
         }

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
      else if((stream) && (std::isdigit(LastChar) || LastChar == '.'))
      {

         tok.type = TokenType::eof;
         std::string NumStr;
         do
         {
            NumStr += LastChar;
         } while( (stream >> LastChar) && (stream) && (std::isdigit(LastChar) || LastChar == '.'));
         if(std::isdigit(LastChar))
         {
            tok.type = TokenType::num;
            tok.num = std::strtod(NumStr.c_str(), 0);
         }
         return;
      }
      else if((stream) && (LastChar == '#'))
      {

         while((stream >> LastChar) && (stream) && LastChar != '\n' && LastChar != '\r');
         if(stream)
         {
            return gettok(tok, stream);
         }
      }
   }
   else
   {
      return;
   }
//   char LastChar = ' ';
//
//   while(std::isspace(LastChar))
//   {
//      if(stream)
//      {
//         stream >> LastChar;
//      }
//      else
//      {
//         break;
//      }
//   }
//
//   if(isalpha(LastChar))
//   {
//      tok.id = LastChar;
//      stream >> LastChar;
//      while(stream)
//      {
//         if(isalnum(LastChar))
//         {
//            stream >> LastChar;
//            tok.id += LastChar;
//         }
//         else
//         {
//            break;
//         }
//      }
//      if(tok.id == "def")
//      {
//         tok.type = TokenType::def;
//         return;
//      }
//      if(tok.id == "extern")
//      {
//         tok.type = TokenType::ext;
//         return;
//      }
//      tok.type = TokenType::id;
//      return;
//   }
//
//   if(std::isdigit(LastChar) || LastChar == '.')
//   {
//      std::string NumStr;
//      do
//      {
//         NumStr += LastChar;
//         stream >> LastChar;
//      } while(std::isdigit(LastChar) || LastChar == '.');
//      tok.num = std::strtod(NumStr.c_str(), 0);
//      return;
//   }
//
//   if(LastChar == '#')
//   {
//      do
//      {
//         if(stream)
//         {
//            stream >> LastChar;
//         }
//         else
//         {
//            break;
//         }
//      } while(LastChar != EOF && LastChar != '\n' && LastChar != '\r');
//
//      if(stream)
//      {
//         return gettok(tok, stream);
//      }
//   }
//
//   if(!stream)
//   {
//      tok.type = TokenType::eof;
//      return;
//   }
//
//   int ThisChar = LastChar;
//   stream >> LastChar;
//
//   tok.type = TokenType::sym;
//   tok.ch = ThisChar;
//   return;
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

TEST_F(lexer_test, id_multiple_lead_space)
{
   std::stringstream io;
   io << " ABC";
   Token tok{};
   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::id);
   EXPECT_EQ(tok.id, "ABC");
}

TEST_F(lexer_test, id_multiple_trail_space)
{
   std::stringstream io;
   io << "ABC ";
   Token tok{};
   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::id);
   EXPECT_EQ(tok.id, "ABC");
}

TEST_F(lexer_test, definition)
{
   std::stringstream io;
   io << "def ";
   Token tok{};
   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::def);
   EXPECT_EQ(tok.id, "def");
}

TEST_F(lexer_test, extern_word)
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

TEST_F(lexer_test, decimal_bad1)
{
   std::stringstream io;
   io << "1234.-";
   Token tok{};
   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::eof);
   EXPECT_EQ(tok.num, double(0));
}

TEST_F(lexer_test, decimal_bad2)
{
   std::stringstream io;
   io << "1234.-";
   Token tok{};
   gettok(tok, io);
   EXPECT_EQ(tok.type, TokenType::eof);
   EXPECT_EQ(tok.num, double(0));
}

#endif
