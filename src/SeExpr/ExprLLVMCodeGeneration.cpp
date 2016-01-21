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

#include "ExprNode.h"
#include "ExprFunc.h"
#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Intrinsics.h>
#include <llvm/IR/TypeBuilder.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <llvm/Transforms/Utils/Cloning.h>
using namespace llvm;
using namespace SeExpr2;

// TODO: Use ordered or unordered float comparison?
// TODO: factor out commonly used llvm types
// TODO: factor out integer/double constant creation
namespace {

Function* llvm_getFunction(LLVM_BUILDER Builder) {
    return Builder.GetInsertBlock()->getParent();
}

Module* llvm_getModule(LLVM_BUILDER Builder) {
    return llvm_getFunction(Builder)->getParent();
}

bool isVarArg(ExprFuncStandard::FuncType seFuncType) {
    if(seFuncType == ExprFuncStandard::FUNCN  ||
       seFuncType == ExprFuncStandard::FUNCNV ||
       seFuncType == ExprFuncStandard::FUNCNVV)
        return true;
    return false;
}

bool isReturnVector(ExprFuncStandard::FuncType seFuncType) {
    if(seFuncType == ExprFuncStandard::FUNC1VV ||
       seFuncType == ExprFuncStandard::FUNC2VV ||
       seFuncType == ExprFuncStandard::FUNCNVV)
        return true;
    return false;
}

bool isTakeOnlyDoubleArg(ExprFuncStandard::FuncType seFuncType) {
    if(seFuncType <= ExprFuncStandard::FUNC6 ||
       seFuncType == ExprFuncStandard::FUNCN)
        return true;
    return false;
}

FunctionType* getSeExprFuncStandardLLVMType(ExprFuncStandard::FuncType sft,
                                           LLVMContext &llvmContext) {
    assert(sft != ExprFuncStandard::NONE);

    Type *intType = TypeBuilder<int,false>::get(llvmContext);
    Type *doubleType = TypeBuilder<double,false>::get(llvmContext);
    Type *doublePtrType = TypeBuilder<double*,false>::get(llvmContext);
    Type *voidType = TypeBuilder<void,false>::get(llvmContext);
    FunctionType *FT = 0;

    if(sft <= ExprFuncStandard::FUNC6) {
        std::vector<Type*> paramTypes;
        switch(sft) {
        case ExprFuncStandard::FUNC6: paramTypes.push_back(doubleType);
        case ExprFuncStandard::FUNC5: paramTypes.push_back(doubleType);
        case ExprFuncStandard::FUNC4: paramTypes.push_back(doubleType);
        case ExprFuncStandard::FUNC3: paramTypes.push_back(doubleType);
        case ExprFuncStandard::FUNC2: paramTypes.push_back(doubleType);
        case ExprFuncStandard::FUNC1: paramTypes.push_back(doubleType);
        case ExprFuncStandard::FUNC0:
        default:
                   FT = FunctionType::get(doubleType, paramTypes, false);
        }
    } else if(sft == ExprFuncStandard::FUNC1V) {
        Type *paramTypes[1] = {doublePtrType};
        FT =  FunctionType::get(doubleType, paramTypes, false);
    } else if(sft == ExprFuncStandard::FUNC2V) {
        Type *paramTypes[2] = {doublePtrType, doublePtrType};
        FT =  FunctionType::get(doubleType, paramTypes, false);
    } else if(sft == ExprFuncStandard::FUNC1VV) {
        Type *paramTypes[2] = {doublePtrType, doublePtrType};
        FT =  FunctionType::get(voidType, paramTypes, false);
    } else if(sft == ExprFuncStandard::FUNC2VV) {
        Type *paramTypes[3] = {doublePtrType, doublePtrType, doublePtrType};
        FT =  FunctionType::get(voidType, paramTypes, false);
    } else if(sft == ExprFuncStandard::FUNCN) {
        Type *paramTypes[2] = {intType, doublePtrType};
        FT =  FunctionType::get(doubleType, paramTypes, false);
    } else if(sft == ExprFuncStandard::FUNCNV) {
        Type *paramTypes[2] = {intType, doublePtrType};
        FT =  FunctionType::get(doubleType, paramTypes, false);
    } else if(sft == ExprFuncStandard::FUNCNVV) {
        Type *paramTypes[3] = {doublePtrType, intType, doublePtrType};
        FT =  FunctionType::get(voidType, paramTypes, false);
    } else assert(false);

    return FT;
}

Type *createLLVMTyForSeExprType(LLVMContext &llvmContext, ExprType seType) {
    if(seType.isFP()) {
        int dim = seType.dim();
        return dim == 1 ? Type::getDoubleTy(llvmContext) : VectorType::get(Type::getDoubleTy(llvmContext), dim);
    } else if(seType.isString()) {
        // TODO: post c++11
        // static_assert(sizeof(char*) == 8, "Expect 64-bit pointers");
        return Type::getInt8PtrTy(llvmContext);
    }
    assert(!"unknown SeExpr type encountered"); // unknown type 
    return 0;
}

// Copy a scalar "val" to a vector of "dim" length
LLVM_VALUE createVecVal(LLVM_BUILDER Builder, LLVM_VALUE val, unsigned dim) {
    LLVMContext &llvmContext = Builder.getContext();
    VectorType *doubleVecTy = VectorType::get(Type::getDoubleTy(llvmContext), dim);
    LLVM_VALUE vecVal = UndefValue::get(doubleVecTy);
    for (unsigned i = 0; i < dim; i++)
        vecVal = Builder.CreateInsertElement(vecVal, val, ConstantInt::get(Type::getInt32Ty(llvmContext), i));
    return vecVal;
}

// Copy a vector "val" to a vector of the same length
LLVM_VALUE createVecVal(LLVM_BUILDER Builder, ArrayRef<LLVM_VALUE > val) {
    if(!val.size())
        return 0;

    LLVMContext &llvmContext = Builder.getContext();
    unsigned dim = val.size();
    VectorType *elemType = VectorType::get(val[0]->getType(), dim);
    LLVM_VALUE vecVal = UndefValue::get(elemType);
    for (unsigned i = 0; i < dim; i++)
        vecVal = Builder.CreateInsertElement(vecVal, val[i], ConstantInt::get(Type::getInt32Ty(llvmContext), i));
    return vecVal;
}

LLVM_VALUE createVecValFromAlloca(LLVM_BUILDER Builder, AllocaInst *destPtr, unsigned vecLen) {
    Type *destTy = destPtr->getType()->getPointerElementType();
    assert(destTy->isDoubleTy() || destTy->isArrayTy());
    std::vector<LLVM_VALUE > vals;

        for(unsigned i = 0; i < vecLen; ++i) {
            LLVM_VALUE ptr = destTy->isDoubleTy()?
                         Builder.CreateConstGEP1_32(destPtr, i):
                         Builder.CreateConstGEP2_32(destPtr, 0, i);
            vals.push_back(Builder.CreateLoad(ptr));
        }

    return createVecVal(Builder, vals);
}

LLVM_VALUE getFirstElement(LLVM_VALUE V, IRBuilder<> Builder) {
    Type *VTy = V->getType();
    if(VTy->isDoubleTy())
        return V;

    assert(VTy->isVectorTy());
    LLVMContext &llvmContext = Builder.getContext();
    LLVM_VALUE zero = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
    return Builder.CreateExtractElement(V, zero);
}

LLVM_VALUE promoteToTy(LLVM_VALUE val, Type *destTy, LLVM_BUILDER Builder) {
    Type *srcTy = val->getType();
    if(srcTy == destTy)
        return val;

    if(destTy->isDoubleTy())
        return val;

    return createVecVal(Builder, val, destTy->getVectorNumElements());
}

AllocaInst *createAllocaInst(LLVM_BUILDER Builder, Type *ty,
                             unsigned arraySize = 1,
                             const std::string &varName = "") {
    // move builder to first position of entry BB
    BasicBlock *entryBB = &llvm_getFunction(Builder)->getEntryBlock();
    IRBuilder<>::InsertPoint oldIP = Builder.saveIP();
    if(entryBB->size())
        Builder.SetInsertPoint(&entryBB->front());
    else
        Builder.SetInsertPoint(entryBB);

    // allocate stack memory and store value to it.
    LLVMContext &llvmContext = Builder.getContext();
    LLVM_VALUE arraySizeVal = ConstantInt::get(Type::getInt32Ty(llvmContext), arraySize);
    AllocaInst *varPtr = Builder.CreateAlloca(ty, arraySizeVal, varName);
    // restore builder insertion position
    Builder.restoreIP(oldIP);
    return varPtr;
}

AllocaInst *createArray(LLVM_BUILDER Builder, Type *ty,
                             unsigned arraySize,
                             const std::string &varName = "") {
    // move builder to first position of entry BB
    BasicBlock *entryBB = &llvm_getFunction(Builder)->getEntryBlock();
    IRBuilder<>::InsertPoint oldIP = Builder.saveIP();
    if(entryBB->size())
        Builder.SetInsertPoint(&entryBB->front());
    else
        Builder.SetInsertPoint(entryBB);

    // allocate stack memory and store value to it.
    ArrayType *arrayTy = ArrayType::get(ty, arraySize);
    AllocaInst *varPtr = Builder.CreateAlloca(arrayTy, 0, varName);
    // restore builder insertion position
    Builder.restoreIP(oldIP);
    return varPtr;
}

std::pair<LLVM_VALUE, LLVM_VALUE > promoteBinaryOperandsToAppropriateVector(LLVM_BUILDER Builder,
                                              LLVM_VALUE op1, LLVM_VALUE op2) {
    Type *op1Ty = op1->getType();
    Type *op2Ty = op2->getType();
    if(op1Ty == op2Ty)
        return std::make_pair(op1,op2);

    LLVM_VALUE toPromote = op1;
    LLVM_VALUE target = op2;
    if(op1Ty->isVectorTy())
        std::swap(toPromote, target);

    assert(target->getType()->isVectorTy());

    unsigned dim = target->getType()->getVectorNumElements();
    LLVM_VALUE vecVal = createVecVal(Builder, toPromote, dim);

    if(op1Ty->isVectorTy())
        op2 = vecVal;
    else
        op1 = vecVal;

    return std::make_pair(op1,op2);
}

Function *getOrCreateEvalVarDeclaration(LLVM_BUILDER Builder) {
    LLVMContext &llvmContext = Builder.getContext();
    Type *i8PtrTy = Type::getInt8PtrTy(llvmContext);
    Type *doublePtrTy = Type::getDoublePtrTy(llvmContext);
    Type *ParamTys[2] = {i8PtrTy, doublePtrTy};
    FunctionType *FT = FunctionType::get(Type::getVoidTy(llvmContext), ParamTys, false);

    Module *M = llvm_getModule(Builder);
    Function *callback = M->getFunction("evaluateVarRef");
    if(callback)
        return callback;

    return Function::Create(FT, GlobalValue::ExternalLinkage, "evaluateVarRef", M);
}

Function *getOrCreateCustomFunctionWrapperDeclaration(LLVM_BUILDER Builder) {
    LLVMContext &llvmContext = Builder.getContext();
    Type *i8PtrTy = Type::getInt8PtrTy(llvmContext);
    Type *i32PtrTy = Type::getInt32PtrTy(llvmContext);
    Type *i32Ty = Type::getInt32Ty(llvmContext);
    Type *doublePtrTy = Type::getDoublePtrTy(llvmContext);
    PointerType *i8PtrPtr = PointerType::getUnqual(i8PtrTy);
    Type *ParamTys[10] = {i8PtrTy, i32PtrTy, i32Ty,
                          doublePtrTy, i32Ty, //fp
                          i8PtrPtr, i32Ty,    //str
                          i8PtrPtr, doublePtrTy, i32Ty};
    FunctionType *FT = FunctionType::get(Type::getVoidTy(llvmContext), ParamTys, false);

    Module *M = llvm_getModule(Builder);
    Function *callback = M->getFunction("resolveCustomFunction");
    if(callback)
        return callback;

    return Function::Create(FT, GlobalValue::ExternalLinkage, "resolveCustomFunction", M);
}

LLVM_VALUE resolveLocalVar(const char *name, LLVM_BUILDER Builder) {
    // TODO: can we just remember the llvm value so we don't have to go searching for this?
    BasicBlock &entryBB = llvm_getFunction(Builder)->getEntryBlock();
    for(BasicBlock::iterator BI = entryBB.begin(),
                             BE = entryBB.end(); BI != BE; ++BI)
        if (AllocaInst *AI = dyn_cast<AllocaInst>(BI))
            if(AI->getName() == name)
                return BI;

    // TODO: use this when adopting c++11
    //return nullptr;
    return 0;
}

AllocaInst *storeVectorToArrayPtr(LLVM_BUILDER Builder, LLVM_VALUE vecVal) {
    LLVMContext &llvmContext = Builder.getContext();
    AllocaInst *arrayPtr = createArray(Builder, Type::getDoubleTy(llvmContext),
                                       vecVal->getType()->getVectorNumElements());

    for(unsigned i = 0; i < 3; ++i) {
        LLVM_VALUE idx = ConstantInt::get(Type::getInt32Ty(llvmContext), i);
        LLVM_VALUE val = Builder.CreateExtractElement(vecVal, idx);
        LLVM_VALUE ptr = Builder.CreateConstGEP2_32(arrayPtr, 0, i);
        Builder.CreateStore(val, ptr);
    }
    return arrayPtr;
}

void storeVectorToArrayPtr(LLVM_BUILDER Builder, LLVM_VALUE vecVal, LLVM_VALUE arrayPtr) {
    LLVMContext &llvmContext = Builder.getContext();
    for(unsigned i = 0; i < 3; ++i) {
        LLVM_VALUE idx = ConstantInt::get(Type::getInt32Ty(llvmContext), i);
        LLVM_VALUE val = Builder.CreateExtractElement(vecVal, idx);
        LLVM_VALUE ptr = Builder.CreateConstGEP2_32(arrayPtr, 0, i);
        Builder.CreateStore(val, ptr);
    }
}

AllocaInst *storeVectorToDoublePtr(LLVM_BUILDER Builder, LLVM_VALUE vecVal) {
    LLVMContext &llvmContext = Builder.getContext();
    AllocaInst *doublePtr = createAllocaInst(Builder, Type::getDoubleTy(llvmContext),
                                   vecVal->getType()->getVectorNumElements());
    for(unsigned i = 0; i < 3; ++i) {
        LLVM_VALUE idx = ConstantInt::get(Type::getInt32Ty(llvmContext), i);
        LLVM_VALUE val = Builder.CreateExtractElement(vecVal, idx);
        LLVM_VALUE ptr = Builder.CreateConstGEP1_32(doublePtr, i);
        Builder.CreateStore(val, ptr);
    }
    return doublePtr;
}

std::vector<LLVM_VALUE > codegenFuncCallArgs(LLVM_BUILDER Builder,
                                        const ExprFuncNode* funcNode) {
    std::vector<LLVM_VALUE > args;
    for(int i = 0; i < funcNode->numChildren(); ++i)
        args.push_back(funcNode->child(i)->codegen(Builder));
    return args;
}

std::vector<LLVM_VALUE > promoteArgs(std::vector<LLVM_VALUE > args, LLVM_BUILDER Builder,
                           FunctionType *llvmFuncType) {
    std::vector<LLVM_VALUE > ret;
    for(unsigned i = 0; i < args.size(); ++i)
        ret.push_back(promoteToTy(args[i], llvmFuncType->getParamType(i), Builder));
    return ret;
}

std::vector<LLVM_VALUE > promoteArgs(std::vector<LLVM_VALUE > args, LLVM_BUILDER Builder,
                                ExprFuncStandard::FuncType seFuncType) {
    if(isTakeOnlyDoubleArg(seFuncType))
        return args;

    LLVMContext &llvmContext = Builder.getContext();
    VectorType *destTy = VectorType::get(Type::getDoubleTy(llvmContext), 3);
    std::vector<LLVM_VALUE > ret;
    for(unsigned i = 0; i < args.size(); ++i)
        ret.push_back(promoteToTy(args[i], destTy, Builder));
    return ret;
}

std::vector<LLVM_VALUE > replaceVecArgWithDoublePointer(LLVM_BUILDER Builder,
        std::vector<LLVM_VALUE > args) {
    for(unsigned i = 0; i < args.size(); ++i)
        if(args[i]->getType()->isVectorTy())
            args[i] = storeVectorToDoublePtr(Builder, args[i]);
    return args;
}

std::vector<LLVM_VALUE > convertArgsToPointerAndLength(LLVM_BUILDER Builder,
                                            std::vector<LLVM_VALUE > actualArgs,
                                            ExprFuncStandard::FuncType seFuncType) {
    assert(isVarArg(seFuncType));

    LLVMContext &llvmContext = Builder.getContext();
    unsigned numArgs = actualArgs.size();

    // type of arg should be either double or double*(aka. vector).
    for(unsigned i = 0; i < numArgs; ++i)
        assert(actualArgs[i]->getType()->isDoubleTy() ||
               actualArgs[i]->getType() == Type::getDoublePtrTy(llvmContext));

    std::vector<LLVM_VALUE > args;
    // push "int n"
    args.push_back(ConstantInt::get(Type::getInt32Ty(llvmContext), numArgs));

    if(seFuncType == ExprFuncStandard::FUNCN) {
        AllocaInst *doublePtr = createAllocaInst(Builder, Type::getDoubleTy(llvmContext), numArgs);
        for(unsigned i = 0; i < numArgs; ++i) {
            LLVM_VALUE ptr = Builder.CreateConstGEP1_32(doublePtr, i);
            Builder.CreateStore(actualArgs[i], ptr);
        }
        args.push_back(doublePtr);
        return args;
    }

    AllocaInst *arrayPtr = createArray(Builder, ArrayType::get(Type::getDoubleTy(llvmContext), 3), numArgs);
    for(unsigned i = 0; i < numArgs; ++i) {
        LLVM_VALUE toInsert = actualArgs[i];
        LLVM_VALUE subArrayPtr = Builder.CreateConstGEP2_32(arrayPtr, 0, i);
        for(unsigned j = 0; j < 3; ++j) {
            LLVM_VALUE destAddr = Builder.CreateConstGEP2_32(subArrayPtr, 0, j);
            LLVM_VALUE srcAddr = Builder.CreateConstGEP1_32(toInsert, j);
            Builder.CreateStore(Builder.CreateLoad(srcAddr), destAddr);
        }
    }
    args.push_back(Builder.CreateBitCast(arrayPtr, Type::getDoublePtrTy(llvmContext)));
    return args;
}

LLVM_VALUE executeStandardFunction(LLVM_BUILDER Builder, ExprFuncStandard::FuncType seFuncType,
         std::vector<LLVM_VALUE > args, LLVM_VALUE addrVal) {
    LLVMContext &llvmContext = Builder.getContext();

    args = promoteArgs(args, Builder, seFuncType);
    args = replaceVecArgWithDoublePointer(Builder, args);

    if(isVarArg(seFuncType))
        args = convertArgsToPointerAndLength(Builder, args, seFuncType);

    if(isReturnVector(seFuncType) == false)
        return Builder.CreateCall(addrVal, args);

    // TODO: assume standard function all use vector of length 3 as parameter
    //       or return type.
    AllocaInst *retPtr = createAllocaInst(Builder, Type::getDoubleTy(llvmContext), 3);
    args.insert(args.begin(), retPtr);
    Builder.CreateCall(addrVal, replaceVecArgWithDoublePointer(Builder, args));
    return createVecValFromAlloca(Builder, retPtr, 3);
}

// TODO: Is this necessary? why not use printf custom function?
LLVM_VALUE callPrintf(const ExprFuncNode *seFunc, LLVM_BUILDER Builder, Function *callee) {
    LLVMContext &llvmContext = Builder.getContext();
    std::vector<LLVM_VALUE > args;

    // TODO: promotion for printf?
    { // preprocess format string.
        const ExprStrNode* formatStrNode = dynamic_cast<const ExprStrNode*>(seFunc->child(0));
        assert(formatStrNode);
        std::string formatStr(formatStrNode->str());
        std::string::size_type pos = std::string::npos;
        while((pos = formatStr.find("%v")) != std::string::npos)
            formatStr.replace(pos, 2, std::string("[%f,%f,%f]"));
        formatStr.append("\n");
        args.push_back(Builder.CreateGlobalStringPtr(formatStr));
    }

    for(int i = 1; i < seFunc->numChildren(); ++i) {
        LLVM_VALUE arg = seFunc->child(i)->codegen(Builder);
        if(arg->getType()->isVectorTy()) {
            AllocaInst *vecArray = storeVectorToDoublePtr(Builder, arg);
            for(unsigned i=0; i<arg->getType()->getVectorNumElements(); ++i) {
                LLVM_VALUE elemPtr = Builder.CreateConstGEP1_32(vecArray, i);
                args.push_back(Builder.CreateLoad(elemPtr));
            }
        } else
            args.push_back(arg);
    }

    Builder.CreateCall(callee, args);
    return ConstantFP::get(Type::getDoubleTy(llvmContext), 0.0);
}

// TODO: not good. need better implementation.
LLVM_VALUE callCustomFunction(const ExprFuncNode *funcNode, LLVM_BUILDER Builder) {
    LLVMContext &llvmContext = Builder.getContext();
    int nargs = funcNode->numChildren();
    std::vector<LLVM_VALUE > args = codegenFuncCallArgs(Builder,funcNode);

    unsigned sizeOfRet = (unsigned)funcNode->type().dim();
    assert(sizeOfRet == 1 || sizeOfRet == 3);
    AllocaInst *result = createAllocaInst(Builder, Type::getDoubleTy(llvmContext), sizeOfRet);
    LLVM_VALUE varName = Builder.CreateGlobalString(funcNode->name());
    LLVM_VALUE ptrToFirstChar = Builder.CreateConstGEP2_32(varName, 0, 0);

    unsigned sizeOfFpArgs = 0;
    unsigned sizeOfStrArgs = 0;
    for(int i=0; i < nargs; ++i) {
        Type *argType = args[i]->getType();
        if(argType->isDoubleTy())
            sizeOfFpArgs += 1;
        else if (argType->isVectorTy()) {
            assert(argType->getVectorNumElements() == 3);
            sizeOfFpArgs += 3;
        } else if (argType == Type::getInt8PtrTy(llvmContext)) {
            sizeOfStrArgs += 1;
        } else assert(false);
    }

    // allocate opData
    AllocaInst *opDataArg = createAllocaInst(Builder, Type::getInt32Ty(llvmContext), (unsigned)nargs);
    AllocaInst *fpArg = createAllocaInst(Builder, Type::getDoubleTy(llvmContext), sizeOfFpArgs);
    AllocaInst *strArg = createAllocaInst(Builder, Type::getInt8PtrTy(llvmContext), sizeOfStrArgs);
    unsigned fpIdx = 0;
    unsigned strIdx = 0;
    for(int i=0; i < nargs; ++i) {
        Type *argType = args[i]->getType();

        unsigned opIdx = 0;
        if(argType->isDoubleTy() || argType->isVectorTy()) {
            opIdx = fpIdx + 1 + sizeOfRet;
        } else if (argType == Type::getInt8PtrTy(llvmContext)) {
            opIdx = strIdx + 2;
        } else assert(false);
        LLVM_VALUE idxVal = ConstantInt::get(Type::getInt32Ty(llvmContext), opIdx);
        LLVM_VALUE opDataPtr = Builder.CreateConstGEP1_32(opDataArg, i);
        Builder.CreateStore(idxVal, opDataPtr);

        if(argType->isDoubleTy()) {
            LLVM_VALUE fpArgPtr = Builder.CreateConstGEP1_32(fpArg, fpIdx++);
            Builder.CreateStore(args[i], fpArgPtr);
        } else if (argType->isVectorTy()) {
            for(unsigned j = 0; j < 3; ++j) {
                LLVM_VALUE vecIdx = ConstantInt::get(Type::getInt32Ty(llvmContext), j);
                LLVM_VALUE val = Builder.CreateExtractElement(args[i], vecIdx);
                LLVM_VALUE fpArgPtr = Builder.CreateConstGEP1_32(fpArg, fpIdx++);
                Builder.CreateStore(val, fpArgPtr);
            }
        } else if (argType == Type::getInt8PtrTy(llvmContext)) {
            LLVM_VALUE strArgPtr = Builder.CreateConstGEP1_32(strArg, strIdx++);
            Builder.CreateStore(args[i], strArgPtr);
        } else assert(false);
    }

    // TODO: thread safety?
    ConstantPointerNull *nullPtrVal =
                          ConstantPointerNull::get(Type::getInt8PtrTy(llvmContext));
    Module *M = llvm_getModule(Builder);
    GlobalVariable *dataGV  = new GlobalVariable(*M, Type::getInt8PtrTy(llvmContext), false,
                                      GlobalValue::InternalLinkage, nullPtrVal);

    std::vector<LLVM_VALUE > params;
    params.push_back(ptrToFirstChar);
    params.push_back(opDataArg);
    params.push_back(ConstantInt::get(Type::getInt32Ty(llvmContext), nargs));
    params.push_back(fpArg);
    params.push_back(ConstantInt::get(Type::getInt32Ty(llvmContext), sizeOfFpArgs));
    params.push_back(strArg);
    params.push_back(ConstantInt::get(Type::getInt32Ty(llvmContext), sizeOfStrArgs));
    params.push_back(dataGV);
    params.push_back(result);
    params.push_back(ConstantInt::get(Type::getInt32Ty(llvmContext), sizeOfRet));

    Function *callee = getOrCreateCustomFunctionWrapperDeclaration(Builder);
    Builder.CreateCall(callee, params);

    if(sizeOfRet == 1) {
        return Builder.CreateLoad(result);
    } else if(sizeOfRet == 3) {
        std::vector<LLVM_VALUE > resultArray;
        for (int i = 0; i < 3; i++) {
            LLVM_VALUE ptr = Builder.CreateConstGEP1_32(result, i);
            resultArray.push_back(Builder.CreateLoad(ptr));
        }
        return createVecVal(Builder, resultArray);
    }

    assert(false);
    return 0;
}

}

