#pragma once 
#include <string>
#include <memory>
#include <vector>
#include <iostream>
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
        public:
            enum class Type {
                PLUS,
                MINUS,
                MULT,
                DIV,
                LESS,
                GREATER,
                OR,
                AND,
                EQUALS,
                RET_RIGHT
            };
        private:
            Type op;
        public:
            std::unique_ptr<Expr> LHS, RHS;
            BinaryExpr(Type op, std::unique_ptr<Expr> LHS, std::unique_ptr<Expr> RHS) : op(op), RHS(std::move(RHS)), LHS(std::move(LHS)) {}
            void accept(Visitor& vis) override {vis.visitBinary(*this);} 
            Type getOp() const {return op;}  
    };

    class AssignExpr : public Expr {
        std::string target;
        public:
            std::unique_ptr<Expr> value;
            AssignExpr(const std::string& target, std::unique_ptr<Expr> value)
                : target(std::move(target)), value(std::move(value)) {}
            void accept(Visitor& vis) override {vis.visitAssign(*this);}
            const std::string& getTarget() const {return target;}
    };

    class UnaryExpr : public Expr {
        public: 
            enum class Type {
                NOT,
                NEGATE
            };
        private:
            Type op;
        public:
            std::unique_ptr<Expr> expr;
            UnaryExpr(Type op, std::unique_ptr<Expr> expr) : op(op), expr(std::move(expr)) {}
            void accept(Visitor& vis) override {vis.visitUnary(*this);}
            Type getOp() const {return op;}
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

    class IfExpr : public Expr {
        public:
            std::unique_ptr<Expr> cond, then, Else;
            IfExpr(std::unique_ptr<Expr> cond, std::unique_ptr<Expr> then, std::unique_ptr<Expr> Else) : cond(std::move(cond)), then(std::move(then)), Else(std::move(Else)) {}
            void accept(Visitor& vis) override {vis.visitIf(*this);}
    };

    class ForExpr : public Expr {
        std::string varName;
        public:
            std::unique_ptr<Expr> start, end, step, body;
            ForExpr(const std::string& varName, std::unique_ptr<Expr> start, 
                std::unique_ptr<Expr> end, std::unique_ptr<Expr> step, std::unique_ptr<Expr> body)
                : varName(std::move(varName)), start(std::move(start)), end(std::move(end)), step(std::move(step)), body(std::move(body)) {}
            void accept(Visitor& vis) override {vis.visitFor(*this);}
            const std::string& getVarName() const {return varName;}
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

inline std::ostream& operator<<(std::ostream& stream, const AST::BinaryExpr::Type type) {
    switch (type) {
        case AST::BinaryExpr::Type::PLUS:
            stream << "+";
            break;
        case AST::BinaryExpr::Type::MINUS:
            stream << "-";
            break;
        case AST::BinaryExpr::Type::MULT:
            stream << "*";
            break;
        case AST::BinaryExpr::Type::DIV:
            stream << "/";
            break;
        case AST::BinaryExpr::Type::LESS:
            stream << "<";
            break;
        case AST::BinaryExpr::Type::GREATER:
            stream << ">";
            break;
        case AST::BinaryExpr::Type::OR:
            stream << "|";
            break;
        case AST::BinaryExpr::Type::AND:
            stream << "&";
            break;
        case AST::BinaryExpr::Type::EQUALS:
            stream << "==";
            break;
        case AST::BinaryExpr::Type::RET_RIGHT:
            stream << ":";
            break;
    }
    return stream;
}

inline std::ostream& operator<<(std::ostream& stream, const AST::UnaryExpr::Type type) {
    switch (type) {
        case AST::UnaryExpr::Type::NEGATE:
            stream << "-";
            break;
        case AST::UnaryExpr::Type::NOT:
            stream << "!";
            break;
    }
    return stream;
}