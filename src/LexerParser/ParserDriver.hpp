#pragma once

#include <string>
#include "AST.hpp"
namespace yy {
    class Parser;
    class Scanner;
}
class ParserDriver {
public:
    ParserDriver(std::string file_name,std::istream& input_stream);
    ParserDriver();
    ~ParserDriver();

    int parse();
    const std::string& get_file_name() const {return file_name;}
    AST::AST& getAST() {return tree;}
private:
    yy::Parser* parser;
    yy::Scanner* scanner;

    std::string file_name;

    AST::AST tree;

friend class yy::Parser;
};