extern "C" void evaluateVarRef(ExprVarRef* seVR, double *result) {
    seVR->eval(result);
}


namespace SeExpr2 {

LLVM_VALUE promoteToDim(LLVM_VALUE val, unsigned dim, LLVM_BUILDER Builder) {
    Type *srcTy = val->getType();
    if(srcTy->isVectorTy() || dim <= 1)
        return val;

    assert(srcTy->isDoubleTy());
    return createVecVal(Builder, val, dim);
}

LLVM_VALUE ExprNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    for (int i = 0; i < numChildren(); i++)
        child(i)->codegen(Builder);
    return 0;
}

LLVM_VALUE ExprModuleNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    LLVM_VALUE lastVal = 0;
    for (int i = 0; i < numChildren(); i++)
        lastVal = child(i)->codegen(Builder);
    assert(lastVal);
    return lastVal;
}

LLVM_VALUE ExprBlockNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    LLVM_VALUE lastVal = 0;
    for (int i = 0; i < numChildren(); i++)
        lastVal = child(i)->codegen(Builder);
    assert(lastVal);
    return lastVal;
}

LLVM_VALUE ExprNumNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    return ConstantFP::get(Builder.getContext(), APFloat(_val));
}

LLVM_VALUE ExprBinaryOpNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    LLVM_VALUE c1 = child(0)->codegen(Builder);
    LLVM_VALUE c2 = child(1)->codegen(Builder);
    std::pair<LLVM_VALUE, LLVM_VALUE > pv = promoteBinaryOperandsToAppropriateVector(Builder, c1, c2);
    LLVM_VALUE op1 = pv.first;
    LLVM_VALUE op2 = pv.second;

    switch (_op) {
    case '+':
        return Builder.CreateFAdd(op1, op2);
    case '-':
        return Builder.CreateFSub(op1, op2);
    case '*':
        return Builder.CreateFMul(op1, op2);
    case '/':
        return Builder.CreateFDiv(op1, op2);
    case '%':
        return Builder.CreateFRem(op1, op2);
    case '^': {
        // TODO: make external function reference work with interpreter, libffi
        // TODO: needed for MCJIT??
        // TODO: is the above not already done?!
        std::vector<Type *> arg_type;
        arg_type.push_back(op1->getType());
        Function *fun = Intrinsic::getDeclaration(llvm_getModule(Builder),
                Intrinsic::pow, arg_type);
        return Builder.CreateCall2(fun, op1, op2);
    }
    }

    assert(false && "unexpected op");
    return 0;
}

