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

#ifdef SEEXPR_ENABLE_LLVM
#include "ExprLLVM.h"
#include "ExprLLVMAll.h"
#include "ExprNode.h"
#include "ExprFunc.h"
#include "VarBlock.h"
#include <array>
using namespace llvm;
using namespace SeExpr2;

// TODO: Use ordered or unordered float comparison?
// TODO: factor out commonly used llvm types
// TODO: factor out integer/double constant creation
namespace {

Function *llvm_getFunction(LLVM_BUILDER Builder) { return Builder.GetInsertBlock()->getParent(); }

Module *llvm_getModule(LLVM_BUILDER Builder) { return llvm_getFunction(Builder)->getParent(); }

//! Turn LLVM type into a std::string, convenience to work around needing to use raw_string_ostream everywhere
std::string llvmTypeString(llvm::Type *type) {
    std::string myString;
    llvm::raw_string_ostream rawStream(myString);
    type->print(rawStream);
    return rawStream.str();
}

bool isVarArg(ExprFuncStandard::FuncType seFuncType) {
    if (seFuncType == ExprFuncStandard::FUNCN || seFuncType == ExprFuncStandard::FUNCNV ||
        seFuncType == ExprFuncStandard::FUNCNVV)
        return true;
    return false;
}

bool isReturnVector(ExprFuncStandard::FuncType seFuncType) {
    if (seFuncType == ExprFuncStandard::FUNC1VV || seFuncType == ExprFuncStandard::FUNC2VV ||
        seFuncType == ExprFuncStandard::FUNCNVV)
        return true;
    return false;
}

bool isTakeOnlyDoubleArg(ExprFuncStandard::FuncType seFuncType) {
    if (seFuncType <= ExprFuncStandard::FUNC6 || seFuncType == ExprFuncStandard::FUNCN) return true;
    return false;
}

FunctionType *getSeExprFuncStandardLLVMType(ExprFuncStandard::FuncType sft, LLVMContext &llvmContext) {
    assert(sft != ExprFuncStandard::NONE);

    Type *intType = TypeBuilder<int, false>::get(llvmContext);
    Type *doubleType = TypeBuilder<double, false>::get(llvmContext);
    Type *doublePtrType = TypeBuilder<double *, false>::get(llvmContext);
    Type *voidType = TypeBuilder<void, false>::get(llvmContext);
    FunctionType *FT = 0;

    if (sft <= ExprFuncStandard::FUNC6) {
        std::vector<Type *> paramTypes;
        switch (sft) {
            case ExprFuncStandard::FUNC6:
                paramTypes.push_back(doubleType);
            case ExprFuncStandard::FUNC5:
                paramTypes.push_back(doubleType);
            case ExprFuncStandard::FUNC4:
                paramTypes.push_back(doubleType);
            case ExprFuncStandard::FUNC3:
                paramTypes.push_back(doubleType);
            case ExprFuncStandard::FUNC2:
                paramTypes.push_back(doubleType);
            case ExprFuncStandard::FUNC1:
                paramTypes.push_back(doubleType);
            case ExprFuncStandard::FUNC0:
            default:
                FT = FunctionType::get(doubleType, paramTypes, false);
        }
    } else if (sft == ExprFuncStandard::FUNC1V) {
        Type *paramTypes[1] = {doublePtrType};
        FT = FunctionType::get(doubleType, paramTypes, false);
    } else if (sft == ExprFuncStandard::FUNC2V) {
        Type *paramTypes[2] = {doublePtrType, doublePtrType};
        FT = FunctionType::get(doubleType, paramTypes, false);
    } else if (sft == ExprFuncStandard::FUNC1VV) {
        Type *paramTypes[2] = {doublePtrType, doublePtrType};
        FT = FunctionType::get(voidType, paramTypes, false);
    } else if (sft == ExprFuncStandard::FUNC2VV) {
        Type *paramTypes[3] = {doublePtrType, doublePtrType, doublePtrType};
        FT = FunctionType::get(voidType, paramTypes, false);
    } else if (sft == ExprFuncStandard::FUNCN) {
        Type *paramTypes[2] = {intType, doublePtrType};
        FT = FunctionType::get(doubleType, paramTypes, false);
    } else if (sft == ExprFuncStandard::FUNCNV) {
        Type *paramTypes[2] = {intType, doublePtrType};
        FT = FunctionType::get(doubleType, paramTypes, false);
    } else if (sft == ExprFuncStandard::FUNCNVV) {
        Type *paramTypes[3] = {doublePtrType, intType, doublePtrType};
        FT = FunctionType::get(voidType, paramTypes, false);
    } else
        assert(false);

    return FT;
}

Type *createLLVMTyForSeExprType(LLVMContext &llvmContext, ExprType seType) {
    if (seType.isFP()) {
        int dim = seType.dim();
        return dim == 1 ? Type::getDoubleTy(llvmContext) : VectorType::get(Type::getDoubleTy(llvmContext), dim);
    } else if (seType.isString()) {
        // TODO: post c++11
        // static_assert(sizeof(char*) == 8, "Expect 64-bit pointers");
        return Type::getInt8PtrTy(llvmContext);
    }
    assert(!"unknown SeExpr type encountered");  // unknown type
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
LLVM_VALUE createVecVal(LLVM_BUILDER Builder, ArrayRef<LLVM_VALUE> val, const std::string &name = "") {
    if (!val.size()) return 0;

    LLVMContext &llvmContext = Builder.getContext();
    unsigned dim = val.size();
    VectorType *elemType = VectorType::get(val[0]->getType(), dim);
    LLVM_VALUE vecVal = UndefValue::get(elemType);
    for (unsigned i = 0; i < dim; i++)
        vecVal = Builder.CreateInsertElement(vecVal, val[i], ConstantInt::get(Type::getInt32Ty(llvmContext), i), name);
    return vecVal;
}

LLVM_VALUE createVecValFromAlloca(LLVM_BUILDER Builder, AllocaInst *destPtr, unsigned vecLen) {
    Type *destTy = destPtr->getType()->getPointerElementType();
    assert(destTy->isDoubleTy() || destTy->isArrayTy());
    std::vector<LLVM_VALUE> vals;

    for (unsigned i = 0; i < vecLen; ++i) {
        LLVM_VALUE ptr = destTy->isDoubleTy() ? Builder.CreateConstGEP1_32(destPtr, i)
                                              : Builder.CreateConstGEP2_32(nullptr, destPtr, 0, i);
        vals.push_back(Builder.CreateLoad(ptr));
    }

    return createVecVal(Builder, vals);
}

LLVM_VALUE getFirstElement(LLVM_VALUE V, IRBuilder<> Builder) {
    Type *VTy = V->getType();
    if (VTy->isDoubleTy()) return V;

    assert(VTy->isVectorTy());
    LLVMContext &llvmContext = Builder.getContext();
    LLVM_VALUE zero = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
    return Builder.CreateExtractElement(V, zero);
}

LLVM_VALUE promoteToTy(LLVM_VALUE val, Type *destTy, LLVM_BUILDER Builder) {
    Type *srcTy = val->getType();
    if (srcTy == destTy) return val;

    if (destTy->isDoubleTy()) return val;

    return createVecVal(Builder, val, destTy->getVectorNumElements());
}

AllocaInst *createAllocaInst(LLVM_BUILDER Builder, Type *ty, unsigned arraySize = 1, const std::string &varName = "") {
    // move builder to first position of entry BB
    BasicBlock *entryBB = &llvm_getFunction(Builder)->getEntryBlock();
    IRBuilder<>::InsertPoint oldIP = Builder.saveIP();
    if (entryBB->size())
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

AllocaInst *createArray(LLVM_BUILDER Builder, Type *ty, unsigned arraySize, const std::string &varName = "") {
    // move builder to first position of entry BB
    BasicBlock *entryBB = &llvm_getFunction(Builder)->getEntryBlock();
    IRBuilder<>::InsertPoint oldIP = Builder.saveIP();
    if (entryBB->size())
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

std::pair<LLVM_VALUE, LLVM_VALUE> promoteBinaryOperandsToAppropriateVector(LLVM_BUILDER Builder,
                                                                           LLVM_VALUE op1,
                                                                           LLVM_VALUE op2) {
    Type *op1Ty = op1->getType();
    Type *op2Ty = op2->getType();
    if (op1Ty == op2Ty) return std::make_pair(op1, op2);

    LLVM_VALUE toPromote = op1;
    LLVM_VALUE target = op2;
    if (op1Ty->isVectorTy()) std::swap(toPromote, target);

    assert(target->getType()->isVectorTy());

    unsigned dim = target->getType()->getVectorNumElements();
    LLVM_VALUE vecVal = createVecVal(Builder, toPromote, dim);

    if (op1Ty->isVectorTy())
        op2 = vecVal;
    else
        op1 = vecVal;

    return std::make_pair(op1, op2);
}

LLVM_VALUE promoteOperand(LLVM_BUILDER Builder, const ExprType refType, LLVM_VALUE val) {
    Type *valTy = val->getType();
    if (refType.isFP() && refType.dim() > 1 && !valTy->isVectorTy()) {
        return createVecVal(Builder, val, refType.dim());
    } else {
        return val;
    }
}

AllocaInst *storeVectorToDoublePtr(LLVM_BUILDER Builder, LLVM_VALUE vecVal) {
    LLVMContext &llvmContext = Builder.getContext();
    AllocaInst *doublePtr =
        createAllocaInst(Builder, Type::getDoubleTy(llvmContext), vecVal->getType()->getVectorNumElements());
    for (unsigned i = 0; i < 3; ++i) {
        LLVM_VALUE idx = ConstantInt::get(Type::getInt32Ty(llvmContext), i);
        LLVM_VALUE val = Builder.CreateExtractElement(vecVal, idx);
        LLVM_VALUE ptr = Builder.CreateConstGEP1_32(doublePtr, i);
        Builder.CreateStore(val, ptr);
    }
    return doublePtr;
}

std::vector<LLVM_VALUE> codegenFuncCallArgs(LLVM_BUILDER Builder, const ExprFuncNode *funcNode) {
    std::vector<LLVM_VALUE> args;
    for (int i = 0; i < funcNode->numChildren(); ++i) args.push_back(funcNode->child(i)->codegen(Builder));
    return args;
}

std::vector<LLVM_VALUE> promoteArgs(std::vector<LLVM_VALUE> args, LLVM_BUILDER Builder, FunctionType *llvmFuncType) {
    std::vector<LLVM_VALUE> ret;
    for (unsigned i = 0; i < args.size(); ++i)
        ret.push_back(promoteToTy(args[i], llvmFuncType->getParamType(i), Builder));
    return ret;
}

std::vector<LLVM_VALUE> promoteArgs(std::vector<LLVM_VALUE> args,
                                    LLVM_BUILDER Builder,
                                    ExprFuncStandard::FuncType seFuncType) {
    if (isTakeOnlyDoubleArg(seFuncType)) return args;

    LLVMContext &llvmContext = Builder.getContext();
    VectorType *destTy = VectorType::get(Type::getDoubleTy(llvmContext), 3);
    std::vector<LLVM_VALUE> ret;
    for (unsigned i = 0; i < args.size(); ++i) ret.push_back(promoteToTy(args[i], destTy, Builder));
    return ret;
}

std::vector<LLVM_VALUE> replaceVecArgWithDoublePointer(LLVM_BUILDER Builder, std::vector<LLVM_VALUE> args) {
    for (unsigned i = 0; i < args.size(); ++i)
        if (args[i]->getType()->isVectorTy()) args[i] = storeVectorToDoublePtr(Builder, args[i]);
    return args;
}

std::vector<LLVM_VALUE> convertArgsToPointerAndLength(LLVM_BUILDER Builder,
                                                      std::vector<LLVM_VALUE> actualArgs,
                                                      ExprFuncStandard::FuncType seFuncType) {
    assert(isVarArg(seFuncType));

    LLVMContext &llvmContext = Builder.getContext();
    unsigned numArgs = actualArgs.size();

    // type of arg should be either double or double*(aka. vector).
    for (unsigned i = 0; i < numArgs; ++i)
        assert(actualArgs[i]->getType()->isDoubleTy() || actualArgs[i]->getType() == Type::getDoublePtrTy(llvmContext));

    std::vector<LLVM_VALUE> args;
    // push "int n"
    args.push_back(ConstantInt::get(Type::getInt32Ty(llvmContext), numArgs));

    if (seFuncType == ExprFuncStandard::FUNCN) {
        AllocaInst *doublePtr = createAllocaInst(Builder, Type::getDoubleTy(llvmContext), numArgs);
        for (unsigned i = 0; i < numArgs; ++i) {
            LLVM_VALUE ptr = Builder.CreateConstGEP1_32(doublePtr, i);
            Builder.CreateStore(actualArgs[i], ptr);
        }
        args.push_back(doublePtr);
        return args;
    }

    AllocaInst *arrayPtr = createArray(Builder, ArrayType::get(Type::getDoubleTy(llvmContext), 3), numArgs);
    for (unsigned i = 0; i < numArgs; ++i) {
        LLVM_VALUE toInsert = actualArgs[i];
        LLVM_VALUE subArrayPtr = Builder.CreateConstGEP2_32(nullptr, arrayPtr, 0, i);
        for (unsigned j = 0; j < 3; ++j) {
            LLVM_VALUE destAddr = Builder.CreateConstGEP2_32(nullptr, subArrayPtr, 0, j);
            LLVM_VALUE srcAddr = Builder.CreateConstGEP1_32(toInsert, j);
            Builder.CreateStore(Builder.CreateLoad(srcAddr), destAddr);
        }
    }
    args.push_back(Builder.CreateBitCast(arrayPtr, Type::getDoublePtrTy(llvmContext)));
    return args;
}

LLVM_VALUE executeStandardFunction(LLVM_BUILDER Builder,
                                   ExprFuncStandard::FuncType seFuncType,
                                   std::vector<LLVM_VALUE> args,
                                   LLVM_VALUE addrVal) {
    LLVMContext &llvmContext = Builder.getContext();

    args = promoteArgs(args, Builder, seFuncType);
    args = replaceVecArgWithDoublePointer(Builder, args);

    if (isVarArg(seFuncType)) args = convertArgsToPointerAndLength(Builder, args, seFuncType);

    if (isReturnVector(seFuncType) == false) return Builder.CreateCall(addrVal, args);

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
    std::vector<LLVM_VALUE> args;

    // TODO: promotion for printf?
    {  // preprocess format string.
        const ExprStrNode *formatStrNode = dynamic_cast<const ExprStrNode *>(seFunc->child(0));
        assert(formatStrNode);
        std::string formatStr(formatStrNode->str());
        std::string::size_type pos = std::string::npos;
        while ((pos = formatStr.find("%v")) != std::string::npos) formatStr.replace(pos, 2, std::string("[%f,%f,%f]"));
        formatStr.append("\n");
        args.push_back(Builder.CreateGlobalStringPtr(formatStr));
    }

    for (int i = 1; i < seFunc->numChildren(); ++i) {
        LLVM_VALUE arg = seFunc->child(i)->codegen(Builder);
        if (arg->getType()->isVectorTy()) {
            AllocaInst *vecArray = storeVectorToDoublePtr(Builder, arg);
            for (unsigned i = 0; i < arg->getType()->getVectorNumElements(); ++i) {
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

    // get the function's arguments
    std::vector<LLVM_VALUE> args = codegenFuncCallArgs(Builder, funcNode);
    int nargs = funcNode->numChildren();
    assert(nargs == (int)args.size());

    // get the number of items that the function returns
    unsigned sizeOfRet = (unsigned)funcNode->type().dim();
    assert(sizeOfRet == 1 || funcNode->type().isFP());

    // TODO: is this necessary ? Doesn't seem to be used :/
    createAllocaInst(Builder, Type::getDoubleTy(llvmContext), sizeOfRet);

    // calculate how much space for opData, fpArg and strArg
    unsigned sizeOfFpArgs = 1 + sizeOfRet;
    unsigned sizeOfStrArgs = 2;
    for (int i = 0; i < nargs; ++i) {
        ExprType argType = funcNode->child(i)->type();
        if (argType.isFP()) {
            sizeOfFpArgs += std::max(funcNode->promote(i), argType.dim());
        } else if (argType.isString()) {
            sizeOfStrArgs += 1;
        } else {
            assert(false && "invalid type encountered");
        }
    }

    // a few types that are reused throughout this function
    Type*           int32Ty     = Type::getInt32Ty(llvmContext);        // int
    Type*           doubleTy    = Type::getDoubleTy(llvmContext);       // double
    PointerType*    int8PtrTy   = Type::getInt8PtrTy(llvmContext);      // char*
    Type*           int64Ty     = Type::getInt64Ty(llvmContext);        // int64_t


    // allocate data that we will feed to SeExpr2LLVMEvalCustomFunction on the stack
    AllocaInst *opDataArg = createAllocaInst(Builder, int32Ty, (unsigned)nargs + 4, "opDataArgPtr");
    AllocaInst *fpArg = createAllocaInst(Builder, doubleTy, sizeOfFpArgs, "fpArgPtr");
    AllocaInst *strArg = createAllocaInst(Builder, int8PtrTy, sizeOfStrArgs, "strArgPtr");

    // fill fpArgPtr's first value
    Builder.CreateStore(ConstantFP::get(doubleTy, nargs), fpArg);

    // fill opDataArgPtr
    Builder.CreateStore(ConstantInt::get(int32Ty, 0), Builder.CreateConstGEP1_32(opDataArg, 0));
    Builder.CreateStore(ConstantInt::get(int32Ty, 1), Builder.CreateConstGEP1_32(opDataArg, 1));
    Builder.CreateStore(ConstantInt::get(int32Ty, 1), Builder.CreateConstGEP1_32(opDataArg, 2));
    Builder.CreateStore(ConstantInt::get(int32Ty, 0), Builder.CreateConstGEP1_32(opDataArg, 3));

    // Load arguments into the pseudo interpreter data structure
    unsigned fpIdx = 1 + sizeOfRet;
    unsigned strIdx = 2;
    for (int argIndex = 0; argIndex < nargs; ++argIndex) {
        int opIndex = argIndex + 4;
        ExprType argType = funcNode->child(argIndex)->type();
        if (argType.isFP()) {
            // store the fpArgPtr indirection index
            Builder.CreateStore(ConstantInt::get(int32Ty, fpIdx), Builder.CreateConstGEP1_32(opDataArg, opIndex));
            if (argType.dim() > 1) {
                for (int comp = 0; comp < argType.dim(); comp++) {
                    LLVM_VALUE compIndex = ConstantInt::get(int32Ty, comp);
                    LLVM_VALUE val = Builder.CreateExtractElement(args[argIndex], compIndex);
                    LLVM_VALUE fpArgPtr = Builder.CreateConstGEP1_32(fpArg, fpIdx + comp);
                    Builder.CreateStore(val, fpArgPtr);
                }
                fpIdx += argType.dim();
            } else {
                // TODO: this needs the promote!!!
                int promote = funcNode->promote(argIndex);
                if (promote) {
                    LLVM_VALUE val = args[argIndex];
                    for (int comp = 0; comp < promote; comp++) {
                        LLVM_VALUE fpArgPtr = Builder.CreateConstGEP1_32(fpArg, fpIdx + comp);
                        Builder.CreateStore(val, fpArgPtr);
                    }
                    fpIdx += promote;
                } else {
                    Builder.CreateStore(args[argIndex], Builder.CreateConstGEP1_32(fpArg, fpIdx));
                    fpIdx++;
                }
            }
        } else if (argType.isString()) {
            // store the strArgPtr indirection index
            Builder.CreateStore(ConstantInt::get(int32Ty, strIdx), Builder.CreateConstGEP1_32(opDataArg, opIndex));
            Builder.CreateStore(args[argIndex], Builder.CreateConstGEP1_32(strArg, strIdx));
            strIdx++;
        }
    }

    // get the module from the builder
    Module* module = llvm_getModule(Builder);

    // TODO: thread safety?
    // TODO: This leaks!
    GlobalVariable *dataGV = new GlobalVariable(*module, int8PtrTy, false, GlobalValue::InternalLinkage, ConstantPointerNull::get(int8PtrTy));

    // call the function
    Builder.CreateCall(
        module->getFunction("SeExpr2LLVMEvalCustomFunction"),
        {
            opDataArg,
            fpArg,
            strArg,
            dataGV,
            ConstantInt::get(int64Ty, (uint64_t)funcNode)
        }
    );

    // read the result from memory
    int resultOffset = 1;
    if (funcNode->type().isFP()) {
        if (sizeOfRet == 1) {
            return Builder.CreateLoad(Builder.CreateConstGEP1_32(fpArg, resultOffset));
        } else if (sizeOfRet > 1) {
            std::vector<LLVM_VALUE> resultArray;
            for (unsigned int comp = 0; comp < sizeOfRet; comp++) {
                LLVM_VALUE ptr = Builder.CreateConstGEP1_32(fpArg, resultOffset + comp);  // skip nargs
                resultArray.push_back(Builder.CreateLoad(ptr));
            }
            return createVecVal(Builder, resultArray);
        }
    } else {
        return Builder.CreateLoad(Builder.CreateConstGEP1_32(strArg, 1));
    }

    assert(false);
    return 0;
}
}

extern "C" void SeExpr2LLVMEvalFPVarRef(ExprVarRef *seVR, double *result) { seVR->eval(result); }
extern "C" void SeExpr2LLVMEvalStrVarRef(ExprVarRef *seVR, char **result) { seVR->eval((const char **)result); }

namespace SeExpr2 {

LLVM_VALUE promoteToDim(LLVM_VALUE val, unsigned dim, LLVM_BUILDER Builder) {
    Type *srcTy = val->getType();
    if (srcTy->isVectorTy() || dim <= 1) return val;

    assert(srcTy->isDoubleTy());
    return createVecVal(Builder, val, dim);
}

LLVM_VALUE ExprNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    for (int i = 0; i < numChildren(); i++) child(i)->codegen(Builder);
    return 0;
}

LLVM_VALUE ExprModuleNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    LLVM_VALUE lastVal = 0;
    for (int i = 0; i < numChildren(); i++) lastVal = child(i)->codegen(Builder);
    assert(lastVal);
    return lastVal;
}

LLVM_VALUE ExprBlockNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    LLVM_VALUE lastVal = 0;
    for (int i = 0; i < numChildren(); i++) lastVal = child(i)->codegen(Builder);
    assert(lastVal);
    return lastVal;
}

LLVM_VALUE ExprNumNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    return ConstantFP::get(Builder.getContext(), APFloat(_val));
}

LLVM_VALUE ExprBinaryOpNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    LLVM_VALUE c1 = child(0)->codegen(Builder);
    LLVM_VALUE c2 = child(1)->codegen(Builder);
    std::pair<LLVM_VALUE, LLVM_VALUE> pv = promoteBinaryOperandsToAppropriateVector(Builder, c1, c2);
    LLVM_VALUE op1 = pv.first;
    LLVM_VALUE op2 = pv.second;

    const bool isString = child(0)->type().isString();

    if (isString == false) {
        switch (_op) {
            case '+':
                return Builder.CreateFAdd(op1, op2);
            case '-':
                return Builder.CreateFSub(op1, op2);
            case '*':
                return Builder.CreateFMul(op1, op2);
            case '/':
                return Builder.CreateFDiv(op1, op2);
            case '%': {
                // niceMod() from v1: b==0 ? 0 : a-floor(a/b)*b
                LLVM_VALUE a = op1, b = op2;
                LLVM_VALUE aOverB = Builder.CreateFDiv(a, b);
                Function *floorFun = Intrinsic::getDeclaration(llvm_getModule(Builder), Intrinsic::floor, op1->getType());
                LLVM_VALUE normal = Builder.CreateFSub(a, Builder.CreateFMul(Builder.CreateCall(floorFun, {aOverB}), b));
                Constant *zero = ConstantFP::get(op1->getType(), 0.0);
                return Builder.CreateSelect(Builder.CreateFCmpOEQ(zero, op1), zero, normal);
            }
            case '^': {
                // TODO: make external function reference work with interpreter, libffi
                // TODO: needed for MCJIT??
                // TODO: is the above not already done?!
                std::vector<Type *> arg_type;
                arg_type.push_back(op1->getType());
                Function *fun = Intrinsic::getDeclaration(llvm_getModule(Builder), Intrinsic::pow, arg_type);
                std::vector<LLVM_VALUE> ops = {op1, op2};
                return Builder.CreateCall(fun, ops);
            }
        }
    } else {
        // precompute a few things
        LLVMContext &context    = Builder.getContext();
        Module      *module     = llvm_getModule(Builder);
        PointerType *i8PtrPtrTy = PointerType::getUnqual(Type::getInt8PtrTy(context));
        Type        *i32Ty      = Type::getInt32Ty(context);
        Function    *strlen     = module->getFunction("strlen");
        Function    *malloc     = module->getFunction("malloc");
        Function    *free       = module->getFunction("free");
        Function    *memset     = module->getFunction("memset");
        Function    *strcat     = module->getFunction("strcat");

        // do magic (see the pseudo C code on the comments at the end
        // of each LLVM instruction)

        // compute the length of the operand strings
        LLVM_VALUE len1 = Builder.CreateCall(strlen, { op1 });              // len1 = strlen(op1);
        LLVM_VALUE len2 = Builder.CreateCall(strlen, { op2 });              // len2 = strlen(op2);
        LLVM_VALUE len = Builder.CreateAdd(len1, len2);                     // len = len1 + len2;

        // allocate and clear memory
        LLVM_VALUE alloc = Builder.CreateCall(malloc, { len });             // alloc = malloc(len1 + len2);
        LLVM_VALUE zero = ConstantInt::get(i32Ty, 0);                       // zero = 0;
        Builder.CreateCall(memset, { alloc, zero, len });                   // memset(alloc, zero, len);

        // concatenate operand strings into output string
        Builder.CreateCall(strcat, { alloc, op1 });                         // strcat(alloc, op1);
        LLVM_VALUE newAlloc = Builder.CreateGEP(nullptr, alloc, len1);      // newAlloc = alloc + len1
        Builder.CreateCall(strcat, { newAlloc, op2 });                      // strcat(alloc, op2);

        // store the address in the node's _out member so that it will be
        // cleaned up when the expression is destroyed.
        APInt outAddr = APInt(64, (uint64_t)&_out);
        LLVM_VALUE out = Constant::getIntegerValue(i8PtrPtrTy, outAddr);    // out = &_out;
        Builder.CreateCall(free, { Builder.CreateLoad(out) });              // free(*out);
        Builder.CreateStore(alloc, out);                                    // *out = alloc
        return alloc;
    }

    assert(false && "unexpected op");
    return 0;
}

// This is the def of def-use chain
// We don't go to VarNode::codegen. It is codegen'd here.
LLVM_VALUE ExprAssignNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    // codegen value to store
    LLVM_VALUE val = child(0)->codegen(Builder);
    // code gen pointer to store into
    const std::string &varName = name();
    LLVM_VALUE varPtr = _localVar->codegen(Builder, varName, val);
    // do actual store
    Builder.CreateStore(val, varPtr);
    return 0;
}

//! LLVM value that has been allocated
LLVM_VALUE ExprLocalVar::codegen(LLVM_BUILDER Builder, const std::string &varName, LLVM_VALUE refValue) const {
    _varPtr = createAllocaInst(Builder, refValue->getType(), 1, varName);
    return _varPtr;
}

LLVM_VALUE ExprCompareEqNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    LLVM_VALUE op1 = getFirstElement(child(0)->codegen(Builder), Builder);
    LLVM_VALUE op2 = getFirstElement(child(1)->codegen(Builder), Builder);

