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

#include <cstdint>

#include "Evaluator.h"
#include "ExprConfig.h"
#include "ExprLLVMAll.h"
#include "VarBlock.h"
#include "ExprNode.h"

extern "C" void SeExpr2LLVMEvalVarRef(SeExpr2::ExprVarRef* seVR, double* result);
extern "C" void SeExpr2LLVMEvalCustomFunction(const int* opDataArg,
                                              double* fpArg,
                                              char** strArg,
                                              void** funcdata,
                                              const SeExpr2::ExprFuncNode* node,
                                              double** varBlockData);

namespace SeExpr2 {
#ifdef SEEXPR_ENABLE_LLVM

LLVM_VALUE promoteToDim(LLVM_VALUE val, unsigned dim, llvm::IRBuilder<>& Builder);

class LLVMEvaluator : public Evaluator {
    // TODO: this seems needlessly complex, let's fix it
    // TODO: let the dev code allocate memory?
    // FP is the native function for this expression.
    template <class T>
    class LLVMEvaluationContext {
      private:
        typedef void (*FunctionPtr)(T*, const char**, uint32_t);
        typedef void (*FunctionPtrMultiple)(const char**, double*, uint32_t, uint32_t);
        FunctionPtr functionPtr;
        FunctionPtrMultiple functionPtrMultiple;

      public:
        LLVMEvaluationContext() : functionPtr(nullptr)
        {
        }
        ~LLVMEvaluationContext()
        {
        }

        LLVMEvaluationContext(const LLVMEvaluationContext&) = delete;
        LLVMEvaluationContext& operator=(const LLVMEvaluationContext&) = delete;

        void init(void* fp, void* fpLoop, int)
        {
            reset();
            functionPtr = reinterpret_cast<FunctionPtr>(fp);
            functionPtrMultiple = reinterpret_cast<FunctionPtrMultiple>(fpLoop);
        }
        void reset()
        {
            functionPtr = nullptr;
            functionPtrMultiple = nullptr;
        }
        void operator()(T* dst, VarBlock* varBlock)
        {
            assert(functionPtr);
            functionPtr(dst, varBlock ? varBlock->data() : nullptr, varBlock ? varBlock->indirectIndex : 0);
        }
        void operator()(VarBlock* varBlock, double* outputBuffer, size_t rangeStart, size_t rangeEnd)
        {
            assert(functionPtrMultiple);
            functionPtrMultiple(varBlock ? varBlock->data() : nullptr, outputBuffer, rangeStart, rangeEnd);
        }
    };

    bool _debugging;

    std::unique_ptr<LLVMEvaluationContext<double>> _llvmEvalFP;
    std::unique_ptr<LLVMEvaluationContext<char*>> _llvmEvalStr;

    std::unique_ptr<llvm::LLVMContext> _llvmContext;
    std::unique_ptr<llvm::ExecutionEngine> TheExecutionEngine;

  public:
    LLVMEvaluator()
        : _debugging(false)
        , _llvmEvalFP(nullptr)
        , _llvmEvalStr(nullptr)
        , _llvmContext(nullptr)
        , TheExecutionEngine(nullptr)
    {
    }

    virtual void setDebugging(bool debugging) override
    {
        _debugging = debugging;
    }

    virtual inline void dump() const override
    {
        // TheModule->dump();
    }

    virtual bool prep(ExprNode* parseTree, ExprType desiredReturnType) override;

    virtual bool isValid() const override
    {
        return true;
    }

    virtual void evalFP(double* dst, VarBlock* varBlock) const override
    {
        (*_llvmEvalFP)(dst, varBlock);
    }

    virtual void evalStr(char* dst, VarBlock* varBlock) const override
    {
        (*_llvmEvalStr)(&dst, varBlock);
    }

    virtual void evalMultiple(VarBlock* varBlock,
                              double* outputBuffer,
                              size_t rangeStart,
                              size_t rangeEnd) const override
    {
        return (*_llvmEvalFP)(varBlock, outputBuffer, rangeStart, rangeEnd);
    }

    std::string getUniqueName() const
    {
        std::ostringstream o;
        o << std::setbase(16) << (uint64_t)(this);
        return ("_" + o.str());
    }
};

#else  // no LLVM support
class LLVMEvaluator : public Evaluator {
  public:
    void unsupported()
    {
        throw std::runtime_error("LLVM is not enabled in build");
    }

    virtual void setDebugging(bool debugging) override
    {
        unsupported();
    }

    virtual inline void dump() const override
    {
        unsupported();
    }

    virtual bool prep(ExprNode* parseTree, ExprType desiredReturnType) override
    {
        unsupported();
        return false;
    }

    virtual bool isValid() const override
    {
        unsupported();
        return false;
    }

    virtual const double* evalFP(VarBlock* varBlock) const override
    {
        unsupported();
        return 0;
    }

    virtual const char* evalStr(VarBlock* varBlock) const override
    {
        unsupported();
        return "";
    }

    virtual void evalMultiple(VarBlock* varBlock,
                              int outputVarBlockOffset,
                              size_t rangeStart,
                              size_t rangeEnd) const override
    {
        unsupported();
    }
};
#endif

}  // end namespace SeExpr2
