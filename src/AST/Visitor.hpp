#pragma once 
namespace AST {
    class NumberExpr;
    class VariableExpr;
    class BinaryExpr;
    class AssignExpr;
    class UnaryExpr;
    class VarInitExpr;
    class CallExpr;
    class IfExpr;
    class ForExpr;
    class Prototype;
    class Function;
    class Visitor {
        public:
            virtual void visitNumber(NumberExpr& num) = 0;
            virtual void visitVariable(VariableExpr& var) = 0;
            virtual void visitBinary(BinaryExpr& bin) = 0;
            virtual void visitAssign(AssignExpr& assign) = 0;
            virtual void visitUnary(UnaryExpr& unary) = 0;
            virtual void visitVarInit(VarInitExpr& varInit) = 0;
            virtual void visitCall(CallExpr& call) = 0;
            virtual void visitPrototype(Prototype& proto) = 0;
            virtual void visitFunction(Function& func) = 0;
            virtual void visitIf(IfExpr& ifexpr) = 0;
            virtual void visitFor(ForExpr& forexpr) = 0;
    };
}