// This is the def of def-use chain
// We don't go to VarNode::codegen. It is codegen'd here.
LLVM_VALUE ExprAssignNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    // codegen stored value.
    LLVM_VALUE val = child(0)->codegen(Builder);

    const std::string &varName = name();
    if(LLVM_VALUE existingPtr = resolveLocalVar(varName.c_str(), Builder)) {
        Type *ptrType = existingPtr->getType()->getPointerElementType();
        Type *valType = val->getType();
        if(ptrType == valType)
            Builder.CreateStore(val, existingPtr);
        else if(valType->isDoubleTy() && ptrType->isVectorTy()) {// promotion
            LLVM_VALUE proVal = createVecVal(Builder,val, ptrType->getVectorNumElements());
            Builder.CreateStore(proVal, existingPtr);
        } else {
            AllocaInst *varPtr = createAllocaInst(Builder,val->getType());
            varPtr->takeName(existingPtr);
            Builder.CreateStore(val, varPtr);
        }
    } else {
        AllocaInst *varPtr = createAllocaInst(Builder,val->getType(), 1, varName);
        Builder.CreateStore(val, varPtr);
    }

    // ExprAssignNode has no parent node.
    return 0;
}

LLVM_VALUE ExprCompareEqNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    LLVM_VALUE op1 = getFirstElement(child(0)->codegen(Builder), Builder);
    LLVM_VALUE op2 = getFirstElement(child(1)->codegen(Builder), Builder);

    LLVM_VALUE boolVal = 0;
    switch(_op) {
    case '!':
        boolVal = Builder.CreateFCmpONE(op1, op2);
        break;
    case '=':
        boolVal = Builder.CreateFCmpOEQ(op1, op2);
        break;
    default:
        assert(false && "Unkown CompareEq op.");
    }

    return Builder.CreateUIToFP(boolVal, op1->getType());
}

