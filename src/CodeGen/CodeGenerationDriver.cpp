#include <iostream>
#include "CodeGenerationDriver.hpp"
#include "CodeGenerator.hpp"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"

#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"

static bool isLLVMinit = false;

void CodeGenerationDriver::initLLVM() {
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();
}

CodeGenerationDriver::CodeGenerationDriver(const AST::AST& ast) {
    context = std::make_unique<llvm::LLVMContext>();
    auto codeGen = Visitor::CodeGenerator(*context);
    const_cast<AST::AST&>(ast).accept(codeGen);
    if (codeGen.hadError()) {
        throw;
    }
    moduleToCompile = codeGen.consumeModule();
    passManager = std::make_unique<llvm::legacy::PassManager>();
    passManager->add(llvm::createInstructionCombiningPass());
    passManager->add(llvm::createReassociatePass());
    passManager->add(llvm::createGVNPass());
    passManager->add(llvm::createCFGSimplificationPass());

    if(!isLLVMinit) {
        initLLVM();
        isLLVMinit = true;
    }

    auto targetTripple = llvm::sys::getDefaultTargetTriple();
    std::string error;
    auto t = llvm::TargetRegistry::lookupTarget(targetTripple,error);
    if(!t) {std::cerr << error; throw;}


    auto cpu = "generic";
    auto features = "";

    llvm::TargetOptions opt;
    auto RM = llvm::Optional<llvm::Reloc::Model>();
    target = std::unique_ptr<llvm::TargetMachine>(t->createTargetMachine(targetTripple, cpu, features, opt, RM));

    moduleToCompile->setDataLayout(target->createDataLayout());
    moduleToCompile->setTargetTriple(targetTripple);
}

void CodeGenerationDriver::optimize() {
    passManager->run(*moduleToCompile);
}

void CodeGenerationDriver::compile(const std::string& filename) {
    std::error_code ec;
    llvm::raw_fd_ostream dest(filename, ec, llvm::sys::fs::OF_None);
    if (ec) {std::cerr << "Could not open file: " << ec.message(); throw;}

    llvm::legacy::PassManager pass;
    auto fileType = llvm::CGFT_ObjectFile;

    if(target->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
        std::cerr << "TargetMachine can't emit a file of this type"; throw;
    }

    pass.run(*moduleToCompile);
    dest.flush();

}

