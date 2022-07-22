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
        RETURN(llvmBuilder->CreateLoad(llvm::Type::getDoubleTy(llvmContext),v, var.getName().c_str()));
    }

    void CodeGenerator::visitBinary(AST::BinaryExpr& binary) {
        llvm::Value* lhs = codeGen(*binary.LHS);
        llvm::Value* rhs = codeGen(*binary.RHS);
        if (!lhs || !rhs) {
            RETURN(nullptr);
        }
        switch(binary.getOp()) {
            case AST::BinaryExpr::Type::PLUS:
                RETURN(llvmBuilder->CreateFAdd(lhs,rhs,"addtmp"));
            case AST::BinaryExpr::Type::MINUS:
                RETURN(llvmBuilder->CreateFSub(lhs,rhs,"subtmp"));
            case AST::BinaryExpr::Type::MULT: 
                RETURN(llvmBuilder->CreateFMul(lhs,rhs,"multmp"));
            case AST::BinaryExpr::Type::DIV:
                RETURN(llvmBuilder->CreateFDiv(lhs,rhs,"divtmp"));
            case AST::BinaryExpr::Type::LESS:
                lhs = llvmBuilder->CreateFCmpULT(lhs, rhs, "lesstmp");
                RETURN(llvmBuilder->CreateUIToFP(lhs, llvm::Type::getDoubleTy(llvmContext)));
            case AST::BinaryExpr::Type::GREATER:
                lhs = llvmBuilder->CreateFCmpUGT(lhs,rhs, "greatertmp");
                RETURN(llvmBuilder->CreateUIToFP(lhs, llvm::Type::getDoubleTy(llvmContext)));
            case AST::BinaryExpr::Type::EQUALS:
                lhs = llvmBuilder->CreateFCmpUGT(lhs,rhs, "equalstmp");
                RETURN(llvmBuilder->CreateUIToFP(lhs, llvm::Type::getDoubleTy(llvmContext)));
            case AST::BinaryExpr::Type::OR:
                lhs = llvmBuilder->CreateFPToUI(lhs, llvm::Type::getInt1Ty(llvmContext), "lhs");
                rhs = llvmBuilder->CreateFPToUI(rhs, llvm::Type::getInt1Ty(llvmContext), "rhs");
                lhs = llvmBuilder->CreateOr(lhs, rhs, "ortmp");
                RETURN(llvmBuilder->CreateUIToFP(lhs, llvm::Type::getDoubleTy(llvmContext)));
            case AST::BinaryExpr::Type::AND:
                lhs = llvmBuilder->CreateFPToUI(lhs, llvm::Type::getInt1Ty(llvmContext), "lhs");
                rhs = llvmBuilder->CreateFPToUI(rhs, llvm::Type::getInt1Ty(llvmContext), "rhs");
                lhs = llvmBuilder->CreateAnd(lhs, rhs, "andtmp");
                RETURN(llvmBuilder->CreateUIToFP(lhs, llvm::Type::getDoubleTy(llvmContext)));
            case AST::BinaryExpr::Type::RET_RIGHT:
                RETURN(rhs); //Evaluate both but return value of right expression
            default: //Does not happen because node is created in the parser with controlled operators. But if I forget to add a case statement for a new operator ....
                RETURN(LogErrorV("Unknown binary operator"));
        }
    }

    void CodeGenerator::visitAssign(AST::AssignExpr& assign) {
        llvm::Value* val = codeGen(*assign.value);
        if (!val) {RETURN(nullptr);}
        llvm::Value* variable = namedValues[assign.getTarget()];
        if (!variable) {RETURN(LogErrorV("Unknown variable name"));}
        llvmBuilder->CreateStore(val, variable);
        RETURN(val);

    }

    void CodeGenerator::visitUnary(AST::UnaryExpr& unary) {
        llvm::Value* expr = codeGen(*unary.expr);
        switch (unary.getOp()) {
            case AST::UnaryExpr::Type::NEGATE:
                RETURN(llvmBuilder->CreateFNeg(expr, "negtmp"));
            case AST::UnaryExpr::Type::NOT:
                expr = llvmBuilder->CreateFPToUI(expr, llvm::Type::getInt1Ty(llvmContext), "notbool");
                expr = llvmBuilder->CreateNot(expr, "notboolNegated");
                RETURN(llvmBuilder->CreateUIToFP(expr, llvm::Type::getDoubleTy(llvmContext)));
            default:
                RETURN(LogErrorV("Unknown unary operator"));
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

    void CodeGenerator::visitIf(AST::IfExpr& ifexpr) {
        llvm::Value* condV = codeGen(*ifexpr.cond);
        if (!condV) {RETURN(nullptr);}
        condV = llvmBuilder->CreateFCmpONE(condV, llvm::ConstantFP::get(llvmContext, llvm::APFloat(0.0)), "ifcond");

        llvm::Function* function = llvmBuilder->GetInsertBlock()->getParent();
        llvm::BasicBlock* thenBB = llvm::BasicBlock::Create(llvmContext, "then", function);
        llvm::BasicBlock* elseBB = llvm::BasicBlock::Create(llvmContext,"else");
        llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(llvmContext, "ifcont");
        llvmBuilder->CreateCondBr(condV, thenBB, elseBB);

        llvmBuilder->SetInsertPoint(thenBB);
        llvm::Value* thenV = codeGen(*ifexpr.then);
        if (!thenV) {RETURN(nullptr);}

        llvmBuilder->CreateBr(mergeBB);

        thenBB = llvmBuilder->GetInsertBlock();

        function->getBasicBlockList().push_back(elseBB);
        llvmBuilder->SetInsertPoint(elseBB);
        llvm::Value* elseV = codeGen(*ifexpr.Else);
        llvmBuilder->CreateBr(mergeBB);
        elseBB = llvmBuilder->GetInsertBlock();

        function->getBasicBlockList().push_back(mergeBB);
        llvmBuilder->SetInsertPoint(mergeBB);
        llvm::PHINode* pn = llvmBuilder->CreatePHI(llvm::Type::getDoubleTy(llvmContext), 2, "iftmp");
        pn->addIncoming(thenV, thenBB);
        pn->addIncoming(elseV, elseBB);
        RETURN(pn);

    }

    void CodeGenerator::visitFor(AST::ForExpr& forexpr) {
        llvm::Function* function = llvmBuilder->GetInsertBlock()->getParent();
        llvm::AllocaInst* alloca = createEntryBlockAlloca(function, forexpr.getVarName());
        llvm::Value* startVal = codeGen(*forexpr.start);
        if (!startVal) {RETURN(nullptr);}
        
        llvmBuilder->CreateStore(startVal, alloca);


        llvm::BasicBlock* preheaderBB = llvmBuilder->GetInsertBlock();
        llvm::BasicBlock* loopBB = llvm::BasicBlock::Create(llvmContext, "loop", function);
        llvmBuilder->CreateBr(loopBB);
        llvmBuilder->SetInsertPoint(loopBB);
        
        llvm::AllocaInst* oldVal = namedValues[forexpr.getVarName()];
        namedValues[forexpr.getVarName()] = alloca;
        if (!codeGen(*forexpr.body)) {RETURN(nullptr);}
        llvm::Value* stepVal = nullptr;
        if (forexpr.step) {
            stepVal = codeGen(*forexpr.step);
            if (!stepVal) {RETURN(nullptr);}
        } else {
            stepVal = llvm::ConstantFP::get(llvmContext, llvm::APFloat(1.0));
        }

        llvm::Value* endCond = codeGen(*forexpr.end);
        if (!endCond) {RETURN(nullptr);}
        endCond = llvmBuilder->CreateFCmpONE(endCond, llvm::ConstantFP::get(llvmContext, llvm::APFloat(0.0)), "loopcond");

        llvm::Value* curVar = llvmBuilder->CreateLoad(llvm::Type::getDoubleTy(llvmContext),alloca);
        llvm::Value* nextVar = llvmBuilder->CreateFAdd(curVar, stepVal, "nextvar");
        llvmBuilder->CreateStore(nextVar, alloca);

        llvm::BasicBlock* afterBB = llvm::BasicBlock::Create(llvmContext, "afterloop", function);
        llvmBuilder->CreateCondBr(endCond, loopBB, afterBB);
        llvmBuilder->SetInsertPoint(afterBB);

        

        if (oldVal) namedValues[forexpr.getVarName()] = oldVal;
        else namedValues.erase(forexpr.getVarName());

        return RETURN(llvm::Constant::getNullValue(llvm::Type::getDoubleTy(llvmContext)));


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
            llvm::AllocaInst* alloca = createEntryBlockAlloca(function, arg.getName().str());
            llvmBuilder->CreateStore(&arg, alloca);
            namedValues[arg.getName().str()] = alloca;
        }
        if(llvm::Value* retVal = codeGen(*func.body)) {
            llvmBuilder->CreateRet(retVal);
            error = llvm::verifyFunction(*function);
            return;
            //RETURN(function);
        }
        //Error with body
        function->eraseFromParent();
        
    }


}