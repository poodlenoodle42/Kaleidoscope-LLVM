%skeleton "lalr1.cc" //Use C++ skeleton

%require "3.5" //Use new version, supports c++ and does not create legacy files like position.hh
%language "c++" //Well, I mean....
%locations //Generate Location type for better diagnostics
%header
%define api.value.type variant //Use real c++ types as types for terminals and nonterminals, more typesafe than %union
%define api.token.constructor //Define functions to construct token types, makes tokens type safe  
%define api.token.raw //Disables conversion between internal and external token numbers, charachter tokens like '+' can not be created anymore
%define parse.assert //Checks that symbols are constructed and destroyed properly, uses RTTI
%define api.parser.class { Parser }

%code requires {
    #include <string>
    #include <iostream>

    class ParserDriver;
    namespace yy {class Scanner;}
}
%lex-param {yy::Scanner& scanner} //Pass the driver as a parameter to yylex so he can be used in semantic actions
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

%nterm <double> num

%start program
%%

program : numbers;
numbers : numbers num {std::cout << $2 << "\n";}
        | num {std::cout << $1 << "\n";};
num : NUMBER {$$ = $1;}; //Semantic action does not need to be explicit because it is the default rule
%%

void
yy::Parser::error (const location_type& l, const std::string& m)
{
  std::cerr << l << ": " << m << '\n';
}