LLVM_VALUE ExprCompareNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    LLVM_VALUE op1 = getFirstElement(child(0)->codegen(Builder), Builder);
    LLVM_VALUE op2 = getFirstElement(child(1)->codegen(Builder), Builder);

    Type *opTy = op1->getType();
    Constant *zero = ConstantFP::get(opTy, 0.0);
    LLVM_VALUE boolVal = 0;

    switch (_op) {
    case '|': {
         LLVM_VALUE op1IsOne = Builder.CreateFCmpUNE(op1, zero);
         LLVM_VALUE op2IsOne = Builder.CreateFCmpUNE(op2, zero);
         boolVal = Builder.CreateOr(op1IsOne, op2IsOne);
         break;
    }
    case '&': {
        LLVM_VALUE op1IsOne = Builder.CreateFCmpONE(op1, zero);
        LLVM_VALUE op2IsOne = Builder.CreateFCmpONE(op2, zero);
        boolVal = Builder.CreateAnd(op1IsOne, op2IsOne);
        break;
    }
    case 'g':
        boolVal = Builder.CreateFCmpOGE(op1, op2);
        break;
    case 'l':
        boolVal = Builder.CreateFCmpOLE(op1, op2);
        break;
    case '>':
        boolVal = Builder.CreateFCmpOGT(op1, op2);
        break;
    case '<':
        boolVal = Builder.CreateFCmpOLT(op1, op2);
        break;
    default:
        assert(false && "Unkown Compare op.");
    }

    return Builder.CreateUIToFP(boolVal, opTy);
}

