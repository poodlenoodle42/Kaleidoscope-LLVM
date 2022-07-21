#pragma once 
namespace AST {
    class NumberExpr;
    class VariableExpr;
    class BinaryExpr;
    class CallExpr;
    class IfExpr;
    class Prototype;
    class Function;
    class Visitor {
        public:
            virtual void visitNumber(NumberExpr& num) = 0;
            virtual void visitVariable(VariableExpr& var) = 0;
            virtual void visitBinary(BinaryExpr& bin) = 0;
            virtual void visitCall(CallExpr& call) = 0;
            virtual void visitPrototype(Prototype& proto) = 0;
            virtual void visitFunction(Function& func) = 0;
            virtual void visitIf(IfExpr& ifexpr) = 0;
    };
}