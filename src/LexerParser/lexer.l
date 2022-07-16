%{
    #include <string>
        
    #include "ParserDriver.hpp"
    #include "parser.tab.hpp"
    #include "Scanner.hpp"

    #define YY_USER_ACTION loc.columns(yyleng);
    
%}


%option noyywrap nounput noinput batch debug c++
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
{identifier}            return yy::Parser::make_IDENTIFIER(yytext, loc);
{number}.?{number}?     return yy::Parser::make_NUMBER(std::stod(yytext), loc);
"+"                     return yy::Parser::make_ADD(loc);
"-"                     return yy::Parser::make_MINUS(loc);
"*"                     return yy::Parser::make_STAR(loc);
"/"                     return yy::Parser::make_SLASH(loc);
"="                     return yy::Parser::make_EQUALS(loc);
<<EOF>>                 return yy::Parser::make_YYEOF(loc);
%%

yy::Scanner::Scanner(ParserDriver& driver) : driver(driver), yyFlexLexer(&std::cin) {loc.initialize(&driver.file);}
yy::Scanner::Scanner(ParserDriver& driver, std::istream& in) : driver(driver), yyFlexLexer(&in) {loc.initialize(&driver.file);} 