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
#ifndef VarBlock_h
#define VarBlock_h

#include <functional>
#include <sstream>
#include <vector>
#include <unordered_map>

#include "Expression.h"
#include "ExprType.h"
#include "ExprFunc.h"
#include "ExprFuncX.h"
#include "Vec.h"

namespace SeExpr2 {

class Expression;
class ExprNode;
class ExprVarNode;
class Interpreter;

class VarBlockCreator;

/// A thread local evaluation context. Just allocate and fill in with data.
class VarBlock {
  private:
    /// Allocate an VarBlock
    VarBlock(const VarBlockCreator* creator, int size) : indirectIndex(0), _creator(creator)
    {
        _dataPtrs.resize(size, 0);
    }

  public:
    friend class VarBlockCreator;

    /// Move semantics is the only allowed way to change the structure
    VarBlock(VarBlock&& other)
        : indirectIndex(std::move(other.indirectIndex)), _creator(other._creator), _dataPtrs(std::move(other._dataPtrs))
    {
    }

    virtual ~VarBlock()
    {
    }

    /// Don't allow copying and operator='ing'
    VarBlock(const VarBlock&) = delete;
    VarBlock& operator=(const VarBlock&) = delete;

    inline size_t numSymbols() const
    {
        return _dataPtrs.size();
    }

    /// Get a reference to the data block pointer which can be modified
    const double*& Pointer(uint32_t offset)
    {
        return reinterpret_cast<const double*&>(_dataPtrs[offset]);
    }
    const char*& CharPointer(uint32_t offset)
    {
        return reinterpret_cast<const char*&>(_dataPtrs[offset]);
    }
    const ExprFuncX*& Function(uint32_t offset)
    {
        return reinterpret_cast<const ExprFuncX*&>(_dataPtrs[offset]);
    }

    /// indirect index to add to pointer based data
    // i.e.  _dataPtrs[someAttributeOffset][indirectIndex]
    int indirectIndex;

    /// Raw data of the data block pointer (used by compiler)
    const char** data()
    {
        return _dataPtrs.data();
    }

    void validate()
    {
        for (auto ptr : _dataPtrs) {
            (void)ptr;
            assert(ptr && "VarBlock has undefined data pointer!");
        }
    }

  protected:
    const VarBlockCreator* _creator;

  private:
    /// This stores double* or char** ptrs
    std::vector<const char*> _dataPtrs;
};

#ifdef DEBUG
#define VALIDATE_VARBLOCK(varblock) \
    if (varblock)                   \
        varblock->validate();
#else
#define VALIDATE_VARBLOCK(varblock)
#endif

// helper class for using VarBlocks
template <typename FunctionCodeStorage = std::function<void(SeExpr2::ExprFuncSimple::ArgHandle)>, typename DeferredVarStorage = std::function<void(double*)>>
class SymbolTable : public VarBlock {
    struct DeferredVarRef : public ExprVarRef {

      public:
        DeferredVarRef(const ExprType& type_) : ExprVarRef(type_)
        {
        }

        void eval(double* result) override
        {
            callable(result);
        }

        void eval(const char**) override
        {
            throw "DeferredVarRef not implemented for strings";
        }

        DeferredVarStorage callable;
    };

  public:
    explicit SymbolTable(VarBlock&& block) : VarBlock(std::move(block))
    {
    }

    SymbolTable(SymbolTable&& other)
        : _allocations(std::move(other._allocations)), _function_code_segments(std::move(other._function_code_segments))
    {
    }

    SymbolTable& operator=(SymbolTable&& other)
    {
        _allocations = std::move(other._allocations);
        _function_code_segments = std::move(other._function_code_segments);
    }

    SymbolTable(const SymbolTable&) = delete;
    SymbolTable& operator=(const SymbolTable&) = delete;

    virtual ~SymbolTable()
    {
        for (auto pair : _allocations)
            free(pair.second);
    }

    // Set code segment for some Function declared in the VarBlockCreator
    void FunctionCode(uint32_t offset, FunctionCodeStorage&& f)
    {
        _function_code_segments.emplace_back(new SeExpr2::ExprFuncClosure<FunctionCodeStorage>(std::move(f)));
        Function(offset) = _function_code_segments.back().get();
    }

    // Set code segment for some Function declared in the VarBlockCreator
    void FunctionCode(uint32_t offset, std::unique_ptr<SeExpr2::ExprFuncX> funcx)
    {
        _function_code_segments.emplace_back(std::move(funcx));
        Function(offset) = _function_code_segments.back().get();
    }

