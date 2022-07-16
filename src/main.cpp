#include <iostream>
#include <fstream>
#include "ParserDriver.hpp"
#include "Printer.hpp"
#include "Printer.hpp"
int main() {
    std::string file_name = "test.kal";
    std::ifstream file_stream(file_name);
    ParserDriver driver(file_name, file_stream);
    int result = driver.parse();
    Visitor::Printer printer;
    driver.get_ast_root()->accept(printer);
    std::cout << "Parsing was " << (result == 0 ? "successful" : "unsuccessful") << "\n";
}

