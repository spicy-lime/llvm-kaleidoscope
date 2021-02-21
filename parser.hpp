#include "ast.hpp"
#include <memory>
#include <map>

static int CurTok;
static std::map<char, int> BinopPrecedence
{
   {'<', 10},
   {'+', 20},
   {'-', 20},
   {'*', 40},
};

int getNextToken(std::string& str);
int getTokPrecedence();
std::unique_ptr<PrototypeAST> logErrorP(char const* str);
std::unique_ptr<PrototypeAST> parsePrototype();
std::unique_ptr<FunctionAST> parseDefinition();
std::unique_ptr<PrototypeAST> parseExtern();
std::unique_ptr<FunctionAST> parseTopLevelExpr();


std::unique_ptr<ExprAST> logError(char const* str);
std::unique_ptr<ExprAST> parseNumberExpr();
std::unique_ptr<ExprAST> parseParenExpr();
std::unique_ptr<ExprAST> parseIndentifierExpr();
std::unique_ptr<ExprAST> parsePrimary();
std::unique_ptr<ExprAST> parseExpression();
std::unique_ptr<ExprAST> parseBinOpRhs(int expPrec, std::unique_ptr<ExprAST> lhs);
