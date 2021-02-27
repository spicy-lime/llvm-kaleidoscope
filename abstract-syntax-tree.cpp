#include <iostream>

#include "abstract-syntax-tree.hpp"

llvm::Value* NumberExprAST::codegen()
{
   return llvm::ConstantFP::get(context, llvm::APFloat(val));
}

llvm::Value* VariableExprAST::codegen()
{
   llvm::Value* value = namedValues[name];
   if(!value)
   {
      std::cerr << "Unknown variable name: " << name;
      return nullptr;
   }
   return value;
}

llvm::Value* BinaryExprAST::codegen()
{
   llvm::Value* left = lhs->codegen();
   llvm::Value* right = rhs->codegen();
   if(!left || !right)
   {
      return nullptr;
   }

   switch(op)
   {
      case('+'):
      {
         return builder.CreateFAdd(left, right, "addtmp");
      }
      case('-'):
      {
         return builder.CreateFSub(left, right, "subtmp");
      }
      case('*'):
      {
         return builder.CreateFMul(left, right, "multmp");
      }
      case('<'):
      {
         left = builder.CreateFCmpULT(left, right, "cmptmp");
         return builder.CreateUIToFP(left, llvm::Type::getDoubleTy(context), "booltmp");
      }
      default:
      {
         std::cerr << "Invalid binary operator";
         return nullptr;
      }
   }
}

llvm::Value* CallExprAST::codegen()
{
   llvm::Function *calleeFunc{mod->getFunction(callee)};
   if(!calleeFunc)
   {
      std::cerr << "Unknown function referenced";
      return nullptr;
   }

   if(calleeFunc->arg_size() != args.size())
   {
      std::cerr << "Incorrect number of arugments passed";
      return nullptr;
   }

   std::vector<llvm::Value*> argsV{};
   for(unsigned long i{0}, e{args.size()}; i != e; ++i)
   {
      argsV.push_back(args[i]->codegen());
      if(!argsV.back())
      {
         return nullptr;
      }
   }

   return builder.CreateCall(calleeFunc, argsV, "calltmp");
}

llvm::Function* PrototypeAST::codegen()
{
   std::vector<llvm::Type*> doubles(args.size(), llvm::Type::getDoubleTy(context));

   llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(context), doubles, false);

   llvm::Function* f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, name, mod.get());

   unsigned idx{0};

   for(auto& arg : f->args())
   {
      arg.setName(args[idx++]);
   }

   return f;
}

llvm::Value* FunctionAST::codegen()
{
   llvm::Function* func = mod->getFunction(proto->getName());
   if(!func)
   {
      func = proto->codegen();
   }

   if(!func)
   {
      return nullptr;
   }

   if(!func->empty())
   {
      std::cerr << "Function cannot be redefined";
      return nullptr;
   }

   llvm::BasicBlock* bb = llvm::BasicBlock::Create(context, "entry", func);
   builder.SetInsertPoint(bb);

   namedValues.clear();
   for(auto& arg : func->args())
   {
      namedValues[std::string(arg.getName())] = &arg;
   }

   if(llvm::Value* retVal = body->codegen())
   {
      builder.CreateRet(retVal);
      verifyFunction(*func);
      return func;
   }
   func->eraseFromParent();
   return nullptr;

}
