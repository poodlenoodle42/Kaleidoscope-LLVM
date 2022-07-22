#include "Printer.hpp"
#include "Expression.hpp"
#include <iostream>
using namespace AST;
namespace Visitor {

    void Printer::printIndent() {
        for (int i = 0; i<indentation; i++) {
            std::cout << "\t";
        }
    }
    #define PRINT_METHOD(stmts) indentation++; printIndent(); stmts indentation--;
    void Printer::visitNumber(NumberExpr& expr) {
        PRINT_METHOD(
            std::cout << "[NUMBER: " << expr.getVal() << "]\n";
        )
    }

    void Printer::visitVariable(VariableExpr& expr) {
        PRINT_METHOD(
            std::cout << "[VARIABLE: " << expr.getName() << "]\n";
        )
    }

    void Printer::visitBinary(BinaryExpr& expr) {
        PRINT_METHOD(
            std::cout << "[Binary: " << expr.getOp() << "]\n";
            expr.LHS->accept(*this);
            expr.RHS->accept(*this);
        )
    }

    void Printer::visitAssign(AssignExpr& expr) {
        PRINT_METHOD(
            std::cout << "[Assign: " << expr.getTarget() << "]\n";
            expr.value->accept(*this);
        )
    }

    void Printer::visitUnary(UnaryExpr& expr) {
        PRINT_METHOD(
            std::cout << "[Unary: " << expr.getOp() << "]\n";
            expr.expr->accept(*this);
        )
    }

    void Printer::visitVarInit(VarInitExpr& varInit) {
        PRINT_METHOD(
            std::cout << "[VarInit\n";
            printIndent();
            for (const auto& variable : varInit.varNames) {
                std::cout << "Init " << variable.first;
                if (variable.second.get() == nullptr) {
                    std::cout << "\n";
                } else {
                    std::cout << " with\n";
                    variable.second->accept(*this);
                }
                printIndent();
            }
        )
    }

    void Printer::visitCall(CallExpr& expr) {
        PRINT_METHOD(
            std::cout << "[Call " << expr.getCallee() << "]\n";
            for(const auto& arg : expr.args) {
                arg->accept(*this);
            }
        )
    }

    void Printer::visitIf(IfExpr& expr) {
        PRINT_METHOD(
            std::cout << "[if\n";
            expr.cond->accept(*this);
            printIndent();
            std::cout << "then\n";
            expr.then->accept(*this);
            printIndent();
            std::cout << "else\n";
            expr.Else->accept(*this);
            printIndent();
            std::cout << "]\n";
        )
    }

    void Printer::visitFor(ForExpr& expr) {
        PRINT_METHOD(
            std::cout << "[for " << expr.getVarName() << " init\n";
            expr.start->accept(*this);
            printIndent();
            std::cout << "condition\n";
            expr.end->accept(*this);
            printIndent();
            if (expr.step) {
                std::cout << "step\n";
                expr.step->accept(*this);
                printIndent();
            } 
            std::cout << "body\n";
            expr.body->accept(*this);
            printIndent();
            std::cout << "]\n";

        )
    }

    void Printer::visitPrototype(Prototype& proto) {
        PRINT_METHOD(
            std::cout << "[Prototype " << proto.getName() << "(";
            for(const auto& name: proto.getArgs()) {
                std::cout << name << " ";
            }
            std::cout << ")]\n";
        )
    }

    void Printer::visitFunction(Function& func) {
        PRINT_METHOD(
            std::cout << "[Function]\n";
            const_cast<AST::Prototype&>(func.getProto()).accept(*this);
            func.body->accept(*this);
        )
    }
}