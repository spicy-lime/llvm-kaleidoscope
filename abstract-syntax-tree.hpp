#include <string>
#include <memory>
#include <vector>
#include <algorithm>

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

static llvm::LLVMContext context{};
static llvm::IRBuilder<> builder(context);
static std::unique_ptr<llvm::Module> mod{};
static std::map<std::string, llvm::Value*> namedValues{};

class NumberExprAST;
class VariableExprAST;
class BinaryExprAST;
class CallExprAST;
class PrototypeAST;
class FunctionAST;

class ExprAST
{
   public:
      virtual ~ExprAST() {};
      virtual llvm::Value* codegen() = 0;
      friend bool operator==(ExprAST const& lhs, ExprAST const& rhs);
};

class NumberExprAST : public ExprAST
{
   public:
      NumberExprAST(double v)
         : val{v}
      {}
      virtual llvm::Value* codegen();

      friend bool operator==(NumberExprAST const& lhs, NumberExprAST const& rhs);
   private:
      double val;

};

class VariableExprAST : public ExprAST
{
   public:
      VariableExprAST(std::string const& n)
         : name{n}
      {}
      virtual llvm::Value* codegen();

      friend bool operator==(VariableExprAST const& lhs, VariableExprAST const& rhs);

      std::string name;

};

class BinaryExprAST : public ExprAST
{
   public:
      BinaryExprAST(char o, std::unique_ptr<ExprAST> l, std::unique_ptr<ExprAST> r)
         : op{o}
         , lhs{std::move(l)}
         , rhs{std::move(r)}
      {}
      virtual llvm::Value* codegen();

      friend bool operator==(BinaryExprAST const& lhs, BinaryExprAST const& rhs);

      char op;
      std::unique_ptr<ExprAST> lhs;
      std::unique_ptr<ExprAST> rhs;
};


class CallExprAST : public ExprAST
{
   public:
      CallExprAST(std::string const& c, std::vector<std::unique_ptr<ExprAST>> a)
         : callee{c}
         , args{std::move(a)}
      {}
   virtual llvm::Value* codegen();

   friend bool operator==(CallExprAST const& lhs, CallExprAST const& rhs);

      std::string callee;
      std::vector<std::unique_ptr<ExprAST>> args;
};

class PrototypeAST
{
   public:
      PrototypeAST(std::string const& n, std::vector<std::string> a)
         : name{n}
         , args{std::move(a)}
      {}
   llvm::Function* codegen();
   std::string getName() {return name;}

   friend bool operator==(PrototypeAST const& lhs, PrototypeAST const& rhs);

      std::string name;
      std::vector<std::string> args;
};

class FunctionAST
{
   public:
      FunctionAST(std::unique_ptr<PrototypeAST> p, std::unique_ptr<ExprAST> b)
         : proto(std::move(p))
         , body(std::move(b))
      {}
   llvm::Value* codegen();

   friend bool operator==(FunctionAST const& lhs, FunctionAST const& rhs);


      std::unique_ptr<PrototypeAST> proto;
      std::unique_ptr<ExprAST> body;
};
inline bool operator!=(ExprAST const& lhs, ExprAST const& rhs){ return !operator==(lhs,rhs); }
inline bool operator==(NumberExprAST const& lhs, NumberExprAST const& rhs){ return lhs.val == rhs.val; }
inline bool operator!=(NumberExprAST const& lhs, NumberExprAST const& rhs){ return !operator==(lhs,rhs); }
inline bool operator==(VariableExprAST const& lhs, VariableExprAST const& rhs){ return lhs.name == rhs.name; }
inline bool operator!=(VariableExprAST const& lhs, VariableExprAST const& rhs){ return !operator==(lhs,rhs); }
inline bool operator==(BinaryExprAST const& lhs, BinaryExprAST const& rhs){ return (lhs.op == rhs.op) && (*(lhs.lhs) == *(rhs.lhs)) && (*(lhs.rhs) == *(rhs.rhs)); }
inline bool operator==(CallExprAST const& lhs, CallExprAST const& rhs)
{
   bool res{lhs.args.size() == rhs.args.size()};
   for(int i{}; res && (i < lhs.args.size()); i++)
   {
      res &= *(lhs.args[i]) == *(rhs.args[i]);
   }
   return (lhs.callee == rhs.callee) && (res);
}
inline bool operator!=(CallExprAST const& lhs, CallExprAST const& rhs){ return !operator==(lhs,rhs); }
inline bool operator==(PrototypeAST const& lhs, PrototypeAST const& rhs) { return (lhs.name == rhs.name) && (lhs.args == rhs.args); }
inline bool operator!=(PrototypeAST const& lhs, PrototypeAST const& rhs){ return !operator==(lhs,rhs); }
inline bool operator==(FunctionAST const& lhs, FunctionAST const& rhs){ return (*(lhs.proto) == *(rhs.proto)) && (*(lhs.body) == *(rhs.body)); }
inline bool operator!=(FunctionAST const& lhs, FunctionAST const& rhs){ return !operator==(lhs,rhs); }

inline bool operator==(ExprAST const& lhs, ExprAST const& rhs)
{
   if(dynamic_cast<NumberExprAST const*>(&lhs) && dynamic_cast<NumberExprAST const*>(&rhs))
   {
      return operator==(*dynamic_cast<NumberExprAST const*>(&lhs), *dynamic_cast<NumberExprAST const*>(&rhs));
   }
   else if(dynamic_cast<VariableExprAST const*>(&lhs) && dynamic_cast<VariableExprAST const*>(&rhs))
   {
      return operator==(*dynamic_cast<VariableExprAST const*>(&lhs), *dynamic_cast<VariableExprAST const*>(&rhs));
   }
   else if(dynamic_cast<BinaryExprAST const*>(&lhs) && dynamic_cast<BinaryExprAST const*>(&rhs))
   {
      return operator==(*dynamic_cast<BinaryExprAST const*>(&lhs), *dynamic_cast<BinaryExprAST const*>(&rhs));
   }
   else if(dynamic_cast<CallExprAST const*>(&lhs) && dynamic_cast<CallExprAST const*>(&rhs))
   {
      return operator==(*dynamic_cast<CallExprAST const*>(&lhs), *dynamic_cast<CallExprAST const*>(&rhs));
   }
   else if(dynamic_cast<PrototypeAST const*>(&lhs) && dynamic_cast<PrototypeAST const*>(&rhs))
   {
      return operator==(*dynamic_cast<PrototypeAST const*>(&lhs), *dynamic_cast<PrototypeAST const*>(&rhs));
   }
   else if(dynamic_cast<FunctionAST const*>(&lhs) && dynamic_cast<FunctionAST const*>(&rhs))
   {
      return operator==(*dynamic_cast<FunctionAST const*>(&lhs), *dynamic_cast<FunctionAST const*>(&rhs));
   }
   return false;
}
