#pragma once

#include "Expression.hpp"
typedef std::unique_ptr<AST::Expr> ExprPtr;
typedef std::unique_ptr<AST::Prototype> ProtoPtr;
typedef std::unique_ptr<AST::Function> FuncPtr;

namespace AST {
    class AST {
        private:
            std::vector<ExprPtr> topLevelExpressions;
            std::vector<ProtoPtr> externFunctions;
            std::vector<FuncPtr> functions;
        public:
            void addTopLevel(ExprPtr expr) {topLevelExpressions.push_back(std::move(expr));}
            void addExternalFunction(ProtoPtr proto) {externFunctions.push_back(std::move(proto));}
            void addFunction(FuncPtr func) {functions.push_back(std::move(func));}
            void accept(Visitor& vis) {
                for(auto& proto : externFunctions) {
                    proto->accept(vis);
                }
                for(auto& func : functions) {
                    func->accept(vis);
                }
                for(auto& expr : topLevelExpressions) {
                    expr->accept(vis);
                }
            }


    };
}