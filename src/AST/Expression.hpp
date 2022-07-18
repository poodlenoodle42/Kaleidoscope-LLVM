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
            double getVal() const {return val;}
    };

    class VariableExpr : public Expr {
        std::string name;

        public:
            VariableExpr(const std::string&& name) : name(name) {}
            void accept(Visitor& vis) override {vis.visitVariable(*this);}
            const std::string& getName() const {return name;}
    };

    class BinaryExpr : public Expr {
        char op;
        public:
            std::unique_ptr<Expr> LHS, RHS;
            BinaryExpr(char op, std::unique_ptr<Expr> LHS, std::unique_ptr<Expr> RHS) : op(op), RHS(std::move(RHS)), LHS(std::move(LHS)) {}
            void accept(Visitor& vis) override {vis.visitBinary(*this);} 
            char getOp() const {return op;}  
    };

    class CallExpr : public Expr {
        std::string callee;
        
        public:
            std::vector<std::unique_ptr<Expr>> args;
            CallExpr(const std::string&& callee, std::vector<std::unique_ptr<Expr>>&& args) : callee(callee), args(std::move(args)) {}
            void accept(Visitor& vis) override {vis.visitCall(*this);}
            void addExpr(std::unique_ptr<Expr> expr) {args.push_back(std::move(expr));}
            const std::string& getCallee() const {return callee;}

    };

    class Prototype {
        std::string name;
        std::vector<std::string> args;
        public:
            Prototype(const std::string&& name, std::vector<std::string>&& args) : name(name), args(std::move(args)) {}
            void accept(Visitor& vis) {vis.visitPrototype(*this);}
            void addArgument(const std::string& arg) {args.push_back(std::move(arg));}
            const std::string& getName() const {return name;}
            const std::vector<std::string>& getArgs() {return args;}

    };

    class Function {
        std::unique_ptr<Prototype> proto;
        public:
            std::unique_ptr<Expr> body;
            Function(std::unique_ptr<Prototype> proto, std::unique_ptr<Expr> expr) : proto(std::move(proto)), body(std::move(expr)) {}
            void accept(Visitor& vis) {vis.visitFunction(*this);}
            const Prototype& getProto() const {return *proto;}
    };
};