LLVM_VALUE ExprCondNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    LLVM_VALUE condVal = getFirstElement(child(0)->codegen(Builder), Builder);
    LLVM_VALUE cond = Builder.CreateFCmpUNE(condVal,
                                     ConstantFP::get(condVal->getType(), 0.0));
    LLVM_VALUE trueVal = child(1)->codegen(Builder);
    LLVM_VALUE falseVal = child(2)->codegen(Builder);
    std::pair<LLVM_VALUE, LLVM_VALUE> pv = promoteBinaryOperandsToAppropriateVector(Builder, trueVal, falseVal);
    return Builder.CreateSelect(cond, pv.first, pv.second);
}

LLVM_VALUE ExprFuncNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    LLVMContext &llvmContext = Builder.getContext();
    Module *M = llvm_getModule(Builder);
    std::string calleeName(name());

    /************* call local function or printf *************/
    Function *callee = M->getFunction(calleeName);
    if(calleeName == "printf") {
        if(!callee) {
            FunctionType *FT = FunctionType::get(Type::getVoidTy(llvmContext),
                    Type::getInt8PtrTy(llvmContext), true);
            callee = Function::Create(FT, GlobalValue::ExternalLinkage,
                                      "printf", llvm_getModule(Builder));
        }
        return callPrintf(this, Builder, callee);
    } else if (callee) {
        std::vector<LLVM_VALUE > args = promoteArgs(codegenFuncCallArgs(Builder,this), Builder,
                           callee->getFunctionType());
        return Builder.CreateCall(callee, args);
    }

    /************* call standard function or custom function *************/
    // call custom function
    const ExprFuncStandard* standfunc =
                        dynamic_cast<const ExprFuncStandard*>(_func->funcx());
    if(!standfunc)
        return callCustomFunction(this, Builder);

    // call standard function
    // get function pointer
    ExprFuncStandard::FuncType seFuncType = standfunc->getFuncType();
    FunctionType *llvmFuncType =  getSeExprFuncStandardLLVMType(seFuncType, llvmContext);
    void *fp = standfunc->getFuncPointer();
    ConstantInt *funcAddr = ConstantInt::get(Type::getInt64Ty(llvmContext), (uint64_t)fp);
    LLVM_VALUE addrVal = Builder.CreateIntToPtr(funcAddr, PointerType::getUnqual(llvmFuncType));

    // Collect distribution positions
    std::vector<LLVM_VALUE > args = codegenFuncCallArgs(Builder,this);
    std::set<int> distributionArgPos;
    if(seFuncType == ExprFuncStandard::FUNCN) {
        for(unsigned i = 0; i < args.size(); ++i)
            if(args[i]->getType()->isVectorTy())
                distributionArgPos.insert(i);
    } else if(
       seFuncType == ExprFuncStandard::FUNCNV ||
       seFuncType == ExprFuncStandard::FUNCNVV) {

    } else {
        unsigned shift = isReturnVector(seFuncType)? 1: 0;
        for(unsigned i = 0; i < args.size(); ++i) {
            Type *paramType = llvmFuncType->getParamType(i+shift);
            Type *argType = args[i]->getType();
            if(argType->isVectorTy() && paramType->isDoubleTy())
                distributionArgPos.insert(i);
        }
    }

    if(distributionArgPos.size() == 0)
        return executeStandardFunction(Builder, seFuncType, args, addrVal);

    Type *firstArgType = args[*distributionArgPos.begin()]->getType();
    assert(firstArgType->isVectorTy());

    std::vector<LLVM_VALUE > ret;
    for(unsigned i = 0; i < firstArgType->getVectorNumElements(); ++i) {
        LLVM_VALUE idx = ConstantInt::get(Type::getInt32Ty(llvmContext), i);
        std::vector<LLVM_VALUE > realArgs;
        for(unsigned j = 0; j < args.size(); ++j) {
            LLVM_VALUE realArg = args[j];
            if(distributionArgPos.count(j)) {
                if(args[j]->getType()->isPointerTy())
                    realArg = Builder.CreateLoad(Builder.CreateConstGEP2_32(args[j], 0, i));
                else
                    realArg = Builder.CreateExtractElement(args[j], idx);
            }

            realArgs.push_back(realArg);
        }
        ret.push_back(executeStandardFunction(Builder, seFuncType, realArgs, addrVal));
    }
    return createVecVal(Builder, ret);
}

