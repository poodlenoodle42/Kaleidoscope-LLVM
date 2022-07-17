#pragma once
#include "Expression.hpp"
#include <stack>
namespace Visitor {
    class CodeGenerator : public AST::Visitor {
        public:
            void visitNumber(AST::NumberExpr& num) override;
            void visitVariable(AST::VariableExpr& var) override;
            void visitBinary(AST::BinaryExpr& bin) override;
            void visitCall(AST::CallExpr& call) override;
            void visitPrototype(AST::Prototype& proto) override;
            void visitFunction(AST::Function& func) override;
    };
}