    LLVM_VALUE boolVal = 0;
    switch (_op) {
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
    if (_op == '&' || _op == '|') {
        // Handle & and | specially as conditionals to handle short circuiting!
        LLVMContext &llvmContext = Builder.getContext();

        LLVM_VALUE op1 = getFirstElement(child(0)->codegen(Builder), Builder);
        Type *opTy = op1->getType();
        Constant *zero = ConstantFP::get(opTy, 0.0);

        LLVM_VALUE op1IsOne = Builder.CreateFCmpUNE(op1, zero);

        Function *F = llvm_getFunction(Builder);
        BasicBlock *thenBlock = BasicBlock::Create(llvmContext, "then", F);
        BasicBlock *elseBlock = BasicBlock::Create(llvmContext, "else", F);
        BasicBlock *phiBlock = BasicBlock::Create(llvmContext, "phi", F);
        Builder.CreateCondBr(op1IsOne, thenBlock, elseBlock);

        LLVM_VALUE op2IsOne;
        Type *intTy = Type::getInt1Ty(llvmContext);
        Type *doubleTy = Type::getDoubleTy(llvmContext);
        llvm::PHINode *phiNode = nullptr;
        if (_op == '&') {
            // TODO: full IfThenElsenot needed
            Builder.SetInsertPoint(thenBlock);
            LLVM_VALUE op2 = child(1)->codegen(Builder);
            op2IsOne = Builder.CreateFCmpUNE(op2, zero);
            Builder.CreateBr(phiBlock);
            thenBlock = Builder.GetInsertBlock();

            Builder.SetInsertPoint(elseBlock);
            Builder.CreateBr(phiBlock);
            Builder.SetInsertPoint(phiBlock);

            phiNode = Builder.CreatePHI(intTy, 2, "iftmp");
            phiNode->addIncoming(op2IsOne, thenBlock);
            phiNode->addIncoming(op1IsOne, elseBlock);
        } else if (_op == '|') {
            // TODO: full IfThenElsenot needed
            Builder.SetInsertPoint(thenBlock);
            Builder.CreateBr(phiBlock);

            Builder.SetInsertPoint(elseBlock);
            LLVM_VALUE op2 = child(1)->codegen(Builder);
            op2IsOne = Builder.CreateFCmpUNE(op2, zero);
            Builder.CreateBr(phiBlock);
            elseBlock = Builder.GetInsertBlock();

            Builder.SetInsertPoint(phiBlock);
            phiNode = Builder.CreatePHI(intTy, 2, "iftmp");
            phiNode->addIncoming(op1IsOne, thenBlock);
            phiNode->addIncoming(op2IsOne, elseBlock);
        } else {
            throw std::runtime_error("Logical inconsistency.");
        }
        LLVM_VALUE out = Builder.CreateUIToFP(phiNode, doubleTy);
        return out;
    } else {
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
                assert(false);  // handled above
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
}

LLVM_VALUE ExprCondNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {

#if 0  // old non-short circuit
    LLVM_VALUE condVal = getFirstElement(child(0)->codegen(Builder), Builder);
    LLVM_VALUE cond = Builder.CreateFCmpUNE(condVal,
                                     ConstantFP::get(condVal->getType(), 0.0));
    LLVM_VALUE trueVal = child(1)->codegen(Builder);
    LLVM_VALUE falseVal = child(2)->codegen(Builder);
    std::pair<LLVM_VALUE, LLVM_VALUE> pv = promoteBinaryOperandsToAppropriateVector(Builder, trueVal, falseVal);
    return Builder.CreateSelect(cond, pv.first, pv.second);
#else  // new short circuit version
    LLVM_VALUE condVal = getFirstElement(child(0)->codegen(Builder), Builder);
    LLVM_VALUE condAsBool = Builder.CreateFCmpUNE(condVal, ConstantFP::get(condVal->getType(), 0.0));
    LLVMContext &llvmContext = Builder.getContext();
    Function *F = llvm_getFunction(Builder);
    BasicBlock *thenBlock = BasicBlock::Create(llvmContext, "then", F);
    BasicBlock *elseBlock = BasicBlock::Create(llvmContext, "else", F);
    BasicBlock *phiBlock = BasicBlock::Create(llvmContext, "phi", F);
    Builder.CreateCondBr(condAsBool, thenBlock, elseBlock);

    Builder.SetInsertPoint(thenBlock);
    LLVM_VALUE trueVal = promoteOperand(Builder, _type, child(1)->codegen(Builder));
    Builder.CreateBr(phiBlock);
    thenBlock = Builder.GetInsertBlock();

    Builder.SetInsertPoint(elseBlock);
    LLVM_VALUE falseVal = promoteOperand(Builder, _type, child(2)->codegen(Builder));
    Builder.CreateBr(phiBlock);
    elseBlock = Builder.GetInsertBlock();

    Builder.SetInsertPoint(phiBlock);
    llvm::PHINode *phiNode = Builder.CreatePHI(trueVal->getType(), 2, "iftmp");
    phiNode->addIncoming(trueVal, thenBlock);
    phiNode->addIncoming(falseVal, elseBlock);
    return phiNode;

#endif
}

LLVM_VALUE ExprFuncNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    LLVMContext &llvmContext = Builder.getContext();
    Module *M = llvm_getModule(Builder);
    std::string calleeName(name());

    /************* call local function or printf *************/
    Function *callee = M->getFunction(calleeName);
    if (calleeName == "printf") {
        if (!callee) {
            FunctionType *FT = FunctionType::get(Type::getVoidTy(llvmContext), Type::getInt8PtrTy(llvmContext), true);
            callee = Function::Create(FT, GlobalValue::ExternalLinkage, "printf", llvm_getModule(Builder));
        }
        return callPrintf(this, Builder, callee);
    } else if (callee) {
        std::vector<LLVM_VALUE> args =
            promoteArgs(codegenFuncCallArgs(Builder, this), Builder, callee->getFunctionType());
        return Builder.CreateCall(callee, args);
    }

    /************* call standard function or custom function *************/
    // call custom function
    const ExprFuncStandard *standfunc = dynamic_cast<const ExprFuncStandard *>(_func->funcx());
    if (!standfunc) return callCustomFunction(this, Builder);

    // call standard function
    // get function pointer
    ExprFuncStandard::FuncType seFuncType = standfunc->getFuncType();
    FunctionType *llvmFuncType = getSeExprFuncStandardLLVMType(seFuncType, llvmContext);
    void *fp = standfunc->getFuncPointer();
    ConstantInt *funcAddr = ConstantInt::get(Type::getInt64Ty(llvmContext), (uint64_t)fp);
    LLVM_VALUE addrVal = Builder.CreateIntToPtr(funcAddr, PointerType::getUnqual(llvmFuncType));

    // Collect distribution positions
    std::vector<LLVM_VALUE> args = codegenFuncCallArgs(Builder, this);
    std::vector<int> argumentIsVectorAndNeedsDistribution(args.size(), 0);
    Type *maxVectorArgType = nullptr;
    if (seFuncType == ExprFuncStandard::FUNCN) {
        for (unsigned i = 0; i < args.size(); ++i)
            if (args[i]->getType()->isVectorTy()) {
                maxVectorArgType = args[i]->getType();
                argumentIsVectorAndNeedsDistribution[i] = 1;
            }
    } else if (seFuncType == ExprFuncStandard::FUNCNV || seFuncType == ExprFuncStandard::FUNCNVV) {

    } else {
        unsigned shift = isReturnVector(seFuncType) ? 1 : 0;
        for (unsigned i = 0; i < args.size(); ++i) {
            Type *paramType = llvmFuncType->getParamType(i + shift);
            Type *argType = args[i]->getType();
            if (argType->isVectorTy() && paramType->isDoubleTy()) {
                maxVectorArgType = args[i]->getType();
                argumentIsVectorAndNeedsDistribution[i] = 1;
            }
        }
    }

    if (!maxVectorArgType)  // nothing needs distribution so just execute normally
        return executeStandardFunction(Builder, seFuncType, args, addrVal);

    assert(maxVectorArgType->isVectorTy());

    std::vector<LLVM_VALUE> ret;
    for (unsigned vecComponent = 0; vecComponent < maxVectorArgType->getVectorNumElements(); ++vecComponent) {
        LLVM_VALUE idx = ConstantInt::get(Type::getInt32Ty(llvmContext), vecComponent);
        std::vector<LLVM_VALUE> realArgs;
        // Break the function into multiple calls per component of the output
        // i.e. sin([1,2,3]) should be [sin(1),sin(2),sin(3)]
        for (unsigned argIndex = 0; argIndex < args.size(); ++argIndex) {
            LLVM_VALUE realArg = args[argIndex];
            if (argumentIsVectorAndNeedsDistribution[argIndex]) {
                if (args[argIndex]->getType()->isPointerTy())
                    realArg = Builder.CreateLoad(Builder.CreateConstGEP2_32(nullptr, args[argIndex], 0, vecComponent));
                else
                    realArg = Builder.CreateExtractElement(args[argIndex], idx);
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
    thenBlock = Builder.GetInsertBlock();

    Builder.SetInsertPoint(elseBlock);
    child(2)->codegen(Builder);
    elseBlock = Builder.GetInsertBlock();

    // make all the merged variables. in the if then basic blocks
    // this is because we need phi ops to be alone
    Builder.SetInsertPoint(phiBlock);
    const auto &merges = _varEnv->merge(_varEnvMergeIndex);
    std::vector<LLVM_VALUE> phis;
    phis.reserve(merges.size());
    for (auto &it : merges) {
        ExprLocalVarPhi *finalVar = it.second;
        if (finalVar->valid()) {
            ExprType refType = finalVar->type();
            Builder.SetInsertPoint(thenBlock);
            LLVM_VALUE thenValue = promoteOperand(Builder, refType, Builder.CreateLoad(finalVar->_thenVar->varPtr()));
            Builder.SetInsertPoint(elseBlock);
            LLVM_VALUE elseValue = promoteOperand(Builder, refType, Builder.CreateLoad(finalVar->_elseVar->varPtr()));

            Type *finalType = thenValue->getType();
            Builder.SetInsertPoint(phiBlock);
            PHINode *phi = Builder.CreatePHI(finalType, 2, it.first);
            phi->addIncoming(thenValue, thenBlock);
            phi->addIncoming(elseValue, elseBlock);
            phis.push_back(phi);
        }
    }
    // Now that we made all of the phi blocks, we must store them into the variables
    int idx = 0;
    for (auto &it : _varEnv->merge(_varEnvMergeIndex)) {
        const std::string &name = it.first;
        ExprLocalVarPhi *finalVar = it.second;
        if (finalVar->valid()) {
            LLVM_VALUE _finalVarPtr = finalVar->codegen(Builder, name + "-merge", phis[idx]);
            Builder.CreateStore(phis[idx++], _finalVarPtr);
        }
    }
    // Insert the ending jumps out of the then, else basic blocks
    Builder.SetInsertPoint(thenBlock);
    Builder.CreateBr(phiBlock);
    Builder.SetInsertPoint(elseBlock);
    Builder.CreateBr(phiBlock);
    // insert at end again
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
        Alloca->takeName(&*AI);
        Builder.CreateStore(&*AI, Alloca);
    }

    LLVM_VALUE result = 0;
    for (int i = 1; i < numChildren(); i++) result = child(i)->codegen(Builder);

    Builder.CreateRet(result);
    Builder.restoreIP(oldIP);
    return 0;
}

LLVM_VALUE ExprPrototypeNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    LLVMContext &llvmContext = Builder.getContext();

    // get arg type
    std::vector<Type *> ParamTys;
    for (int i = 0; i < numChildren(); ++i) ParamTys.push_back(createLLVMTyForSeExprType(llvmContext, argType(i)));
    // get ret type
    Type *retTy = createLLVMTyForSeExprType(llvmContext, returnType());

    FunctionType *FT = FunctionType::get(retTy, ParamTys, false);
    Function *F = Function::Create(FT, GlobalValue::InternalLinkage, name(), llvm_getModule(Builder));

    // Set names for all arguments.
    auto AI = F->arg_begin();
    for (int i = 0, e = numChildren(); i != e; ++i, ++AI) {
        const ExprVarNode *childNode = dynamic_cast<const ExprVarNode *>(child(i));
        assert(childNode);
        AI->setName(childNode->name());
    }

    return F;
}

LLVM_VALUE ExprStrNode::codegen(LLVM_BUILDER Builder) LLVM_BODY { return Builder.CreateGlobalStringPtr(_str); }

LLVM_VALUE ExprSubscriptNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    LLVM_VALUE op1 = child(0)->codegen(Builder);
    LLVM_VALUE op2 = child(1)->codegen(Builder);

    if (op1->getType()->isDoubleTy()) return op1;

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

/// Visitor pattern for VarCodeGeneration to make ExprVarNode behave more like a delegation
struct VarCodeGeneration {
    static LLVM_VALUE codegen(ExprVarRef *varRef, const std::string &varName, LLVM_BUILDER Builder) {
        LLVMContext &llvmContext = Builder.getContext();

        // a few types
        Type *int64Ty           = Type::getInt64Ty(llvmContext);    // int64_t
        Type *doubleTy          = Type::getDoubleTy(llvmContext);   // double
        PointerType *int8PtrTy  = Type::getInt8PtrTy(llvmContext);  // char *

        // get var informations
        bool isDouble = varRef->type().isFP();
        int dim = varRef->type().dim();

        // create the return value on the stack
        AllocaInst *returnValue = createAllocaInst(Builder, isDouble ? doubleTy : int8PtrTy, dim);

        // get our eval var function, and call it with a pointer to our var ref and a ref to the return value
        Function *evalVarFunc = llvm_getModule(Builder)->getFunction(isDouble == true ? "SeExpr2LLVMEvalFPVarRef" : "SeExpr2LLVMEvalStrVarRef");
        Builder.CreateCall(evalVarFunc, {
           Builder.CreateIntToPtr(ConstantInt::get(int64Ty, (uint64_t)varRef), int8PtrTy),
           returnValue
        });

        // load our return value
        LLVM_VALUE ret = 0;
        if (dim == 1) {
            ret = Builder.CreateLoad(returnValue);
        } else {
            // TODO: I don't really see how this requires dim==3... this assert should be removable
            assert(dim == 3 && "future work.");
            ret = createVecValFromAlloca(Builder, returnValue, dim);
        }

        AllocaInst *thisvar = createAllocaInst(Builder, ret->getType(), 1, varName);
        Builder.CreateStore(ret, thisvar);
        return ret;
    }

    static LLVM_VALUE codegen(VarBlockCreator::Ref *varRef, const std::string &varName, LLVM_BUILDER Builder) {
        LLVMContext &llvmContext = Builder.getContext();

        int variableOffset = varRef->offset();
        int variableStride = varRef->stride();
        Function *function = llvm_getFunction(Builder);
        auto argIterator = function->arg_begin();
        argIterator++;  // skip first arg
        llvm::Argument *variableBlock = &*(argIterator++);
        llvm::Argument *indirectIndex = &*(argIterator++);

        int dim = varRef->type().dim();

        Type *ptrToPtrTy = variableBlock->getType();
        Value *variableBlockAsPtrPtr = Builder.CreatePointerCast(variableBlock, ptrToPtrTy);
        Value *variableOffsetIndex = ConstantInt::get(Type::getInt32Ty(llvmContext), variableOffset);
        Value *variableBlockIndirectPtrPtr = Builder.CreateInBoundsGEP(variableBlockAsPtrPtr, variableOffsetIndex);
        Value *baseMemory = Builder.CreateLoad(variableBlockIndirectPtrPtr);
        Value *variableStrideValue = ConstantInt::get(Type::getInt32Ty(llvmContext), variableStride);
        if (dim == 1) {
            /// If we are uniform always assume indirectIndex is 0 (there's only one value)
            Value *variablePointer =
                varRef->type().isLifetimeUniform() ? baseMemory : Builder.CreateInBoundsGEP(baseMemory, indirectIndex);
            return Builder.CreateLoad(variablePointer);
        } else {
            std::vector<Value *> loadedValues(3);
            for (int component = 0; component < dim; component++) {
                Value *componentIndex = ConstantInt::get(Type::getInt32Ty(llvmContext), component);
                /// If we are uniform always assume indirectIndex is 0 (there's only one value)
                Value *variablePointer =
                    varRef->type().isLifetimeUniform()
                        ? Builder.CreateInBoundsGEP(Type::getDoubleTy(llvmContext), baseMemory, componentIndex)
                        : Builder.CreateInBoundsGEP(
                              Type::getDoubleTy(llvmContext),
                              baseMemory,
                              Builder.CreateAdd(Builder.CreateMul(indirectIndex, variableStrideValue), componentIndex));
                loadedValues[component] = Builder.CreateLoad(variablePointer, varName);
            }
            return createVecVal(Builder, loadedValues, varName);
        }
    }
};

// This is the use of def-use chain
LLVM_VALUE ExprVarNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    if (_var) {
        // All external var has the prefix "external_" in current function to avoid
        // potential name conflict with local variable
        std::string varName("external_");
        varName.append(name());
        // if (LLVM_VALUE valPtr = resolveLocalVar(varName.c_str(), Builder))
        //     return Builder.CreateLoad(valPtr);
        if (VarBlockCreator::Ref *varBlockRef = dynamic_cast<VarBlockCreator::Ref *>(_var))
            return VarCodeGeneration::codegen(varBlockRef, varName, Builder);
        else
            return VarCodeGeneration::codegen(_var, varName, Builder);
    } else if (_localVar) {
        ExprType varTy = _localVar->type();
        if (varTy.isFP() || varTy.isString()) {
            // LLVM_VALUE valPtr = resolveLocalVar(name(), Builder);
            LLVM_VALUE varPtr = _localVar->varPtr();
            assert(varPtr && "can not found symbol?");
            return Builder.CreateLoad(varPtr);
        }
    }

    assert(false);
    return 0;
}

LLVM_VALUE ExprVecNode::codegen(LLVM_BUILDER Builder) LLVM_BODY {
    std::vector<LLVM_VALUE> elems;
    ConstantInt *zero = ConstantInt::get(Type::getInt32Ty(Builder.getContext()), 0);
    for (int i = 0; i < numChildren(); i++) {
        LLVM_VALUE val = child(i)->codegen(Builder);
        elems.push_back(val->getType()->isVectorTy() ? Builder.CreateExtractElement(val, zero) : val);
    }
    return createVecVal(Builder, elems);
}
}

#endif
