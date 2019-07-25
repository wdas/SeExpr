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

#include <deque>
#include <sstream>
#include <unordered_map>
#include <vector>

// this potentially sets SEEXPR_ENABLE_FOLLY
#include "ExprConfig.h"

#ifdef SEEXPR_ENABLE_FOLLY
#include <folly/Function.h>
#else
#include <functional>
#endif

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
    VarBlock(const VarBlockCreator* creator, int size, bool makeThreadSafe)
        : indirectIndex(0), threadSafe(makeThreadSafe), _creator(creator)
    {
        _dataPtrs.resize(size, 0);
    }

  public:
    friend class VarBlockCreator;

    /// Move semantics is the only allowed way to change the structure
    VarBlock(VarBlock&& other)
    {
        indirectIndex = other.indirectIndex;
        threadSafe = other.threadSafe;
        d = std::move(other.d);
        s = std::move(other.s);
        _dataPtrs = std::move(other._dataPtrs);
        _creator = other._creator;
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

    /// if true, interpreter's data will be copied to this instance before evaluation.
    bool threadSafe;

    /// copy of Interpreter's double data
    std::vector<double> d;

    /// copy of Interpreter's str data
    std::vector<char*> s;

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

// helper class for using VarBlocks
class SymbolTable : public VarBlock {
  public:
#ifdef SEEXPR_ENABLE_FOLLY
    typedef folly::Function<void(SeExpr2::ExprFuncSimple::ArgHandle&) const> FunctionCodeStorage;
    typedef folly::Function<void(double*) const> DeferredVarStorage;
#else
    typedef std::function<void(SeExpr2::ExprFuncSimple::ArgHandle&)> FunctionCodeStorage;
    typedef std::function<void(double*)> DeferredVarStorage;
#endif

    class DeferredVarRef : public ExprVarRef {
      public:
        DeferredVarRef(uint32_t offset_, const VarBlockCreator* creator_)
            : ExprVarRef(ExprType().Error()), offset(offset_), callable(), _creator(creator_)
        {
        }

        virtual ExprType type() const override;

        void eval(double* result) override
        {
            callable(result);
        }

        void eval(const char**) override
        {
            throw "DeferredVarRef not implemented for strings";
        }

        uint32_t offset;
        DeferredVarStorage callable;

      private:
        const VarBlockCreator* _creator;
    };

  public:
    explicit SymbolTable(VarBlock&& block)
        : VarBlock(std::move(block)), _stack(), _deferred_vars(), _allocations(VarBlock::numSymbols(), nullptr)
    {
        _deferred_vars.reserve(VarBlock::numSymbols());
        for (size_t i = 0; i < VarBlock::numSymbols(); ++i) {
            _deferred_vars.push_back(DeferredVarRef(i, _creator));
        }
    }

    SymbolTable(SymbolTable&& other)
        : VarBlock(std::move(other))
        , _stack()
        , _function_code_segments(std::move(other._function_code_segments))
        , _deferred_vars(std::move(other._deferred_vars))
        , _allocations(std::move(other._allocations))
    {
    }

    SymbolTable& operator=(SymbolTable&& other)
    {
        _stack = std::move(other._stack);
        _function_code_segments = std::move(other._function_code_segments);
        return *this;
    }

    SymbolTable(const SymbolTable&) = delete;
    SymbolTable& operator=(const SymbolTable&) = delete;

    virtual ~SymbolTable()
    {
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
        return *alloc(offset, 1);
    }

    // dumb helper for integrations in apps that couldn't be bothered to have clean ownership of
    // their own variables with a guaranteed lifetime
    template <int d>
    Vec<double, d, true> Vector(uint32_t offset)
    {
        return Vec<double, d, true>(alloc(offset, d));
    }

    // Set code segment for some Function declared in the VarBlockCreator
    DeferredVarStorage& DeferredVar(uint32_t offset)
    {
        assert(offset < numSymbols() && "SymbolTable index out of bounds");
        Pointer(offset) = (double*)&_deferred_vars[offset];
        return _deferred_vars[offset].callable;
    }

  private:
    double* alloc(uint32_t offset, size_t doublesNeeded)
    {
        assert(doublesNeeded);

        if (_allocations[offset])
            return _allocations[offset];

        size_t idx = _stack.size();
        _stack.resize(_stack.size() + doublesNeeded);
        double* ptr = &_stack[idx];
        Pointer(offset) = ptr;
        _allocations[offset] = ptr;
        return ptr;
    }

    std::deque<double> _stack;
    std::vector<std::unique_ptr<SeExpr2::ExprFuncX>> _function_code_segments;
    std::vector<DeferredVarRef> _deferred_vars;
    std::vector<double*> _allocations;
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

        inline virtual ExprFuncNode::Data* evalConstant(const ExprFuncNode* node, ArgHandle& args) const override
        {
            const ExprFuncSimple* funcsimple = funcSimple(args);
            return funcsimple ? funcsimple->evalConstant(node, args) : nullptr;
        }

        inline virtual void eval(ArgHandle& args) override
        {
            ExprFuncSimple* funcsimple = funcSimple(args);
            if (funcsimple)
                funcsimple->eval(args);
        }

      private:
        const ExprFuncSimple* funcSimple(ArgHandle& args) const
        {
            assert(args.varBlock);
            const ExprFuncSimple** funcs = reinterpret_cast<const ExprFuncSimple**>(const_cast<char*>(args.varBlock));
            assert(funcs);
            return funcs[offset()];
        }

        ExprFuncSimple* funcSimple(ArgHandle& args)
        {
            assert(args.varBlock);
            ExprFuncSimple** funcs = reinterpret_cast<ExprFuncSimple**>(const_cast<char*>(args.varBlock));
            assert(funcs);
            return funcs[offset()];
        }

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
            _funcs.emplace(name, FuncSymbol(decl, offset));
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
    /// \param makeThreadSafe
    ///     If true, right before evaluating the expression, all data used
    ///     by the interpreter will be copied to the var block, to make the
    ///     evaluation thread safe (assuming there's one var block instead
    ///     per thread)
    ///     If false or not specified, the old behavior occurs (var block
    ///     will only hold variables sources and optionally output data,
    ///     and the interpreter will work on its internal data)
    VarBlock create(bool makeThreadSafe = false)
    {
        return VarBlock(this, _offset, makeThreadSafe);
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
    std::map<std::string, FuncSymbol> _funcs;
};

inline ExprType SymbolTable::DeferredVarRef::type() const
{
    return _creator->getVar(offset)->type();
};

}  // namespace

#endif
