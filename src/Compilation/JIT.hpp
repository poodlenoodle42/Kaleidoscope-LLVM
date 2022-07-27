#pragma once

#include "llvm/ADT/StringRef.h"
#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/Core.h"
#include "llvm/ExecutionEngine/Orc/ExecutionUtils.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/Orc/IRTransformLayer.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/LLVMContext.h"
#include <memory>

class JIT {
    private:
        std::unique_ptr<llvm::orc::ExecutionSession> es;
        llvm::orc::RTDyldObjectLinkingLayer objectLayer;
        llvm::orc::IRCompileLayer compileLayer;
        llvm::orc::IRTransformLayer transformLayer;

        llvm::DataLayout dl;
        llvm::orc::MangleAndInterner mangle;
        llvm::orc::JITDylib& mainJD;

        static llvm::Expected<llvm::orc::ThreadSafeModule> optimizeModule(llvm::orc::ThreadSafeModule m, const llvm::orc::MaterializationResponsibility& r);
        static void initLLVM();

    public:
        JIT(std::unique_ptr<llvm::orc::ExecutionSession> es, llvm::orc::JITTargetMachineBuilder jtmb, llvm::DataLayout dl) 
            : es(std::move(es)), objectLayer(*this->es, [](){return std::make_unique<llvm::SectionMemoryManager>();}),
            compileLayer(*this->es, objectLayer, std::make_unique<llvm::orc::ConcurrentIRCompiler>(std::move(jtmb))),
            transformLayer(*this->es, compileLayer, optimizeModule),
            dl(std::move(dl)), mangle(*this->es, this->dl),
            mainJD(this->es->createBareJITDylib("<main>")) {
                mainJD.addGenerator(
                    llvm::cantFail(llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(dl.getGlobalPrefix()))
                );
            }

        ~JIT();

        static llvm::Expected<std::unique_ptr<JIT>> Create();
        
        const llvm::DataLayout& getDataLayout() const {return dl;}
        llvm::orc::JITDylib& getMainJITDylib() {return mainJD;}
        llvm::Error addModule(llvm::orc::ThreadSafeModule tsm, llvm::orc::ResourceTrackerSP rt = nullptr);
        llvm::Expected<llvm::JITEvaluatedSymbol> lookup(llvm::StringRef name);
};