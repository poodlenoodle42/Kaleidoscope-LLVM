#pragma once

#include <string>
#include "Expression.hpp"
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
    std::shared_ptr<AST::Expr> get_ast_root() const {return root;}

private:
    yy::Parser* parser;
    yy::Scanner* scanner;

    std::string file_name;

    std::shared_ptr<AST::Expr> root;

friend class yy::Parser;
};