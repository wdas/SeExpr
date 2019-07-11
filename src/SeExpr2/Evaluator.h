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

#ifdef SEEXPR_ENABLE_LLVM
#include <llvm/Config/llvm-config.h>
#include <llvm/Support/Compiler.h>
#endif

extern "C" void SeExpr2LLVMEvalFPVarRef(SeExpr2::ExprVarRef *seVR, double *result);
extern "C" void SeExpr2LLVMEvalStrVarRef(SeExpr2::ExprVarRef *seVR, double *result);
extern "C" void SeExpr2LLVMEvalCustomFunction(int *opDataArg,
                                              double *fpArg,
                                              char **strArg,
                                              void **funcdata,
                                              const SeExpr2::ExprFuncNode *node);

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
        typedef void (*FunctionPtr)(T *, char **, uint32_t);
        typedef void (*FunctionPtrMultiple)(char **, uint32_t, uint32_t, uint32_t);
        FunctionPtr functionPtr;
        FunctionPtrMultiple functionPtrMultiple;
        T *resultData;

      public:
        LLVMEvaluationContext(const LLVMEvaluationContext &) = delete;
        LLVMEvaluationContext &operator=(const LLVMEvaluationContext &) = delete;
        ~LLVMEvaluationContext() { delete[] resultData; }
        LLVMEvaluationContext() : functionPtr(nullptr), resultData(nullptr) {}
        void init(void *fp, void *fpLoop, int dim) {
            reset();
            functionPtr = reinterpret_cast<FunctionPtr>(fp);
            functionPtrMultiple = reinterpret_cast<FunctionPtrMultiple>(fpLoop);
            resultData = new T[dim];
        }
        void reset() {
            if (resultData) delete[] resultData;
            functionPtr = nullptr;
            resultData = nullptr;
        }
        const T *operator()(VarBlock *varBlock) {
            assert(functionPtr && resultData);
            functionPtr(resultData, varBlock ? varBlock->data() : nullptr, varBlock ? varBlock->indirectIndex : 0);
            return resultData;
        }
        void operator()(VarBlock *varBlock, size_t outputVarBlockOffset, size_t rangeStart, size_t rangeEnd) {
            assert(functionPtr && resultData);
            functionPtrMultiple(varBlock ? varBlock->data() : nullptr, outputVarBlockOffset, rangeStart, rangeEnd);
        }
    };
    std::unique_ptr<LLVMEvaluationContext<double>> _llvmEvalFP;
    std::unique_ptr<LLVMEvaluationContext<char *>> _llvmEvalStr;

    std::unique_ptr<llvm::LLVMContext> _llvmContext;
    std::unique_ptr<llvm::ExecutionEngine> TheExecutionEngine;

  public:
    LLVMEvaluator() {}

    const char *evalStr(VarBlock *varBlock) { return *(*_llvmEvalStr)(varBlock); }
    const double *evalFP(VarBlock *varBlock) { return (*_llvmEvalFP)(varBlock); }

    void evalMultiple(VarBlock *varBlock, uint32_t outputVarBlockOffset, uint32_t rangeStart, uint32_t rangeEnd) {
        return (*_llvmEvalFP)(varBlock, outputVarBlockOffset, rangeStart, rangeEnd);
    }

    void debugPrint() {
        // TheModule->print(llvm::errs(), nullptr);
    }

    bool prepLLVM(ExprNode *parseTree, ExprType desiredReturnType) {
        using namespace llvm;
        InitializeNativeTarget();
        InitializeNativeTargetAsmPrinter();
        InitializeNativeTargetAsmParser();

        std::string uniqueName = getUniqueName();

        // create Module
        _llvmContext.reset(new LLVMContext());

        std::unique_ptr<Module> TheModule(new Module(uniqueName + "_module", *_llvmContext));

        // create all needed types
        Type        *i8PtrTy        = Type::getInt8PtrTy(*_llvmContext);        // char *
        PointerType *i8PtrPtrTy     = PointerType::getUnqual(i8PtrTy);          // char **
        PointerType *i8PtrPtrPtrTy  = PointerType::getUnqual(i8PtrPtrTy);       // char ***
        Type        *i32Ty          = Type::getInt32Ty(*_llvmContext);          // int
        Type        *i32PtrTy       = Type::getInt32PtrTy(*_llvmContext);       // int *
        Type        *i64Ty          = Type::getInt64Ty(*_llvmContext);          // int64 *
        Type        *doublePtrTy    = Type::getDoublePtrTy(*_llvmContext);      // double *
        PointerType *doublePtrPtrTy = PointerType::getUnqual(doublePtrTy);      // double **
        Type        *voidTy         = Type::getVoidTy(*_llvmContext);           // void

        // create bindings to helper functions for variables and fucntions
        Function *SeExpr2LLVMEvalCustomFunctionFunc = nullptr;
        Function *SeExpr2LLVMEvalFPVarRefFunc = nullptr;
        Function *SeExpr2LLVMEvalStrVarRefFunc = nullptr;
        Function *SeExpr2LLVMEvalstrlenFunc = nullptr;
        Function *SeExpr2LLVMEvalmallocFunc = nullptr;
        Function *SeExpr2LLVMEvalfreeFunc = nullptr;
        Function *SeExpr2LLVMEvalmemsetFunc = nullptr;
        Function *SeExpr2LLVMEvalstrcatFunc = nullptr;
        {
            {
                FunctionType *FT = FunctionType::get(voidTy, {i32PtrTy, doublePtrTy, i8PtrPtrTy, i8PtrPtrTy, i64Ty}, false);
                SeExpr2LLVMEvalCustomFunctionFunc = Function::Create(FT, GlobalValue::ExternalLinkage, "SeExpr2LLVMEvalCustomFunction", TheModule.get());
            }
            {
                FunctionType *FT = FunctionType::get(voidTy, {i8PtrTy, doublePtrTy}, false);
                SeExpr2LLVMEvalFPVarRefFunc = Function::Create(FT, GlobalValue::ExternalLinkage, "SeExpr2LLVMEvalFPVarRef", TheModule.get());
            }
            {
                FunctionType *FT = FunctionType::get(voidTy, {i8PtrTy, i8PtrPtrTy}, false);
                SeExpr2LLVMEvalStrVarRefFunc = Function::Create(FT, GlobalValue::ExternalLinkage, "SeExpr2LLVMEvalStrVarRef", TheModule.get());
            }
            {
                FunctionType *FT = FunctionType::get(i32Ty, { i8PtrTy }, false);
                SeExpr2LLVMEvalstrlenFunc = Function::Create(FT, Function::ExternalLinkage, "strlen", TheModule.get());
            }
            {
                FunctionType *FT = FunctionType::get(i8PtrTy, { i32Ty }, false);
                SeExpr2LLVMEvalmallocFunc = Function::Create(FT, Function::ExternalLinkage, "malloc", TheModule.get());
            }
            {
                FunctionType *FT = FunctionType::get(voidTy, { i8PtrTy }, false);
                SeExpr2LLVMEvalfreeFunc = Function::Create(FT, Function::ExternalLinkage, "free", TheModule.get());
            }
            {
                FunctionType *FT = FunctionType::get(voidTy, { i8PtrTy, i32Ty, i32Ty }, false);
                SeExpr2LLVMEvalmemsetFunc = Function::Create(FT, Function::ExternalLinkage, "memset", TheModule.get());
            }
            {
                FunctionType *FT = FunctionType::get(i8PtrTy, { i8PtrTy, i8PtrTy }, false);
                SeExpr2LLVMEvalstrcatFunc = Function::Create(FT, Function::ExternalLinkage, "strcat", TheModule.get());
            }
        }

        // create function and entry BB
        bool desireFP = desiredReturnType.isFP();
        Type *ParamTys[] = {
            desireFP ? doublePtrTy : i8PtrPtrTy,
            doublePtrPtrTy,
            i32Ty
        };
        FunctionType *FT = FunctionType::get(voidTy, ParamTys, false);
        Function *F = Function::Create(FT, Function::ExternalLinkage, uniqueName + "_func", TheModule.get());
#if LLVM_VERSION_MAJOR > 4
        F->addAttribute(llvm::AttributeList::FunctionIndex, llvm::Attribute::AlwaysInline);
#else
        F->addAttribute(llvm::AttributeSet::FunctionIndex, llvm::Attribute::AlwaysInline);
#endif
        {
            // label the function with names
            const char *names[] = {"outputPointer", "dataBlock", "indirectIndex"};
            int idx = 0;
            for (auto &arg : F->args()) arg.setName(names[idx++]);
        }

        unsigned int dimDesired = (unsigned)desiredReturnType.dim();
        unsigned int dimGenerated = parseTree->type().dim();
        {
            BasicBlock *BB = BasicBlock::Create(*_llvmContext, "entry", F);
            IRBuilder<> Builder(BB);

            // codegen
            Value *lastVal = parseTree->codegen(Builder);

            // return values through parameter.
            Value *firstArg = &*F->arg_begin();
            if (desireFP) {
                if (dimGenerated > 1) {
                    Value *newLastVal = promoteToDim(lastVal, dimDesired, Builder);
                    assert(newLastVal->getType()->getVectorNumElements() >= dimDesired);
                    for (unsigned i = 0; i < dimDesired; ++i) {
                        Value *idx = ConstantInt::get(Type::getInt64Ty(*_llvmContext), i);
                        Value *val = Builder.CreateExtractElement(newLastVal, idx);
                        Value *ptr = Builder.CreateInBoundsGEP(firstArg, idx);
                        Builder.CreateStore(val, ptr);
                    }
                } else if (dimGenerated == 1) {
                    for (unsigned i = 0; i < dimDesired; ++i) {
                        Value *ptr = Builder.CreateConstInBoundsGEP1_32(nullptr, firstArg, i);
                        Builder.CreateStore(lastVal, ptr);
                    }
                } else {
                    assert(false && "error. dim of FP is less than 1.");
                }
            } else {
                Builder.CreateStore(lastVal, firstArg);
            }

            Builder.CreateRetVoid();
        }

        // write a new function
        FunctionType *FTLOOP = FunctionType::get(voidTy, {i8PtrTy, i32Ty, i32Ty, i32Ty}, false);
        Function *FLOOP = Function::Create(FTLOOP, Function::ExternalLinkage, uniqueName + "_loopfunc", TheModule.get());
        {
            // label the function with names
            const char *names[] = {"dataBlock", "outputVarBlockOffset", "rangeStart", "rangeEnd"};
            int idx = 0;
            for (auto &arg : FLOOP->args()) {
                arg.setName(names[idx++]);
            }
        }
        {
            // Local variables
            Value *dimValue = ConstantInt::get(i32Ty, dimDesired);
            Value *oneValue = ConstantInt::get(i32Ty, 1);

            // Basic blocks
            BasicBlock *entryBlock = BasicBlock::Create(*_llvmContext, "entry", FLOOP);
            BasicBlock *loopCmpBlock = BasicBlock::Create(*_llvmContext, "loopCmp", FLOOP);
            BasicBlock *loopRepeatBlock = BasicBlock::Create(*_llvmContext, "loopRepeat", FLOOP);
            BasicBlock *loopIncBlock = BasicBlock::Create(*_llvmContext, "loopInc", FLOOP);
            BasicBlock *loopEndBlock = BasicBlock::Create(*_llvmContext, "loopEnd", FLOOP);
            IRBuilder<> Builder(entryBlock);
            Builder.SetInsertPoint(entryBlock);

            // Get arguments
            Function::arg_iterator argIterator = FLOOP->arg_begin();
            Value *varBlockCharPtrPtrArg = &*argIterator;       ++argIterator;
            Value *outputVarBlockOffsetArg = &*argIterator;     ++argIterator;
            Value *rangeStartArg = &*argIterator;                ++argIterator;
            Value *rangeEndArg = &*argIterator;                    ++argIterator;

            // Allocate Variables
            Value *rangeStartVar = Builder.CreateAlloca(Type::getInt32Ty(*_llvmContext), oneValue, "rangeStartVar");
            Value *rangeEndVar = Builder.CreateAlloca(Type::getInt32Ty(*_llvmContext), oneValue, "rangeEndVar");
            Value *indexVar = Builder.CreateAlloca(Type::getInt32Ty(*_llvmContext), oneValue, "indexVar");
            Value *outputVarBlockOffsetVar = Builder.CreateAlloca(Type::getInt32Ty(*_llvmContext), oneValue, "outputVarBlockOffsetVar");
            Value *varBlockDoublePtrPtrVar = Builder.CreateAlloca(doublePtrPtrTy, oneValue, "varBlockDoublePtrPtrVar");
            Value *varBlockTPtrPtrVar = Builder.CreateAlloca(desireFP == true ? doublePtrPtrTy : i8PtrPtrPtrTy, oneValue, "varBlockTPtrPtrVar");

            // Copy variables from args
            Builder.CreateStore(Builder.CreatePointerCast(varBlockCharPtrPtrArg, doublePtrPtrTy, "varBlockAsDoublePtrPtr"), varBlockDoublePtrPtrVar);
            Builder.CreateStore(Builder.CreatePointerCast(varBlockCharPtrPtrArg, desireFP ? doublePtrPtrTy : i8PtrPtrPtrTy, "varBlockAsTPtrPtr"), varBlockTPtrPtrVar);
            Builder.CreateStore(rangeStartArg, rangeStartVar);
            Builder.CreateStore(rangeEndArg, rangeEndVar);
            Builder.CreateStore(outputVarBlockOffsetArg, outputVarBlockOffsetVar);

            // Set output pointer
            Value *outputBasePtrPtr = Builder.CreateGEP(nullptr, Builder.CreateLoad(varBlockTPtrPtrVar), outputVarBlockOffsetArg, "outputBasePtrPtr");
            Value *outputBasePtr = Builder.CreateLoad(outputBasePtrPtr, "outputBasePtr");
            Builder.CreateStore(Builder.CreateLoad(rangeStartVar), indexVar);

            Builder.CreateBr(loopCmpBlock);
            Builder.SetInsertPoint(loopCmpBlock);
            Value *cond = Builder.CreateICmpULT(Builder.CreateLoad(indexVar), Builder.CreateLoad(rangeEndVar));
            Builder.CreateCondBr(cond, loopRepeatBlock, loopEndBlock);

            Builder.SetInsertPoint(loopRepeatBlock);
            Value *myOutputPtr = Builder.CreateGEP(nullptr, outputBasePtr, Builder.CreateMul(dimValue, Builder.CreateLoad(indexVar)));
            Builder.CreateCall(F, {myOutputPtr, Builder.CreateLoad(varBlockDoublePtrPtrVar), Builder.CreateLoad(indexVar)});

            Builder.CreateBr(loopIncBlock);

            Builder.SetInsertPoint(loopIncBlock);
            Builder.CreateStore(Builder.CreateAdd(Builder.CreateLoad(indexVar), oneValue), indexVar);
            Builder.CreateBr(loopCmpBlock);

            Builder.SetInsertPoint(loopEndBlock);
            Builder.CreateRetVoid();
        }

        if (Expression::debugging) {
            #ifdef DEBUG
            std::cerr << "Pre verified LLVM byte code " << std::endl;
            TheModule->print(llvm::errs(), nullptr);
            #endif
        }

        // TODO: Find out if there is a new way to veirfy
        // if (verifyModule(*TheModule)) {
        //     std::cerr << "Logic error in code generation of LLVM alert developers" << std::endl;
        //     TheModule->print(llvm::errs(), nullptr);
        // }
        Module *altModule = TheModule.get();
        std::string ErrStr;
        TheExecutionEngine.reset(EngineBuilder(std::move(TheModule))
                                     .setErrorStr(&ErrStr)
                                 //     .setUseMCJIT(true)
                                     .setOptLevel(CodeGenOpt::Aggressive)
                                     .create());

        altModule->setDataLayout(TheExecutionEngine->getDataLayout());

        // Add bindings to C linkage helper functions
        TheExecutionEngine->addGlobalMapping(SeExpr2LLVMEvalFPVarRefFunc, (void *)SeExpr2LLVMEvalFPVarRef);
        TheExecutionEngine->addGlobalMapping(SeExpr2LLVMEvalStrVarRefFunc, (void *)SeExpr2LLVMEvalStrVarRef);
        TheExecutionEngine->addGlobalMapping(SeExpr2LLVMEvalCustomFunctionFunc, (void *)SeExpr2LLVMEvalCustomFunction);
        TheExecutionEngine->addGlobalMapping(SeExpr2LLVMEvalstrlenFunc, (void *)strlen);
        TheExecutionEngine->addGlobalMapping(SeExpr2LLVMEvalstrcatFunc, (void *)strcat);
        TheExecutionEngine->addGlobalMapping(SeExpr2LLVMEvalmemsetFunc, (void *)memset);
        TheExecutionEngine->addGlobalMapping(SeExpr2LLVMEvalmallocFunc, (void *)malloc);
        TheExecutionEngine->addGlobalMapping(SeExpr2LLVMEvalfreeFunc, (void *)free);

        // [verify]
        std::string errorStr;
        llvm::raw_string_ostream raw(errorStr);
        if (llvm::verifyModule(*altModule, &raw)) {
            parseTree->addError(raw.str());
            return false;
        }

        // Setup optimization
        llvm::PassManagerBuilder builder;
        std::unique_ptr<llvm::legacy::PassManager> pm(new llvm::legacy::PassManager);
        std::unique_ptr<llvm::legacy::FunctionPassManager> fpm(new llvm::legacy::FunctionPassManager(altModule));
        builder.OptLevel = 3;
#if (LLVM_VERSION_MAJOR >= 4)
        builder.Inliner = llvm::createAlwaysInlinerLegacyPass();
#else
        builder.Inliner = llvm::createAlwaysInlinerPass();
#endif
        builder.populateModulePassManager(*pm);
        // fpm->add(new llvm::DataLayoutPass());
        builder.populateFunctionPassManager(*fpm);
        fpm->run(*F);
        fpm->run(*FLOOP);
        pm->run(*altModule);

        // Create the JIT.  This takes ownership of the module.

        if (!TheExecutionEngine) {
            fprintf(stderr, "Could not create ExecutionEngine: %s\n", ErrStr.c_str());
            exit(1);
        }

        TheExecutionEngine->finalizeObject();
        void *fp = TheExecutionEngine->getPointerToFunction(F);
        void *fpLoop = TheExecutionEngine->getPointerToFunction(FLOOP);
        if (desireFP) {
            _llvmEvalFP.reset(new LLVMEvaluationContext<double>);
            _llvmEvalFP->init(fp, fpLoop, dimDesired);
        } else {
            _llvmEvalStr.reset(new LLVMEvaluationContext<char *>);
            _llvmEvalStr->init(fp, fpLoop, dimDesired);
        }

        if (Expression::debugging) {
            #ifdef DEBUG
            std::cerr << "Pre verified LLVM byte code " << std::endl;
            altModule->print(llvm::errs(), nullptr);
            #endif
        }

        return true;
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
    void unsupported() { throw std::runtime_error("LLVM is not enabled in build"); }
    const char *evalStr(VarBlock *varBlock) {
        unsupported();
        return "";
    }
    const double *evalFP(VarBlock *varBlock) {
        unsupported();
        return 0;
    }
    bool prepLLVM(ExprNode *parseTree, ExprType desiredReturnType) {
        unsupported();
        return false;
    }
    void evalMultiple(VarBlock *varBlock, int outputVarBlockOffset, size_t rangeStart, size_t rangeEnd) {
        unsupported();
    }
    void debugPrint() {}
};
#endif

}  // end namespace SeExpr2
