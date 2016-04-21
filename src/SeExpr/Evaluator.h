/*
 Copyright Disney Enterprises, Inc.  All rights reserved.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License
 and the following modification to it: Section 6 Trademarks.
 deleted and replaced with:

 6. Trademarks. This License does not grant permission to use the
 trade names, trademarks, service marks, or product names of the
 Licensor and its affiliates, except as required for reproducing
 the content of the NOTICE file.

 You may obtain a copy of the License at
 http://www.apache.org/licenses/LICENSE-2.0
*/

#include "ExprConfig.h"
#include "ExprLLVMAll.h"
#include "VarBlock.h"

namespace SeExpr2 {
#ifdef SEEXPR_ENABLE_LLVM

LLVM_VALUE promoteToDim(LLVM_VALUE val, unsigned dim, llvm::IRBuilder<> &Builder);

class LLVMEvaluator {
    // TODO: this seems needlessly complex, let's fix it
    // TODO: let the dev code allocate memory?
    // FP is the native function for this expression.
    template <class T>
    class LLVMEvaluationContext {
      private:
        typedef void (*FunctionPtr)(T *);
        FunctionPtr functionPtr;
        T *resultData;

      public:
        LLVMEvaluationContext(const LLVMEvaluationContext &) = delete;
        LLVMEvaluationContext &operator=(const LLVMEvaluationContext &) = delete;

        LLVMEvaluationContext() : functionPtr(nullptr), resultData(nullptr) {}
        void init(void *fp, int dim) {
            reset();
            functionPtr = reinterpret_cast<FunctionPtr>(fp);
            resultData = new T[dim];
        }
        void reset() {
            if (resultData) delete[] resultData;
            functionPtr = nullptr;
            resultData = nullptr;
        }
        const T *operator()() {
            assert(functionPtr && resultData);
            functionPtr(resultData);
            return resultData;
        }
    };
    std::unique_ptr<LLVMEvaluationContext<double>> _llvmEvalFP;
    std::unique_ptr<LLVMEvaluationContext<char *>> _llvmEvalStr;

    std::unique_ptr<llvm::LLVMContext> _llvmContext;
    std::unique_ptr<llvm::ExecutionEngine> TheExecutionEngine;

  public:
    LLVMEvaluator() {}

    const char *evalStr() { return *(*_llvmEvalStr)(); }

    const double *evalFP() { return (*_llvmEvalFP)(); }

    void debugPrint() {
        // TheModule->dump();
    }