LLVM_VALUE ExprIfThenElseNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    LLVM_VALUE condVal = getFirstElement(child(0)->codegen(Builder), Builder);
    Type *condTy = condVal->getType();

    LLVMContext &llvmContext = Builder.getContext();

    Constant *zero = ConstantFP::get(condTy, 0.0);
    LLVM_VALUE intCond = Builder.CreateFCmpUNE(condVal, zero);

    Function *F = llvm_getFunction(Builder);
    BasicBlock *thenBlock = BasicBlock::Create(llvmContext, "then", F);
    BasicBlock *elseBlock = BasicBlock::Create(llvmContext, "else", F);
    BasicBlock *phiBlock = BasicBlock::Create(llvmContext, "phi", F);
    Builder.CreateCondBr(intCond, thenBlock, elseBlock);

    Builder.SetInsertPoint(thenBlock);
    child(1)->codegen(Builder);
    Builder.CreateBr(phiBlock);

    Builder.SetInsertPoint(elseBlock);
    child(2)->codegen(Builder);
    Builder.CreateBr(phiBlock);

    Builder.SetInsertPoint(phiBlock);

    return 0;
}

LLVM_VALUE ExprLocalFunctionNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    IRBuilder<>::InsertPoint oldIP = Builder.saveIP();
    LLVMContext &llvmContext = Builder.getContext();

    // codegen prototype
    Function *F = cast<Function>(child(0)->codegen(Builder));

    // create alloca for args
    BasicBlock *BB = BasicBlock::Create(llvmContext, "entry", F);
    Builder.SetInsertPoint(BB);
    Function::arg_iterator AI = F->arg_begin();
    for (int i = 0, e = F->arg_size(); i != e; ++i, ++AI) {
        AllocaInst *Alloca = createAllocaInst(Builder, AI->getType(), 1, AI->getName());
        Alloca->takeName(AI);
        Builder.CreateStore(AI, Alloca);
    }

    LLVM_VALUE result = 0;
    for (int i = 1; i < numChildren(); i++)
        result = child(i)->codegen(Builder);

    Builder.CreateRet(result);
    Builder.restoreIP(oldIP);
    return 0;
}

