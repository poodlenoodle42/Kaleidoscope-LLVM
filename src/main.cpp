#include <iostream>
#include <fstream>
#include "ParserDriver.hpp"
#include "Printer.hpp"
#include "CodeGenerator.hpp"

int main() {
    std::string file_name = "test.kal";
    std::ifstream file_stream(file_name);
    ParserDriver driver(file_name, file_stream);
    int result = driver.parse();
    if (result == 0) {
        Visitor::Printer printer;
        driver.getAST().accept(printer);
    }

    std::cout << "Parsing was " << (result == 0 ? "successful" : "unsuccessful") << "\n";
}

