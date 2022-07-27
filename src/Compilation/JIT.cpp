#include "JIT.hpp"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Utils.h"

#include "llvm/Support/TargetSelect.h"

static bool isLLVMInit = false;

void JIT::initLLVM() {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetAsmPrinter();
    //llvm::InitializeNativeTargetDisassembler();
}

JIT::~JIT() {
    if (auto err = es->endSession()) {
        es->reportError(std::move(err));
    }
}

llvm::Expected<std::unique_ptr<JIT>> JIT::Create() {

    if (!isLLVMInit) initLLVM();

    auto epc = llvm::orc::SelfExecutorProcessControl::Create();
    if (!epc) return epc.takeError();
    auto es = std::make_unique<llvm::orc::ExecutionSession>(std::move(*epc));

    auto jtmb = llvm::orc::JITTargetMachineBuilder(es->getExecutorProcessControl().getTargetTriple());

    auto dl = jtmb.getDefaultDataLayoutForTarget();
    if (!dl) return dl.takeError();
    
    return std::make_unique<JIT>(std::move(es), std::move(jtmb), std::move(*dl));

}

llvm::Error JIT::addModule(llvm::orc::ThreadSafeModule tsm, llvm::orc::ResourceTrackerSP rt) {
    if (!rt) {rt = mainJD.getDefaultResourceTracker();}
    return compileLayer.add(rt, std::move(tsm));
}

llvm::Expected<llvm::JITEvaluatedSymbol> JIT::lookup(llvm::StringRef name) {
    return es->lookup({&mainJD}, mangle(name.str()));
}

llvm::Expected<llvm::orc::ThreadSafeModule> JIT::optimizeModule(llvm::orc::ThreadSafeModule tsm, const llvm::orc::MaterializationResponsibility& r) {
    tsm.withModuleDo([](llvm::Module& m) {
        auto fpm = std::make_unique<llvm::legacy::FunctionPassManager>(&m);

        fpm->add(llvm::createInstructionCombiningPass());
        fpm->add(llvm::createReassociatePass());
        fpm->add(llvm::createGVNPass());
        fpm->add(llvm::createCFGSimplificationPass());
        fpm->doInitialization();

        for (auto& f : m) {
            fpm->run(f);
        }
    });

    return std::move(tsm);


}