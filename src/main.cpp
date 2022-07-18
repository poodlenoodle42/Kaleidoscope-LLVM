#include <iostream>
#include <fstream>
#include "ParserDriver.hpp"
#include "Printer.hpp"
#include "CodeGenerator.hpp"
#include "CodeGenerationDriver.hpp"
int main() {
    std::string file_name = "test.kal";
    std::ifstream file_stream(file_name);
    ParserDriver driver(file_name, file_stream);
    int result = driver.parse();
    std::cout << "Parsing was " << (result == 0 ? "successful" : "unsuccessful") << "\n";
    if (result == 0) {
        Visitor::Printer printer;
        driver.getAST().accept(printer);
        try {
            CodeGenerationDriver codeDriver(driver.getAST());
            codeDriver.optimize();
            std::cout << "Code Generated\n";
            codeDriver.getModule().print(llvm::errs(), nullptr);
            codeDriver.compile("test.kal.o");

        } catch (...) {
            std::cerr << "There was an error compiling the code. Aborting\n";
            return 1;
        }
    }

    
}