LLVM_VALUE ExprPrototypeNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    LLVMContext &llvmContext = Builder.getContext();

    // get arg type
    std::vector<Type*>ParamTys;
    for(int i = 0; i < numChildren(); ++i)
        ParamTys.push_back(createLLVMTyForSeExprType(llvmContext, argType(i)));
    // get ret type
    Type* retTy = createLLVMTyForSeExprType(llvmContext, returnType());

    FunctionType *FT = FunctionType::get(retTy, ParamTys, false);
    Function *F = Function::Create(FT, GlobalValue::InternalLinkage,
                                   name(), llvm_getModule(Builder));

    // Set names for all arguments.
    Function::arg_iterator AI = F->arg_begin();
    for (int i = 0, e = numChildren(); i != e; ++i, ++AI) {
        const ExprVarNode *childNode = dynamic_cast<const ExprVarNode*>(child(i));
        assert(childNode);
        AI->setName(childNode->name());
    }

    return F;
}

LLVM_VALUE ExprStrNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    return Builder.CreateGlobalStringPtr(_str);
}

LLVM_VALUE ExprSubscriptNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    LLVM_VALUE op1 = child(0)->codegen(Builder);
    LLVM_VALUE op2 = child(1)->codegen(Builder);

    if(op1->getType()->isDoubleTy())
        return op1;

    LLVMContext &llvmContext = Builder.getContext();
    LLVM_VALUE idx = Builder.CreateFPToUI(op2, Type::getInt32Ty(llvmContext));
    return Builder.CreateExtractElement(op1, idx);
}

