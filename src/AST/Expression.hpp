#pragma once 
#include <string>
#include <memory>
#include <vector>
#include "Visitor.hpp"
namespace AST {

    class Expr {
        public:
            virtual ~Expr() {}
            virtual void accept(Visitor& vis) = 0;
    };

    class NumberExpr : public Expr {
        double val;
        public: 
            NumberExpr(double val) : val(val) {}
            void accept(Visitor& vis) override {vis.visitNumber(*this);}
    };

    class VariableExpr : public Expr {
        std::string name;

        public:
            VariableExpr(const std::string& name) : name(std::move(name)) {}
            void accept(Visitor& vis) override {vis.visitVariable(*this);}
    };

    class BinaryExpr : public Expr {
        char op;
        std::unique_ptr<Expr> LHS, RHS;
        public:
            BinaryExpr(char op, std::unique_ptr<Expr> LHS, std::unique_ptr<Expr> RHS) : op(op), RHS(std::move(RHS)), LHS(std::move(LHS)) {}
            void accept(Visitor& vis) override {vis.visitBinary(*this);}   
    };

    class CallExpr : public Expr {
        std::string callee;
        std::vector<std::unique_ptr<Expr>> args;
        public:
            CallExpr(const std::string& callee, std::vector<std::unique_ptr<Expr>>& args) : callee(std::move(callee)), args(std::move(args)) {}
            void accept(Visitor& vis) override {vis.visitCall(*this);}
            void add_expr(std::unique_ptr<Expr> expr) {args.push_back(std::move(expr));}
    };

    class Prototype {
        std::string name;
        std::vector<std::string> args;
        public:
            Prototype(const std::string& name, std::vector<std::string>& args) : name(std::move(name)), args(std::move(args)) {}
            void accept(Visitor& vis) {vis.visitPrototype(*this);}
            void add_argument(const std::string& arg) {args.push_back(std::move(arg));}
    };

    class Function {
        std::unique_ptr<Prototype> proto;
        std::unique_ptr<Expr> body;
        public:
            Function(std::unique_ptr<Prototype> proto, std::unique_ptr<Expr> expr) : proto(std::move(proto)), body(std::move(expr)) {}
            void accept(Visitor& vis) {vis.visitFunction(*this);}
    };
};
