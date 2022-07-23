#include <iostream>
#include <fstream>
#include "ParserDriver.hpp"
#include "Printer.hpp"
#include "CodeGenerator.hpp"
#include "CodeGenerationDriver.hpp"
int main(int argc, char** argv) {
    std::string file_name;
    std::ifstream file_stream;
    if (argc == 2) {
        file_name = std::string(argv[1]);
        file_stream = std::ifstream(file_name);
    } else {
        file_name = "input";
    }

    ParserDriver driver(file_name, argc == 2 ? file_stream : std::cin);
    int result = driver.parse();
    std::cout << "Parsing was " << (result == 0 ? "successful" : "unsuccessful") << "\n";
    if (result == 0) {
        Visitor::Printer printer;
        driver.getAST().accept(printer);
        try {
            CodeGenerationDriver codeDriver(driver.getAST());
            //codeDriver.optimize();
            std::cout << "Code Generated\n";
            codeDriver.getModule().print(llvm::errs(), nullptr);
            codeDriver.compile(file_name + ".o");

        } catch (...) {
            std::cerr << "There was an error compiling the code. Aborting\n";
            return 1;
        }
    } else {
        return 1;
    }

    
}