LLVM_VALUE ExprUnaryOpNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    LLVM_VALUE op1 = child(0)->codegen(Builder);
    Type *op1Ty = op1->getType();
    Constant *negateZero = ConstantFP::getZeroValueForNegation(op1Ty);
    Constant *zero = ConstantFP::get(op1Ty, 0.0);
    Constant *one = ConstantFP::get(op1Ty, 1.0);

    switch (_op) {
    case '-':
        return Builder.CreateFSub(negateZero, op1);
    case '~': {
        LLVM_VALUE neg = Builder.CreateFSub(negateZero, op1);
        return Builder.CreateFAdd(neg, one);
    }
    case '!': {
        LLVM_VALUE eqZero = Builder.CreateFCmpOEQ(zero, op1);
        return Builder.CreateSelect(eqZero, one, zero);
    }
    }

    assert(false && "not implemented.");
    return 0;
}

// This is the use of def-use chain
LLVM_VALUE ExprVarNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    if(_var) {
        // All external var has the prefix "external_" in current function to avoid
        // potential name conflict with local variable
        std::string varName("external_");
        varName.append(name());
        if(LLVM_VALUE valPtr = resolveLocalVar(varName.c_str(), Builder))
            return Builder.CreateLoad(valPtr);
        LLVMContext &llvmContext = Builder.getContext();
        Type *voidPtrType = Type::getInt8PtrTy(llvmContext);
        Type *doubleTy = Type::getDoubleTy(llvmContext);
        ExprVarRef* varRef = expr()->resolveVar(name());
        ConstantInt *varAddr = ConstantInt::get(Type::getInt64Ty(llvmContext),
                                                (uint64_t)varRef);
        LLVM_VALUE addrVal = Builder.CreateIntToPtr(varAddr, voidPtrType);
        Function *evalVarFunc = getOrCreateEvalVarDeclaration(Builder);

        int dim = varRef->type().dim();
        AllocaInst *varAlloca = createAllocaInst(Builder, doubleTy, dim);
        LLVM_VALUE params[2] = {addrVal, varAlloca};
        Builder.CreateCall(evalVarFunc, params);

        LLVM_VALUE ret = 0;
        if(dim == 1) {
            ret = Builder.CreateLoad(varAlloca);
        } else {
            assert(dim == 3 && "future work.");
            ret = createVecValFromAlloca(Builder, varAlloca, dim);
        }

        AllocaInst *thisvar = createAllocaInst(Builder, ret->getType(), 1, varName);
        Builder.CreateStore(ret, thisvar);
        return ret;
    } else if(_localVar) {
        ExprType varTy = _localVar->type();
        if(varTy.isFP() || varTy.isString()) {
            LLVM_VALUE valPtr = resolveLocalVar(name(), Builder);
            assert(valPtr && "can not found symbol?");
            return Builder.CreateLoad(valPtr);
        }
    }

    assert(false);
    return 0;
}

LLVM_VALUE ExprVecNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    std::vector<LLVM_VALUE > elems;
    for (int i = 0; i < numChildren(); i++)
        elems.push_back(child(i)->codegen(Builder));

    Type *elemType = elems[0]->getType();
    if(elemType->isDoubleTy())
        return createVecVal(Builder, elems);

    LLVMContext &llvmContext = Builder.getContext();
    ConstantInt *zero = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
    std::vector<LLVM_VALUE > firstArgs;
    for (int i = 0; i < numChildren(); i++)
        firstArgs.push_back(Builder.CreateExtractElement(elems[i], zero));
    return createVecVal(Builder, firstArgs);
}

}
