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

#include "LLVMEvaluator.h"

namespace SeExpr2 {
#ifdef SEEXPR_ENABLE_LLVM
bool LLVMEvaluator::prep(ExprNode* parseTree, ExprType desiredReturnType)
{
    if (_debugging) {
        parseTree->dump();
        std::cerr << "Eval strategy is llvm" << std::endl;
    }

    using namespace llvm;
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();

    std::string uniqueName = getUniqueName();

    // create Module
    _llvmContext.reset(new LLVMContext());

    std::unique_ptr<Module> TheModule(new Module(uniqueName + "_module", *_llvmContext));

    // create bindings to helper functions for variables and functions
    Function *SeExpr2LLVMEvalCustomFunctionFunc = nullptr, *SeExpr2LLVMEvalVarRefFunc = nullptr;
    {
        Type* i8PtrTy = Type::getInt8PtrTy(*_llvmContext);
        Type* i32PtrTy = Type::getInt32PtrTy(*_llvmContext);
        Type* i64Ty = Type::getInt64Ty(*_llvmContext);
        Type* doublePtrTy = Type::getDoublePtrTy(*_llvmContext);
        PointerType* i8PtrPtr = PointerType::getUnqual(i8PtrTy);
        Type* doublePtrPtrTy = PointerType::getUnqual(doublePtrTy);
        Type* ParamTys[] = {i32PtrTy, doublePtrTy, i8PtrPtr, i8PtrPtr, i64Ty, doublePtrPtrTy};
        {
            FunctionType* FT = FunctionType::get(Type::getVoidTy(*_llvmContext), ParamTys, false);
            SeExpr2LLVMEvalCustomFunctionFunc =
                Function::Create(FT, GlobalValue::ExternalLinkage, "SeExpr2LLVMEvalCustomFunction", TheModule.get());
        }
        {
            Type* ParamTys[2] = {i8PtrTy, doublePtrTy};
            FunctionType* FT = FunctionType::get(Type::getVoidTy(*_llvmContext), ParamTys, false);
            SeExpr2LLVMEvalVarRefFunc =
                Function::Create(FT, GlobalValue::ExternalLinkage, "SeExpr2LLVMEvalVarRef", TheModule.get());
        }
    }

    // create function and entry BB
    bool desireFP = desiredReturnType.isFP();
    Type* ParamTys[] = {
        desireFP ? Type::getDoublePtrTy(*_llvmContext) : PointerType::getUnqual(Type::getInt8PtrTy(*_llvmContext)),
        PointerType::get(Type::getDoublePtrTy(*_llvmContext), 0), Type::getInt32Ty(*_llvmContext),
    };
    FunctionType* FT = FunctionType::get(Type::getVoidTy(*_llvmContext), ParamTys, false);
    Function* F = Function::Create(FT, Function::ExternalLinkage, uniqueName + "_func", TheModule.get());
    F->addAttribute(llvm::AttributeSet::FunctionIndex, llvm::Attribute::AlwaysInline);
    {
        // label the function with names
        const char* names[] = {"outputPointer", "dataBlock", "indirectIndex"};
        int idx = 0;
        for (auto& arg : F->args())
            arg.setName(names[idx++]);
    }

    unsigned int dimDesired = (unsigned)desiredReturnType.dim();
    unsigned int dimGenerated = parseTree->type().dim();
    {
        BasicBlock* BB = BasicBlock::Create(*_llvmContext, "entry", F);
        IRBuilder<> Builder(BB);

        // codegen
        Value* lastVal = parseTree->codegen(Builder);

        // return values through parameter.
        Value* firstArg = &*F->arg_begin();
        if (desireFP) {
            if (dimGenerated > 1) {
                Value* newLastVal = promoteToDim(lastVal, dimDesired, Builder);
                assert(newLastVal->getType()->getVectorNumElements() >= dimDesired);
                for (unsigned i = 0; i < dimDesired; ++i) {
                    Value* idx = ConstantInt::get(Type::getInt64Ty(*_llvmContext), i);
                    Value* val = Builder.CreateExtractElement(newLastVal, idx);
                    Value* ptr = Builder.CreateInBoundsGEP(firstArg, idx);
                    Builder.CreateStore(val, ptr);
                }
            } else if (dimGenerated == 1) {
                for (unsigned i = 0; i < dimDesired; ++i) {
                    Value* ptr = Builder.CreateConstInBoundsGEP1_32(nullptr, firstArg, i);
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
    Type* LOOPParamTys[] = {
        Type::getInt8PtrTy(*_llvmContext), Type::getDoublePtrTy(*_llvmContext), Type::getInt32Ty(*_llvmContext),
        Type::getInt32Ty(*_llvmContext),
    };
    FunctionType* FTLOOP = FunctionType::get(Type::getVoidTy(*_llvmContext), LOOPParamTys, false);

    Function* FLOOP = Function::Create(FTLOOP, Function::ExternalLinkage, uniqueName + "_loopfunc", TheModule.get());
    {
        // label the function with names
        const char* names[] = {"dataBlock", "outputBasePtr", "rangeStart", "rangeEnd"};
        int idx = 0;
        for (auto& arg : FLOOP->args()) {
            arg.setName(names[idx++]);
        }
    }
    {
        // Local variables
        Value* dimValue = ConstantInt::get(Type::getInt32Ty(*_llvmContext), dimDesired);
        Value* oneValue = ConstantInt::get(Type::getInt32Ty(*_llvmContext), 1);

        // Basic blocks
        BasicBlock* entryBlock = BasicBlock::Create(*_llvmContext, "entry", FLOOP);
        BasicBlock* loopCmpBlock = BasicBlock::Create(*_llvmContext, "loopCmp", FLOOP);
        BasicBlock* loopRepeatBlock = BasicBlock::Create(*_llvmContext, "loopRepeat", FLOOP);
        BasicBlock* loopIncBlock = BasicBlock::Create(*_llvmContext, "loopInc", FLOOP);
        BasicBlock* loopEndBlock = BasicBlock::Create(*_llvmContext, "loopEnd", FLOOP);
        IRBuilder<> Builder(entryBlock);
        Builder.SetInsertPoint(entryBlock);
        Function::arg_iterator argIterator = FLOOP->arg_begin();
        // Value* outputDoublePtr=&*argIterator;++argIterator;
        Value* varBlockCharPtrPtrArg = &*argIterator;
        ++argIterator;
        Value* outputBasePtrArg = &*argIterator;
        ++argIterator;
        Value* rangeStartArg = &*argIterator;
        ++argIterator;
        Value* rangeEndArg = &*argIterator;
        ++argIterator;

        // Allocate Variables
        Value* rangeStartVar = Builder.CreateAlloca(Type::getInt32Ty(*_llvmContext), oneValue, "rangeStartVar");
        Value* rangeEndVar = Builder.CreateAlloca(Type::getInt32Ty(*_llvmContext), oneValue, "rangeEndVar");
        Value* indexVar = Builder.CreateAlloca(Type::getInt32Ty(*_llvmContext), oneValue, "indexVar");
        Type* doublePtrPtrTy = llvm::PointerType::get(Type::getDoublePtrTy(*_llvmContext), 0);
        Value* varBlockDoublePtrPtrVar = Builder.CreateAlloca(doublePtrPtrTy, oneValue, "varBlockDoublePtrPtrVar");
        // Value*
        // currentOutputVar=Builder.CreateAlloca(Type::getDoublePtrTy(*_llvmContext),oneValue,"currentOutputVar");
        // Copy variables from args
        Value* varBlockDoublePtrPtr =
            Builder.CreatePointerCast(varBlockCharPtrPtrArg, doublePtrPtrTy, "varBlockAsDoublePtrPtr");
        Builder.CreateStore(varBlockDoublePtrPtr, varBlockDoublePtrPtrVar);
        Builder.CreateStore(rangeStartArg, rangeStartVar);
        Builder.CreateStore(rangeEndArg, rangeEndVar);

        // Value*
        // outputBasePtrOffset=Builder.CreateGEP(nullptr,outputBasePtr,Builder.CreateMul(dimValue,Builder.CreateLoad(rangeStartVar)),"outputPtr");
        // Builder.CreateStore(outputBasePtrOffset,currentOutputVar);
        Builder.CreateStore(Builder.CreateLoad(rangeStartVar), indexVar);

        Builder.CreateBr(loopCmpBlock);

        Builder.SetInsertPoint(loopCmpBlock);
        Value* cond = Builder.CreateICmpULT(Builder.CreateLoad(indexVar), Builder.CreateLoad(rangeEndVar));
        Builder.CreateCondBr(cond, loopRepeatBlock, loopEndBlock);

        Builder.SetInsertPoint(loopRepeatBlock);
        Value* myOutputPtr =
            Builder.CreateGEP(nullptr, outputBasePtrArg, Builder.CreateMul(dimValue, Builder.CreateLoad(indexVar)));
        Builder.CreateCall(F, {myOutputPtr, Builder.CreateLoad(varBlockDoublePtrPtrVar), Builder.CreateLoad(indexVar)});
        // Builder.CreateStore(ConstantFP::get(Type::getDoubleTy(*_llvmContext),
        // 3.14),Builder.CreateLoad(ptrVariable));

        Builder.CreateBr(loopIncBlock);

        Builder.SetInsertPoint(loopIncBlock);
        Builder.CreateStore(Builder.CreateAdd(Builder.CreateLoad(indexVar), oneValue), indexVar);  // i++
        // Builder.CreateStore(Builder.CreateGEP(Builder.CreateLoad(currentOutputVar),dimValue),currentOutputVar);
        // // currentOutput+=dim
        Builder.CreateBr(loopCmpBlock);

        Builder.SetInsertPoint(loopEndBlock);
        Builder.CreateRetVoid();
    }

    if (_debugging) {
        std::cerr << "Pre verified LLVM byte code " << std::endl;
        TheModule->dump();
    }

    // TODO: Find out if there is a new way to veirfy
    // if (verifyModule(*TheModule)) {
    //     std::cerr << "Logic error in code generation of LLVM alert developers" << std::endl;
    //     TheModule->dump();
    // }
    Module* altModule = TheModule.get();
    std::string ErrStr;
    TheExecutionEngine.reset(EngineBuilder(std::move(TheModule))
                                 .setErrorStr(&ErrStr)
                                 //     .setUseMCJIT(true)
                                 .setOptLevel(CodeGenOpt::Aggressive)
                                 .create());

    altModule->setDataLayout(TheExecutionEngine->getDataLayout());

    // Add bindings to C linkage helper functions
    TheExecutionEngine->addGlobalMapping(SeExpr2LLVMEvalVarRefFunc, (void*)SeExpr2LLVMEvalVarRef);
    TheExecutionEngine->addGlobalMapping(SeExpr2LLVMEvalCustomFunctionFunc, (void*)SeExpr2LLVMEvalCustomFunction);

    // [verify]
    std::string errorStr;
    llvm::raw_string_ostream raw(errorStr);
    if (llvm::verifyModule(*altModule, &raw)) {
        if (_debugging) {
            std::cerr << "error in LLVM verifyModule\n";
            parseTree->dump();
            altModule->dump();
        }
        parseTree->addError(raw.str());
        return false;
    }

    // Setup optimization
    llvm::PassManagerBuilder builder;
    std::unique_ptr<llvm::legacy::PassManager> pm(new llvm::legacy::PassManager);
    std::unique_ptr<llvm::legacy::FunctionPassManager> fpm(new llvm::legacy::FunctionPassManager(altModule));
    builder.OptLevel = 3;
    builder.Inliner = llvm::createAlwaysInlinerPass();
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
    void* fp = TheExecutionEngine->getPointerToFunction(F);
    void* fpLoop = TheExecutionEngine->getPointerToFunction(FLOOP);
    if (desireFP) {
        _llvmEvalFP.reset(new LLVMEvaluationContext<double>);
        _llvmEvalFP->init(fp, fpLoop, dimDesired);
    } else {
        _llvmEvalStr.reset(new LLVMEvaluationContext<char*>);
        _llvmEvalStr->init(fp, fpLoop, dimDesired);
    }

    if (_debugging) {
        std::cerr << "Pre verified LLVM byte code " << std::endl;
        altModule->dump();
    }

    return true;
}
#endif
}
