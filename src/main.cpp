#include <iostream>
#include <fstream>
#include <llvm/Support/CommandLine.h>
#include "ParserDriver.hpp"
#include "Printer.hpp"
#include "CodeGenerator.hpp"
#include "AOT.hpp"
#include "JIT.hpp"

extern "C" double printd(double d) {
    std::cout << d << "\n";
    return 0.0;
}

extern "C" double putchard(double d) {
    std::cout << (char)d;
    return 0.0;
}

llvm::cl::opt<std::string> outputFilename("o", llvm::cl::desc("Specify output filename"), llvm::cl::value_desc("filename"));
llvm::cl::opt<std::string> inputFilename(llvm::cl::Positional, llvm::cl::desc("<input file>"), llvm::cl::Required);
llvm::cl::opt<std::string> march("march", llvm::cl::desc("Set CPU microarchitecture for optimisation"), llvm::cl::init("generic"));
llvm::cl::opt<bool> printAST("print-ast", llvm::cl::desc("Print abstract syntax tree in human readable form"));
llvm::cl::opt<bool> printIR("print-ir", llvm::cl::desc("Print compiled llvm IR"));
llvm::cl::opt<bool> useJIT("jit", llvm::cl::desc("Execute the given script directly using a jit"));


llvm::cl::opt<AOT::OptLevel> optimizationLevel(llvm::cl::desc("Choose optimization level:"),
    llvm::cl::init(AOT::OptLevel::g),
    llvm::cl::values(
        llvm::cl::OptionEnumValue{"g", int(AOT::OptLevel::g), "No optimization"},
        llvm::cl::OptionEnumValue{"O1", int(AOT::OptLevel::O1), "Enable trivial optimizations"},
        llvm::cl::OptionEnumValue{"O2", int(AOT::OptLevel::O2), "Enable default optimization"},
        llvm::cl::OptionEnumValue{"O3", int(AOT::OptLevel::O3), "Enable expensive optimization like auto vectorization"}
    )
);

void runJIT(AST::AST& ast) {
    llvm::orc::ThreadSafeContext context(std::make_unique<llvm::LLVMContext>());

    auto jit = std::move(JIT::Create());
    if (!jit) {llvm::errs() << "Error creating jit: " << jit.takeError() << "\n";}

    Visitor::CodeGenerator codeGen(*context.getContext());
    ast.accept(codeGen);
    if (auto e = (*jit)->addModule(llvm::orc::ThreadSafeModule(codeGen.consumeModule(),context))) {
        llvm::errs() << "Failed adding module to JIT: "  << e << "\n";
        exit(1);
    }

    auto mainSymbol = (*jit)->lookup("main");
    if (!mainSymbol) {
        llvm::errs() << mainSymbol.takeError() << "\n";
        exit(1);
    }
    auto main = (double(*)())(intptr_t)mainSymbol.get().getAddress();

    double resultOfTopLevel = main();

    //(*jit)->getMainJITDylib().dump(llvm::outs());

    std::cout << "Top Level Expression evaluated to: " << resultOfTopLevel << "\n";
}


 
void runAOT(AST::AST& ast ,std::string out) {
    llvm::LLVMContext context;
    Visitor::CodeGenerator codeGen(context);

    ast.accept(codeGen);
    try {
        AOT codeDriver(codeGen.consumeModule(), march);
        codeDriver.optimize(optimizationLevel);
        if (printIR) {
            codeDriver.getModule().print(llvm::errs(), nullptr);
        }
        codeDriver.compile(out);
        
    } catch (...) {
        std::cerr << "Failed to compile\n";
        exit(1);
    }

}





int main(int argc, char** argv) {
    llvm::cl::ParseCommandLineOptions(argc, argv);
    std::ifstream file_stream(inputFilename);


    ParserDriver driver(inputFilename,(inputFilename.empty() ? std::cin : file_stream));
    int result = driver.parse();

    if (result == 0) {
        if (printAST) {
            Visitor::Printer printer;
            driver.getAST().accept(printer);
        }
        if (useJIT) {
            runJIT(driver.getAST());
        } else {
            runAOT(driver.getAST(), (outputFilename.empty() ? std::string("out.o") : outputFilename));
        }
        
    } else {
        return 1;
    }

    
}

