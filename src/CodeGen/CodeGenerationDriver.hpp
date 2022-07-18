#pragma once
#include "AST.hpp"
#include <llvm/IR/LegacyPassManager.h>
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/IR/LegacyPassManager.h"
class CodeGenerationDriver {
    private:
        std::unique_ptr<llvm::TargetMachine> target;
        std::unique_ptr<llvm::LLVMContext> context;
        std::unique_ptr<llvm::legacy::PassManager> passManager;
        std::unique_ptr<llvm::Module> moduleToCompile;
        static void initLLVM();
    public:
        CodeGenerationDriver(const AST::AST& ast); //Could take information about what optimizations to perform
        void optimize(); 
        void compile(const std::string& filename); //Could take information about compile options and target for cross compiling
        inline const llvm::Module& getModule() const {return *moduleToCompile;}

};