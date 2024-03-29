%{
    #include <string>
        
    #include "ParserDriver.hpp"
    #include "parser.tab.hpp"
    #include "Scanner.hpp"

    #define YY_USER_ACTION loc.columns(yyleng);
    
%}


%option noyywrap nounput noinput debug c++
%option yyclass="yy::Scanner"
%option prefix="yy_"

identifier  [a-zA-z][a-zA-Z0-9]* 
number      [0-9]
%% 

%{
    //yy::location& loc = drv.location;
    loc.step();
%}
#.* ; //Comment
[\t \r] loc.step();
\n+     loc.lines(yyleng); loc.step();
"def"                   return yy::Parser::make_DEF(loc);
"extern"                return yy::Parser::make_EXTERN(loc);
"if"                    return yy::Parser::make_IF(loc);
"then"                  return yy::Parser::make_THEN(loc);
"else"                  return yy::Parser::make_ELSE(loc);
"for"                   return yy::Parser::make_FOR(loc);
"in"                    return yy::Parser::make_IN(loc);
"var"                   return yy::Parser::make_VAR(loc);
{identifier}            return yy::Parser::make_IDENTIFIER(yytext, loc);
{number}*\.?{number}*   return yy::Parser::make_NUMBER(std::stod(yytext), loc);
"+"                     return yy::Parser::make_ADD(loc);
"-"                     return yy::Parser::make_MINUS(loc);
"*"                     return yy::Parser::make_STAR(loc);
"/"                     return yy::Parser::make_SLASH(loc);
"="                     return yy::Parser::make_EQUALS(loc);
"=="                    return yy::Parser::make_EQUALEQUAL(loc);
"("                     return yy::Parser::make_LPAREN(loc);
")"                     return yy::Parser::make_RPAREN(loc);
","                     return yy::Parser::make_COMMA(loc);
";"                     return yy::Parser::make_SEMICOLON(loc);
"<"                     return yy::Parser::make_LESS(loc);
">"                     return yy::Parser::make_GREATER(loc);
"|"                     return yy::Parser::make_OR(loc);
"&"                     return yy::Parser::make_AND(loc);
":"                     return yy::Parser::make_DOUBLEPOINT(loc);
<<EOF>>                 return yy::Parser::make_YYEOF(loc);
%%

yy::Scanner::Scanner(ParserDriver& driver) : driver(driver), yyFlexLexer(&std::cin) {loc.initialize(&driver.get_file_name());}
yy::Scanner::Scanner(ParserDriver& driver, std::istream& in) : driver(driver), yyFlexLexer(&in) {loc.initialize(&driver.get_file_name());} 