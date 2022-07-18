#include "CodeGenerator.hpp"
#include <iostream>
//All LLVM Includes from the tutorial
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

#define RETURN(stmt) returnValues.push(stmt); return

namespace Visitor {
    CodeGenerator::CodeGenerator(llvm::LLVMContext& context) : llvmContext(context) {
        llvmBuilder = std::make_unique<llvm::IRBuilder<>>(llvmContext);
        llvmModule = std::make_unique<llvm::Module>("my cool jit", llvmContext);
    }

    llvm::Value* CodeGenerator::LogErrorV(const char* str) {
        error = true;
        std::cerr << "Error: " << str << "\n";
        return nullptr;
    }
    void CodeGenerator::visitNumber(AST::NumberExpr& number) {
        RETURN(llvm::ConstantFP::get(llvmContext, llvm::APFloat(number.getVal())));
    }

    void CodeGenerator::visitVariable(AST::VariableExpr& var) {
        llvm::Value* v = namedValues[var.getName()];
        if (!v) {LogErrorV("Unknown variable name");}
        RETURN(v);
    }

    void CodeGenerator::visitBinary(AST::BinaryExpr& binary) {
        llvm::Value* lhs = codeGen(*binary.LHS);
        llvm::Value* rhs = codeGen(*binary.RHS);
        if (!lhs || !rhs) {
            RETURN(nullptr);
        }
        switch(binary.getOp()) {
            case '+':
                RETURN(llvmBuilder->CreateFAdd(lhs,rhs,"addtmp"));
            case '-':
                RETURN(llvmBuilder->CreateFSub(lhs,rhs,"subtmp"));
            case '*': 
                RETURN(llvmBuilder->CreateFMul(lhs,rhs,"multmp"));
            case '/':
                RETURN(llvmBuilder->CreateFDiv(lhs,rhs,"divtmp"));
            default: //Does not happen because node is created in the parser with controlled operators. But if I forget to add a case statement for a new operator ....
                RETURN(LogErrorV("Unknown binary operator"));
        }
    }

    void CodeGenerator::visitCall(AST::CallExpr& call) {
        llvm::Function* calleeF = llvmModule->getFunction(call.getCallee());
        if(!calleeF) {RETURN(LogErrorV("Unknown function referenced"));}
        if(calleeF->arg_size() != call.args.size()) {
            RETURN(LogErrorV("Incorrect number of arguments parsed"));
        }
        std::vector<llvm::Value*> argsV;
        for(auto& arg : call.args) {
            argsV.push_back(codeGen(*arg));
            if(!argsV.back()) {RETURN(nullptr);}
        }
        RETURN(llvmBuilder->CreateCall(calleeF, argsV, "calltmp"));
    }

    void CodeGenerator::visitPrototype(AST::Prototype& prot) {
        std::vector<llvm::Type*> argumentTypes(prot.getArgs().size(), llvm::Type::getDoubleTy(llvmContext));
        llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(llvmContext), argumentTypes, false);
        llvm::Function* f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, prot.getName(), *llvmModule);

        unsigned idx = 0;
        for (auto& arg : f->args()) {
            arg.setName((prot.getArgs()[idx++]));
        }
        RETURN(f);
    }

    void CodeGenerator::visitFunction(AST::Function& func) {
        llvm::Function* function = llvmModule->getFunction(func.getProto().getName());
        if(!function) {function = codeGen(func.getProto());}
        if(!function) return;
        if(!function->empty()) {LogErrorV("Function can not be redefined."); return;}
        llvm::BasicBlock* bb = llvm::BasicBlock::Create(llvmContext, "entry", function);
        llvmBuilder->SetInsertPoint(bb);
        namedValues.clear();
        for(auto& arg : function->args()) {
            namedValues[std::string(arg.getName())] = &arg;
        }
        if(llvm::Value* retVal = codeGen(*func.body)) {
            llvmBuilder->CreateRet(retVal);
            llvm::verifyFunction(*function);
            return;
            //RETURN(function);
        }
        //Error with body
        function->eraseFromParent();
        
    }


}