    void prepLLVM(ExprNode *parseTree, ExprType desiredReturnType) {
        using namespace llvm;
        InitializeNativeTarget();
        InitializeNativeTargetAsmPrinter();
        InitializeNativeTargetAsmParser();

        std::string uniqueName = getUniqueName();

        // create Module
        _llvmContext.reset(new LLVMContext());
        Module *TheModule = new Module(uniqueName + "_module", *_llvmContext);

        // Create the JIT.  This takes ownership of the module.
        std::string ErrStr;
        TheExecutionEngine.reset(EngineBuilder(TheModule)
                                     .setErrorStr(&ErrStr)
                                     .setUseMCJIT(true)
                                     .setOptLevel(CodeGenOpt::Aggressive)
                                     .create());

        if (!TheExecutionEngine) {
            fprintf(stderr, "Could not create ExecutionEngine: %s\n", ErrStr.c_str());
            exit(1);
        }

        // create function and entry BB
        bool desireFP = desiredReturnType.isFP();
        Type *ParamTys[1] = {desireFP ? Type::getDoublePtrTy(*_llvmContext)
                                      : PointerType::getUnqual(Type::getInt8PtrTy(*_llvmContext))};
        FunctionType *FT = FunctionType::get(Type::getVoidTy(*_llvmContext), ParamTys, false);
        Function *F = Function::Create(FT, Function::ExternalLinkage, uniqueName + "_func", TheModule);
        BasicBlock *BB = BasicBlock::Create(*_llvmContext, "entry", F);
        IRBuilder<> Builder(BB);

        // codegen
        Value *lastVal = parseTree->codegen(Builder);

        // return values through parameter.
        Value *firstArg = &*F->arg_begin();
        unsigned dim = (unsigned)desiredReturnType.dim();
        if (desireFP) {
            if (dim > 1) {
                Value *newLastVal = promoteToDim(lastVal, dim, Builder);
                assert(newLastVal->getType()->getVectorNumElements() == dim);
                for (unsigned i = 0; i < dim; ++i) {
                    Value *idx = ConstantInt::get(Type::getInt32Ty(*_llvmContext), i);
                    Value *val = Builder.CreateExtractElement(newLastVal, idx);
                    Value *ptr = Builder.CreateInBoundsGEP(firstArg, idx);
                    Builder.CreateStore(val, ptr);
                }
            } else if (dim == 1) {
                Value *ptr = Builder.CreateConstInBoundsGEP1_32(firstArg, 0);
                Builder.CreateStore(lastVal, ptr);
            } else {
                assert(false && "error. dim of FP is less than 1.");
            }
        } else {
            Builder.CreateStore(lastVal, firstArg);
        }

        Builder.CreateRetVoid();
        if (Expression::debugging) {
            std::cerr << "Pre verified LLVM byte code " << std::endl;
            TheModule->dump();
        }

        if (verifyModule(*TheModule)) {
            std::cerr << "Logic error in code generation of LLVM alert developers" << std::endl;
            TheModule->dump();
        }

#if 1
        llvm::FunctionPassManager *FPM = new llvm::FunctionPassManager(TheModule);

        // TheModule->setDataLayout(target_layout);
        FPM->add(new llvm::DataLayout(*TheExecutionEngine->getDataLayout()));

        FPM->add(llvm::createTypeBasedAliasAnalysisPass());
        // Provide basic AliasAnalysis support for GVN.
        FPM->add(llvm::createBasicAliasAnalysisPass());
        FPM->add(llvm::createLICMPass());
        // Promote allocas to registers.
        FPM->add(llvm::createPromoteMemoryToRegisterPass());
        // Do simple "peephole" optimizations and bit-twiddling optzns.
        FPM->add(llvm::createInstructionCombiningPass());
        // Reassociate expressions.
        FPM->add(llvm::createReassociatePass());
        // Eliminate Common SubExpressions.
        FPM->add(llvm::createGVNPass());
        // Simplify the control flow graph (deleting unreachable blocks, etc).
        FPM->add(llvm::createCFGSimplificationPass());
        FPM->doInitialization();

        // For each function in the module
        // Run the FPM on this function
        FPM->run(*F);
#endif

        TheExecutionEngine->finalizeObject();
        void *fp = TheExecutionEngine->getPointerToFunction(F);
        if (desireFP) {
            _llvmEvalFP.reset(new LLVMEvaluationContext<double>);
            _llvmEvalFP->init(fp, dim);
        } else {
            _llvmEvalStr.reset(new LLVMEvaluationContext<char *>);
            _llvmEvalStr->init(fp, dim);
        }

        if (Expression::debugging) {
            std::cerr << "Pre verified LLVM byte code " << std::endl;
            TheModule->dump();
        }
    }

    std::string getUniqueName() const {
        std::ostringstream o;
        o << std::setbase(16) << (uint64_t)(this);
        return ("_" + o.str());
    }
};

#else  // no LLVM support
class LLVMEvaluator {
  public:
    const char* evalStr() {
        assert("LLVM is not enabled in build" && false);
        return "";
    }
    const double* evalFP() {
        assert("LLVM is not enabled in build" && false);
        return 0;
    }
    void prepLLVM(ExprNode* parseTree, ExprType desiredReturnType) { assert("LLVM is not enabled in build" && false); }
    void debugPrint() {}
};
#endif

}  // end namespace SeExpr2