    // dumb helper for integrations in apps that couldn't be bothered to have clean ownership of
    // their own variables with a guaranteed lifetime
    double& Scalar(uint32_t offset)
    {
        return *alloc(offset, sizeof(double));
    }

    // dumb helper for integrations in apps that couldn't be bothered to have clean ownership of
    // their own variables with a guaranteed lifetime
    template <int d>
    Vec<double, d, true> Vector(uint32_t offset)
    {
        return Vec<double, d, true>(alloc(offset, d * sizeof(double)));
    }

    // Set code segment for some Function declared in the VarBlockCreator
    DeferredVarStorage& DeferredVar(uint32_t offset);

  private:
    // TODO: small object optimization
    double* alloc(uint32_t offset, size_t bytes)
    {
        auto iter = _allocations.find(offset);
        if (iter != _allocations.end())
            return iter->second;

        double* ptr = (double*)malloc(bytes);
        _allocations[offset] = ptr;
        Pointer(offset) = ptr;
        return ptr;
    }

    std::unordered_map<uint32_t, double*> _allocations;
    std::vector<std::unique_ptr<SeExpr2::ExprFuncX>> _function_code_segments;
    std::unordered_map<uint32_t, DeferredVarRef> _deferred_vars;
};

/// A class that lets you register for the variables used by one or more expressions
// This does not register actual data only types of the data. It can create
// a VarBlock which allows registering actual variable data
class VarBlockCreator {
  public:
    /// Internally implemented var ref used by SeExpr
    class Ref : public ExprVarRef {
        uint32_t _offset;
        uint32_t _stride;

      public:
        Ref(const ExprType& type, uint32_t offset, uint32_t stride) : ExprVarRef(type), _offset(offset), _stride(stride)
        {
        }

        uint32_t offset() const
        {
            return _offset;
        }
        uint32_t stride() const
        {
            return _stride;
        }

        void eval(double*) override
        {
            assert(false);
        }
        void eval(const char**) override
        {
            assert(false);
        }
    };

    class DeferredRef : public ExprVarRef {
        uint32_t _offset;
        uint32_t _stride;

      public:
        DeferredRef(const ExprType& type, uint32_t offset, uint32_t stride)
            : ExprVarRef(type), _offset(offset), _stride(stride)
        {
        }

        uint32_t offset() const
        {
            return _offset;
        }
        uint32_t stride() const
        {
            return _stride;
        }

        void eval(double*) override
        {
            assert(false);
        }
        void eval(const char**) override
        {
            assert(false);
        }
    };

    class FuncSymbol : public ExprFuncSimple {
      public:
        FuncSymbol(const ExprFuncDeclaration& decl, uint32_t offset_)
            : ExprFuncSimple(true), _decl(decl), _offset(offset_), _func(*this, decl.minArgs, decl.maxArgs)
        {
            assert(_decl.types.size() && "FuncSymbol missing type information");
        }

        FuncSymbol(const FuncSymbol&) = delete;
        FuncSymbol& operator=(const FuncSymbol&) = delete;

        FuncSymbol(FuncSymbol&& other)
            : ExprFuncSimple(true)
            , _decl(std::move(other._decl))
            , _offset(std::move(other._offset))
            , _func(*this, other._func.minArgs(), other._func.maxArgs())
        {
            assert(_decl.types.size() && "FuncSymbol missing type information");
        }

        inline uint32_t offset() const
        {
            return _offset;
        }
        inline ExprFunc& func() const
        {
            return _func;
        }

        inline ExprFuncDeclaration& signature()
        {
            return _decl;
        }
        inline const ExprFuncDeclaration& signature() const
        {
            return _decl;
        }

        virtual ExprType type() const override
        {
            return _type;
        }
        virtual ExprType prep(ExprFuncNode* node, bool scalarWanted, ExprVarEnvBuilder& env) const override
        {
            assert(_decl.types.size() && "FuncSymbol missing type information");
            return ExprFuncSimple::genericPrep(node, scalarWanted, env, _decl);
        }

        virtual ExprFuncNode::Data* evalConstant(const ExprFuncNode* node, ArgHandle args) const override
        {
            assert(args.varBlock);
            ExprFuncSimple** funcs = reinterpret_cast<ExprFuncSimple**>(const_cast<char*>(args.varBlock));
            assert(funcs);
            ExprFuncSimple* funcsimple = funcs[offset()];
            return funcsimple ? funcsimple->evalConstant(node, args) : nullptr;
        }

