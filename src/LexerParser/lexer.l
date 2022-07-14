%{
    #include <string>
    #include "parser.tab.hpp"
    //extern "C" int yywrap() { }
%}

%option c++
%% 
[\n \t] ;
[a-zA-z][a-zA-Z0-9]* yylval.string = std::string(yytext, yyleng); return IDENTIFIER;
[0-9]+.?[0-9]+? yylval.number = std::stod(std::string(yytext, yyleng)); return NUMBER;

%%