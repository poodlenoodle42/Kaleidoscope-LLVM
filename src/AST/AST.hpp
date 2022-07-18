#pragma once

#include "Expression.hpp"
typedef std::unique_ptr<AST::Expr> ExprPtr;
typedef std::unique_ptr<AST::Prototype> ProtoPtr;
typedef std::unique_ptr<AST::Function> FuncPtr;

namespace AST {
    class AST {
        private:
            FuncPtr topLevelFunction;
            std::vector<ProtoPtr> externFunctions;
            std::vector<FuncPtr> functions;
        public:
            AST() : topLevelFunction(new Function(std::make_unique<Prototype>("top_level_expr", std::vector<std::string>()), nullptr)) {}
            void addTopLevel(ExprPtr expr) {topLevelFunction->body = std::move(expr);}
            void addExternalFunction(ProtoPtr proto) {externFunctions.push_back(std::move(proto));}
            void addFunction(FuncPtr func) {functions.push_back(std::move(func));}
            void accept(Visitor& vis) {
                for(auto& proto : externFunctions) {
                    proto->accept(vis);
                }
                for(auto& func : functions) {
                    func->accept(vis);
                }
                topLevelFunction->accept(vis);
            }


    };
}