        virtual void eval(ArgHandle args) override
        {
            assert(args.varBlock);
            ExprFuncSimple** funcs = reinterpret_cast<ExprFuncSimple**>(const_cast<char*>(args.varBlock));
            assert(funcs);
            ExprFuncSimple* funcsimple = funcs[offset()];
            if (funcsimple)
                funcsimple->eval(args);
        }

      private:
        ExprFuncDeclaration _decl;
        uint32_t _offset;
        mutable ExprFunc _func;
    };

    /// Register a variable and return a handle
    int registerVariable(const std::string& name, const ExprType& type)
    {
        if (_vars.find(name) != _vars.end()) {
            throw std::runtime_error("Already registered a variable named " + name);
        } else {
            int offset = _offset++;
            auto iter = _vars.insert(std::make_pair(name, Ref(type, offset, type.dim())));
            _varPtrs[offset] = &iter.first->second;
            return offset;
        }
    }

    int registerDeferredVariable(const std::string& name, const ExprType& type)
    {
        if (_deferredVars.find(name) != _deferredVars.end()) {
            throw std::runtime_error("Already registered a variable named " + name);
        } else {
            int offset = _offset++;
            auto iter = _deferredVars.insert(std::make_pair(name, DeferredRef(type, offset, type.dim())));
            _varPtrs[offset] = &iter.first->second;
            return offset;
        }
    }

    int registerFunction(const std::string& name, const ExprFuncDeclaration& decl)
    {
        if (_funcs.find(name) != _funcs.end()) {
            throw std::runtime_error("Already registered a function named " + name);
        } else {
            int offset = _offset++;
            auto iter = _funcs.emplace(name, FuncSymbol(decl, offset));
            _funcPtrs[offset] = &iter.first->second;
            return offset;
        }
    }

    int registerFunction(const std::string& name, int minArgs, int maxArgs, const std::vector<ExprType>& types)
    {
        return registerFunction(name, ExprFuncDeclaration{minArgs, maxArgs, types});
    }

    inline size_t numVariables() const
    {
        return _vars.size();
    }
    inline size_t numFunctions() const
    {
        return _funcs.size();
    }

    /// Get an evaluation handle (one needed per thread)
    VarBlock create() const
    {
        return VarBlock(this, _offset);
    }

    /// Resolve the variable using anything in the data block (call from resolveVar in Expr subclass)
    ExprVarRef* resolveVar(const std::string& name) const
    {
        {
            auto it = _vars.find(name);
            if (it != _vars.end())
                return const_cast<Ref*>(&it->second);
        }
        {
            auto it = _deferredVars.find(name);
            if (it != _deferredVars.end())
                return const_cast<DeferredRef*>(&it->second);
        }
        return nullptr;
    }

    ExprFunc* resolveFunc(const std::string& name) const
    {
        auto it = _funcs.find(name);
        if (it != _funcs.end())
            return &it->second.func();
        return nullptr;
    }

    const ExprVarRef* getVar(int offset) const
    {
        return _varPtrs.at(offset);
    }

    const ExprFuncX* getFunc(int offset) const
    {
        return _funcPtrs.at(offset);
    }

    void dump() const
    {
        for (const auto& pair : _vars) {
            printf("[%4d] %s :: ", pair.second.offset(), pair.first.c_str());
            std::cout << pair.second.type().toString() << std::endl;
        }
        for (const auto& pair : _funcs) {
            printf("[%4d] %s() :: ", pair.second.offset(), pair.first.c_str());
            const ExprFuncDeclaration& signature = pair.second.signature();
            for (size_t i = 0; i < signature.types.size(); ++i) {
                if (i > 0)
                    std::cout << " -> ";
                std::cout << signature.types[i].toString();
            }
            std::cout << std::endl;
        }
    }

  private:
    int _offset = 0;  // shared offset domain between vars and funcs
    std::map<int, const ExprVarRef*> _varPtrs;
    std::map<std::string, Ref> _vars;
    std::map<std::string, DeferredRef> _deferredVars;
    std::map<int, const ExprFuncX*> _funcPtrs;
    std::map<std::string, FuncSymbol> _funcs;
};

template <typename FunctionCodeStorage, typename DeferredVarStorage>
DeferredVarStorage& SymbolTable<FunctionCodeStorage, DeferredVarStorage>::DeferredVar(uint32_t offset)
{
    auto iter = _deferred_vars.find(offset);
    if (iter != _deferred_vars.end())
        return iter->second.callable;

    auto pair = _deferred_vars.emplace(offset, _creator->getVar(offset)->type());
    Pointer(offset) = (double*)&pair.first->second;
    return pair.first->second.callable;
}

}  // namespace

#endif
