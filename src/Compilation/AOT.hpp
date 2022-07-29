#pragma once

#include <llvm/IR/LegacyPassManager.h>
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/IR/LegacyPassManager.h"


class AOT {
    private:
        std::unique_ptr<llvm::TargetMachine> target;
        std::unique_ptr<llvm::Module> moduleToCompile;
        
        static void initLLVM();
    public:
        enum class OptLevel {
            g = 0, O1 = 1, O2 = 2, O3 = 3
        };
        AOT(std::unique_ptr<llvm::Module> moduleToCompile, std::string cpu);
        void optimize(OptLevel optLevel); 
        void compile(const std::string& filename); //Could take information about compile options and target for cross compiling
        inline const llvm::Module& getModule() const {return *moduleToCompile;}

};