#pragma once

#include <string>



class ParserDriver {
public:
    ParserDriver();
    int parse(const std::string& f);

    std::string file;

private:
    //yy::Parser parser;
    //yy::Scanner scanner;

};