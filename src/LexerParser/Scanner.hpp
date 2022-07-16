#pragma once 

#if ! defined(yyFlexLexerOnce)
#undef yyFlexLexer
#define yyFlexLexer yy_FlexLexer // the trick with prefix; no namespace here :(
#include <FlexLexer.h>
#endif

#include "location.hh"

#undef YY_DECL
#define YY_DECL yy::Parser::symbol_type yy::Scanner::get_next_token()

#include "ParserDriver.hpp"
#include "parser.tab.hpp"
namespace yy {

class Scanner : yyFlexLexer {
    public:
        Scanner(ParserDriver& driver);
        Scanner(ParserDriver& driver, std::istream& in);
        virtual ~Scanner() {};
        virtual yy::Parser::symbol_type get_next_token();
    private: 
        ParserDriver& driver;
        location loc;
};

}