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
    const std::vector<std::unique_ptr<AST::Function>>& get_functions() const {return functions;}
    const std::vector<std::unique_ptr<AST::Expr>>& get_top_level_expressions() const {return top_level_expressions;}
    const std::vector<std::unique_ptr<AST::Prototype>>& get_prototypes() const {return prototypes;}
private:
    yy::Parser* parser;
    yy::Scanner* scanner;

    std::string file_name;

    std::vector<std::unique_ptr<AST::Function>> functions;
    std::vector<std::unique_ptr<AST::Expr>> top_level_expressions;
    std::vector<std::unique_ptr<AST::Prototype>> prototypes;

friend class yy::Parser;
};