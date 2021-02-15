#include <string>
#include <memory>
#include <vector>

class ExprAST
{
   public:
      virtual ~ExprAST() {};
};

class NumberExprAST : public ExprAST
{
   public:
      NumberExprAST(double v)
         : Val{v}
      {}

   private:
      double Val;
};

class VariableExprAST : public ExprAST
{
   public:
      VariableExprAST(std::string const& name)
         : Name{name}
      {}

   private:
      std::string Name;
};

class BinaryExprAST : public ExprAST
{
   public:
      BinaryExprAST(char op, std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs)
         : Op{op}
         , LHS{std::move(lhs)}
         , RHS{std::move(rhs)}
      {}

   private:
      char Op;
      std::unique_ptr<ExprAST> LHS;
      std::unique_ptr<ExprAST> RHS;
};

class CallExprAST : public ExprAST
{
   public:
      CallExprAST(std::string const& callee, std::vector<std::unqiue_ptr<ExprAST>> args)
         : Callee{callee}
         , Args{std::move(args)}
      {}
   private:
      std::string Callee;
      std::vector<std::unique_ptr<ExprAST>> Args;

};

class PrototypeAST
{
   public:
      PrototypeAST(std::string const& name, std::vector<std::string> args)
         : Name{name}
         , Args{std::move(args)}
      {}
   private:
      std::string Name;
      std::vector<std::string> Args;
};

class FunctionAST
{
   public:
      FunctionAST(std::unique_ptr<PrototypeAST> proto, std::unique_ptr<ExprAST> body)
         : Proto(std::move(proto))
         , Body(std::move(body))
      {}

   private:
      std::unqiue_ptr<PrototypeAST> Proto;
      std::unique_ptr<ExprAST> Body;
};
