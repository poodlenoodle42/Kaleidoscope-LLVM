#pragma once
#include "Expression.hpp"
#include <stack>
#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <map>
namespace Visitor {
    class CodeGenerator : public AST::Visitor {
        private:
            std::stack<llvm::Value*> returnValues;
            std::map<std::string, llvm::Value*> namedValues;
            std::unique_ptr<llvm::IRBuilder<>> llvmBuilder;
            std::unique_ptr<llvm::LLVMContext> llvmContext;
            std::unique_ptr<llvm::Module> llvmModule;
            bool error = false;
            llvm::Value* LogErrorV(const char* str);
            inline llvm::Value* codeGen(const AST::Expr& expr) {const_cast<AST::Expr&>(expr).accept(*this); auto v = returnValues.top(); returnValues.pop(); return v;}
            inline llvm::Function* codeGen(const AST::Prototype& prot) {const_cast<AST::Prototype&>(prot).accept(*this); auto v = reinterpret_cast<llvm::Function*>(returnValues.top()); returnValues.pop(); return v;}
        public:

            void visitNumber(AST::NumberExpr& num) override;
            void visitVariable(AST::VariableExpr& var) override;
            void visitBinary(AST::BinaryExpr& bin) override;
            void visitCall(AST::CallExpr& call) override;
            void visitPrototype(AST::Prototype& proto) override;
            void visitFunction(AST::Function& func) override;

        public:
            CodeGenerator();
            inline const llvm::Module& getModule() const {return *llvmModule;}
            inline const llvm::LLVMContext& getContext() const {return *llvmContext;}
            inline bool hadError() const {return error;}


    };
}