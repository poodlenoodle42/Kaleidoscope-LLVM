#pragma once
#include "Visitor.hpp"

namespace Visitor {
    class Printer : public AST::Visitor {
        int indentation = -1;
        void printIndent();
        public: 
            void visitNumber(AST::NumberExpr& num) override;
            void visitVariable(AST::VariableExpr& var) override;
            void visitBinary(AST::BinaryExpr& bin) override;
            void visitAssign(AST::AssignExpr& assign) override;
            void visitUnary(AST::UnaryExpr& unary) override;
            void visitCall(AST::CallExpr& call) override;
            void visitPrototype(AST::Prototype& proto) override;
            void visitFunction(AST::Function& func) override;
            void visitIf(AST::IfExpr& ifexpr) override;
            void visitFor(AST::ForExpr& forexpr) override;
    };
}