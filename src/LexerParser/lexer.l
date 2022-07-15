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
%% 

%{
    //yy::location& loc = drv.location;
    loc.step();
%}
[\t \r] loc.step();
\n+     loc.lines(yyleng); loc.step();
[a-zA-z][a-zA-Z0-9]*    return yy::Parser::make_IDENTIFIER(yytext, loc);
[0-9]+.?[0-9]+?         return yy::Parser::make_NUMBER(std::stod(yytext), loc);
<<EOF>>                 return yy::Parser::make_YYEOF(loc);
%%

yy::Scanner::Scanner(ParserDriver& driver) : driver(driver), yyFlexLexer(&std::cin) {loc.initialize(&driver.file);}
yy::Scanner::Scanner(ParserDriver& driver, std::istream& in) : driver(driver), yyFlexLexer(&in) {loc.initialize(&driver.file);} 