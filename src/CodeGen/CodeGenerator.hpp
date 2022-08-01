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
            std::map<std::string, llvm::AllocaInst*> namedValues;
            std::unique_ptr<llvm::IRBuilder<>> llvmBuilder;
            llvm::LLVMContext& llvmContext;
            std::unique_ptr<llvm::Module> llvmModule;
            bool error = false;
            llvm::Value* LogErrorV(yy::location loc, const char* str);
            inline llvm::Value* codeGen(const AST::Expr& expr) {const_cast<AST::Expr&>(expr).accept(*this); auto v = returnValues.top(); returnValues.pop(); return v;}
            inline llvm::Function* codeGen(const AST::Prototype& prot) {const_cast<AST::Prototype&>(prot).accept(*this); auto v = llvm::cast<llvm::Function>(returnValues.top()); returnValues.pop(); return v;}
            inline llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* function, const std::string& varName) {
                llvm::IRBuilder<> tmpB(&function->getEntryBlock(), function->getEntryBlock().begin());
                return tmpB.CreateAlloca(llvm::Type::getDoubleTy(llvmContext), 0, varName.c_str());
            }
        public:

            void visitNumber(AST::NumberExpr& num) override;
            void visitVariable(AST::VariableExpr& var) override;
            void visitBinary(AST::BinaryExpr& bin) override;
            void visitAssign(AST::AssignExpr& assign) override;
            void visitUnary(AST::UnaryExpr& unary) override;
            void visitVarInit(AST::VarInitExpr& varInit) override;
            void visitCall(AST::CallExpr& call) override;
            void visitPrototype(AST::Prototype& proto) override;
            void visitFunction(AST::Function& func) override;
            void visitIf(AST::IfExpr& ifexpr) override;
            void visitFor(AST::ForExpr& forexpr) override;

        public:
            CodeGenerator(llvm::LLVMContext& context);
            inline const llvm::Module& getModule() const {return *llvmModule;}
            inline std::unique_ptr<llvm::Module> consumeModule() {return std::unique_ptr<llvm::Module>(std::move(llvmModule));}
            inline const llvm::LLVMContext& getContext() const {return llvmContext;}
            inline bool hadError() const {return error;}


    };
}