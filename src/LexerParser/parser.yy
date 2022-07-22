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
%token ADD "+" MINUS "-" STAR "*" SLASH "/" EQUALS "=" LPAREN "(" RPAREN ")" COMMA "," SEMICOLON ";"
%token LESS "<" GREATER ">" NOT "!" OR "|" AND "&" EQUALEQUAL "==" DOUBLEPOINT ":"
%token IF "if" THEN "then" ELSE "else" FOR "for" IN "in"
%token UNARY //Only used as precedence for unary operators

%precedence IN ELSE
%left DOUBLEPOINT
%right EQUALS
%left OR AND
%left LESS GREATER EQUALEQUAL
%left ADD MINUS
%left STAR SLASH
%left UNARY


%nterm <ExprPtr> expr
%nterm <std::vector<ExprPtr>> arglist
%nterm <ProtoPtr> prototype extern
%nterm <std::vector<std::string>> idlist
%nterm <FuncPtr> function
%nterm top_level_list top_level_item
%start program
%%

program : top_level_list;

top_level_list: top_level_list top_level_item ";"
| %empty;

top_level_item:
  expr    {drv.getAST().addTopLevel($1);}
| extern    {drv.getAST().addExternalFunction($1);}
| function  {drv.getAST().addFunction($1);}
;


expr:
  "if" expr "then" expr "else" expr {$$ = std::make_unique<AST::IfExpr>($2,$4, $6);}
| "for" IDENTIFIER "=" expr "," expr "in" expr          {$$ = std::make_unique<AST::ForExpr>($2, $4, $6, nullptr, $8);}
| "for" IDENTIFIER "=" expr "," expr "," expr "in" expr {$$ = std::make_unique<AST::ForExpr>($2, $4, $6, $8, $10);}
| expr "+" expr     {$$ = std::make_unique<AST::BinaryExpr>(AST::BinaryExpr::Type::PLUS, $1, $3);}
| expr "-" expr     {$$ = std::make_unique<AST::BinaryExpr>(AST::BinaryExpr::Type::MINUS, $1, $3);}
| expr "*" expr     {$$ = std::make_unique<AST::BinaryExpr>(AST::BinaryExpr::Type::MULT, $1, $3);}
| expr "/" expr     {$$ = std::make_unique<AST::BinaryExpr>(AST::BinaryExpr::Type::DIV, $1, $3);}
| expr "<" expr     {$$ = std::make_unique<AST::BinaryExpr>(AST::BinaryExpr::Type::LESS, $1, $3);}
| expr ">" expr     {$$ = std::make_unique<AST::BinaryExpr>(AST::BinaryExpr::Type::GREATER, $1, $3);}
| expr "|" expr     {$$ = std::make_unique<AST::BinaryExpr>(AST::BinaryExpr::Type::OR, $1, $3);}
| expr "&" expr     {$$ = std::make_unique<AST::BinaryExpr>(AST::BinaryExpr::Type::AND, $1, $3);}
| expr "==" expr    {$$ = std::make_unique<AST::BinaryExpr>(AST::BinaryExpr::Type::EQUALS, $1, $3);}
| expr ":" expr     {{$$ = std::make_unique<AST::BinaryExpr>(AST::BinaryExpr::Type::RET_RIGHT, $1, $3);}}
| "-" expr %prec UNARY {$$ = std::make_unique<AST::UnaryExpr>(AST::UnaryExpr::Type::NEGATE, $2);} //Should introduce negate expression node later
| "!" expr %prec UNARY {$$ = std::make_unique<AST::UnaryExpr>(AST::UnaryExpr::Type::NOT, $2);}
| IDENTIFIER "=" expr  {$$ = std::make_unique<AST::AssignExpr>($1, $3);}
| IDENTIFIER "(" arglist ")" {$$ = std::make_unique<AST::CallExpr>($1, $3);}
| "(" expr ")"      {$$ = $2;}
| IDENTIFIER        {$$ = std::make_unique<AST::VariableExpr>($1);}
| NUMBER            {$$ = std::make_unique<AST::NumberExpr>($1);}
//| top_level_expr    {$$ = $1;} //Expressions allowed on the top level are of course also allowed anywhere else
;

prototype: IDENTIFIER "(" idlist ")" {$$ = std::make_unique<AST::Prototype>($1,$3);}
extern: EXTERN prototype {$$ = $2;}
function: DEF prototype expr {$$ = std::make_unique<AST::Function>($2,$3);}

idlist:
  idlist IDENTIFIER {auto v = $1; v.push_back($2); $$ = std::move(v);}
//| IDENTIFIER            {$$ = std::vector<std::string>(); $$.push_back($1);}
| %empty                {$$ = std::vector<std::string>();}

arglist: 
  arglist "," expr  {auto v = $1; v.push_back($3); $$ = std::move(v);}
| expr              {$$ = std::vector<std::unique_ptr<AST::Expr>>(); $$.push_back($1);}
| %empty            {$$ = std::vector<std::unique_ptr<AST::Expr>>();}
;

%%

void
yy::Parser::error (const location_type& l, const std::string& m)
{
  std::cerr << l << ": " << m << '\n';
}