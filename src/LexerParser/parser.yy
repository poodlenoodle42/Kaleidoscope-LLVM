%skeleton "lalr1.cc" //Use C++ skeleton

%require "3.5" //Use new version, supports c++ and does not create legacy files like position.hh
%language "c++" //Well, I mean....
%locations //Generate Location type for better diagnostics
%header
%define api.value.automove //Auto moves values of terminals/nonterminals (e.g. $1), needed for easy use of unique_ptr
%define api.value.type variant //Use real c++ types as types for terminals and nonterminals, more typesafe than %union
%define api.token.constructor //Define functions to construct token types, makes tokens type safe  
%define api.token.raw //Disables conversion between internal and external token numbers, charachter tokens like '+' can not be created anymore
%define parse.assert //Checks that symbols are constructed and destroyed properly, uses RTTI
%define api.parser.class { Parser }

%code requires {
    #include <string>
    #include <iostream>

    #include <Expression.hpp>
    typedef std::unique_ptr<AST::Expr> ExprPtr;
    typedef std::unique_ptr<AST::Function> FuncPtr;
    typedef std::unique_ptr<AST::Prototype> ProtoPtr;

    class ParserDriver;
    namespace yy {class Scanner;}
}
%lex-param {yy::Scanner& scanner}
%parse-param {ParserDriver& drv} //Pass the driver as a parameter to yyparse so he can be used in semantic actions
%parse-param {Scanner& scanner}
//Good syntax error messages
%define parse.trace
%define parse.error detailed
%define parse.lac full

%code {
    #include "ParserDriver.hpp"
    #include "Scanner.hpp"

    static yy::Parser::symbol_type yylex(yy::Scanner& scanner) {
        return scanner.get_next_token();
    }
}

%token DEF EXTERN 
%token <std::string> IDENTIFIER
%token <double> NUMBER
%token ADD "+" MINUS "-" STAR "*" SLASH "/" EQUALS "="
%token UNARY //Only used as precedence for unary operators
%left ADD MINUS
%left STAR SLASH
%left UNARY

%nterm <ExprPtr> num expr

%start program
%%

program : expr {drv.root = $1;};

expr: expr "+" expr {$$ = std::make_unique<AST::BinaryExpr>('+', $1, $3);}
| expr "-" expr     {$$ = std::make_unique<AST::BinaryExpr>('-', $1, $3);}
| expr "*" expr     {$$ = std::make_unique<AST::BinaryExpr>('*', $1, $3);}
| expr "/" expr     {$$ = std::make_unique<AST::BinaryExpr>('/', $1, $3);}
| "-" expr %prec UNARY {$$ = $2;} //Should introduce negate expression node later
| num;
num : NUMBER {$$ = std::make_unique<AST::NumberExpr>($1);}; //Semantic action does not need to be explicit because it is the default rule
%%

void
yy::Parser::error (const location_type& l, const std::string& m)
{
  std::cerr << l << ": " << m << '\n';
}