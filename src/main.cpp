#include <iostream>
#include <fstream>
#include "ParserDriver.hpp"
#include "Printer.hpp"
#include "CodeGenerator.hpp"
#include "AOT.hpp"
#include "JIT.hpp"

extern "C" double printd(double d) {
    std::cout << d << "\n";
    return 0.0;
}

extern "C" double putChard(double d) {
    std::cout << (char)d;
    return 0.0;
}

void runJIT(AST::AST& ast) {
    llvm::orc::ThreadSafeContext context(std::make_unique<llvm::LLVMContext>());

    auto jit = std::move(JIT::Create());
    if (!jit) {llvm::errs() << "Error creating jit: " << jit.takeError() << "\n";}

    Visitor::CodeGenerator codeGen(*context.getContext());
    ast.accept(codeGen);
    if (auto e = (*jit)->addModule(llvm::orc::ThreadSafeModule(codeGen.consumeModule(),context))) {
        llvm::errs() << "Failed adding module to JIT: "  << e << "\n";
        return;
    }
    auto mainSymbol = (*jit)->lookup("main");
    if (!mainSymbol) {
        llvm::errs() << mainSymbol.takeError() << "\n";
        return;
    }
    auto main = (double(*)())(intptr_t)mainSymbol.get().getAddress();

    double resultOfTopLevel = main();

    std::cout << "Top Level Expression evaluated to: " << resultOfTopLevel << "\n";
}

void runAOT(AST::AST& ast ,std::string out, bool optimize, bool printModule) {
    llvm::LLVMContext context;
    Visitor::CodeGenerator codeGen(context);

    ast.accept(codeGen);
    try {
        AOT codeDriver(codeGen.consumeModule());
        if (optimize) {
            codeDriver.optimize();
        }
        if (printModule) {
            codeDriver.getModule().print(llvm::errs(), nullptr);
        }
        codeDriver.compile(out);
        
    } catch (...) {
        std::cerr << "Failed to compile\n";
        return;
    }

}


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
        //Visitor::Printer printer;
        //driver.getAST().accept(printer);
        runJIT(driver.getAST());
    } else {
        return 1;
    }

    
}

