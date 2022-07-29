#include <iostream>
#include "AOT.hpp"
#include "CodeGenerator.hpp"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Utils.h"
#include "llvm/Passes/PassBuilder.h"

#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"

static bool isLLVMinit = false;

void AOT::initLLVM() {
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();
}

AOT::AOT(std::unique_ptr<llvm::Module> module, std::string cpu) : moduleToCompile(std::move(module)) {

    if(!isLLVMinit) {
        initLLVM();
        isLLVMinit = true;
    }

    auto targetTripple = llvm::sys::getDefaultTargetTriple();
    std::string error;
    auto t = llvm::TargetRegistry::lookupTarget(targetTripple,error);
    if(!t) {std::cerr << error; throw;}


    auto features = "";

    llvm::TargetOptions opt;
    auto RM = llvm::Optional<llvm::Reloc::Model>();
    target = std::unique_ptr<llvm::TargetMachine>(t->createTargetMachine(targetTripple, cpu, features, opt, RM));

    moduleToCompile->setDataLayout(target->createDataLayout());
    moduleToCompile->setTargetTriple(targetTripple);
}

void AOT::optimize(AOT::OptLevel optLevel) {
    llvm::ModuleAnalysisManager mam;
    llvm::LoopAnalysisManager lam;
    llvm::FunctionAnalysisManager fam;
    llvm::CGSCCAnalysisManager cam;
    llvm::PassBuilder passBuilder;
 
    passBuilder.registerModuleAnalyses(mam);
    passBuilder.registerCGSCCAnalyses(cam);
    passBuilder.registerFunctionAnalyses(fam);
    passBuilder.registerLoopAnalyses(lam);
    passBuilder.crossRegisterProxies(lam, fam, cam, mam);
    llvm::ModulePassManager modPassManager;
    if (optLevel == AOT::OptLevel::g) {
        mam.clear();
        return;
    } else if (optLevel == AOT::OptLevel::O1) {
        modPassManager = passBuilder.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O1);
    } else if (optLevel == AOT::OptLevel::O2) {
        modPassManager = passBuilder.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O2);
    } else if (optLevel == AOT::OptLevel::O3) {
        modPassManager = passBuilder.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O3);
    }
    //modPassManager.printPipeline(llvm::outs(),[](llvm::StringRef ref) {return ref;});
    modPassManager.run(*moduleToCompile, mam);
    mam.clear();
}

void AOT::compile(const std::string& filename) {
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

