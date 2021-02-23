#include "ast.hpp"
#include "lexer.hpp"
#include <memory>
#include <map>

static std::map<char, int> BinopPrecedence
{
   {'<', 10},
   {'+', 20},
   {'-', 20},
   {'*', 40},
};

int getNextToken(std::string& str);
int getTokPrecedence(Token& tok);
std::unique_ptr<PrototypeAST> logErrorP(char const* str);
std::unique_ptr<PrototypeAST> parsePrototype(Token& tok, std::istream& stream);
std::unique_ptr<FunctionAST> parseDefinition(Token& tok, std::istream& stream);
std::unique_ptr<PrototypeAST> parseExtern(Token& tok, std::istream& stream);
std::unique_ptr<FunctionAST> parseTopLevelExpr(Token& tok, std::istream& stream);


std::unique_ptr<ExprAST> logError(char const* str);
std::unique_ptr<ExprAST> parseNumberExpr(Token& tok, std::istream& stream);
std::unique_ptr<ExprAST> parseParenExpr(Token& tok, std::istream& stream);
std::unique_ptr<ExprAST> parseIndentifierExpr(Token& tok, std::istream& stream);
std::unique_ptr<ExprAST> parsePrimary(Token& tok, std::istream& stream);
std::unique_ptr<ExprAST> parseExpression(Token& tok, std::istream& stream);
std::unique_ptr<ExprAST> parseBinOpRhs(Token& tok, std::istream& stream,
                                       int expPrec, std::unique_ptr<ExprAST> lhs);
