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
    if (result == 0) {
        Visitor::Printer printer;
        std::cout << "\nFunctions:\n";
        for(const auto& func : driver.get_functions()) {
            func->accept(printer);
        }
        std::cout << "\nPrototypes\n";
        for(const auto& prot : driver.get_prototypes()) {
            prot->accept(printer);
        }
        std::cout << "\nTop Level Expressions\n";
        for(const auto& expr : driver.get_top_level_expressions()) {
            expr->accept(printer);
        }
    }

    std::cout << "Parsing was " << (result == 0 ? "successful" : "unsuccessful") << "\n";
}

