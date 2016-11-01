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

#include "Expression.h"
#include "ExprType.h"
#include "Vec.h"

namespace SeExpr2 {

class ExprNode;
class ExprVarNode;
class ExprFunc;
class Expression;
class Interpreter;

class VarBlockCreator;

/// A thread local evaluation context. Just allocate and fill in with data.
class VarBlock {
  private:
    /// Allocate an VarBlock
    VarBlock(int size) : indirectIndex(0) { _dataPtrs.resize(size); }

  public:
    friend class VarBlockCreator;

    /// Move semantics is the only allowed way to change the structure
    VarBlock(VarBlock&& other) { _dataPtrs = std::move(other._dataPtrs); }

    ~VarBlock() {}

    /// Don't allow copying and operator='ing'
    VarBlock(const VarBlock&) = delete;
    VarBlock& operator=(const VarBlock&) = delete;

    /// Get a reference to the data block pointer which can be modified
    double*& Pointer(uint32_t variableOffset) { return reinterpret_cast<double*&>(_dataPtrs[variableOffset]); }
    char**& CharPointer(uint32_t variableOffset) { return reinterpret_cast<char**&>(_dataPtrs[variableOffset]); }

    /// indirect index to add to pointer based data
    // i.e.  _dataPtrs[someAttributeOffset][indirectIndex]
    int indirectIndex;

    /// Raw data of the data block pointer (used by compiler)
    char** data() { return _dataPtrs.data(); }

  private:
    /// This stores double* or char** ptrs
    std::vector<char*> _dataPtrs;
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
        uint32_t offset() const { return _offset; }
        uint32_t stride() const { return _stride; }
        Ref(const ExprType& type, uint32_t offset, uint32_t stride)
            : ExprVarRef(type), _offset(offset), _stride(stride) {}
        void eval(double*) override { assert(false); }
        void eval(const char**) override { assert(false); }
    };

    /// Register a variable and return a handle
    int registerVariable(const std::string& name, const ExprType type) {
        if (_vars.find(name) != _vars.end()) {
            throw std::runtime_error("Already registered a variable named " + name);
        } else {
            int offset = _nextOffset;
            _nextOffset += 1;
            _vars.insert(std::make_pair(name, Ref(type, offset, type.dim())));
            return offset;
        }
    }

    /// Get an evaluation handle (one needed per thread)
    VarBlock create() { return VarBlock(_nextOffset); }

    /// Resolve the variable using anything in the data block (call from resolveVar in Expr subclass)
    ExprVarRef* resolveVar(const std::string& name) const {
        auto it = _vars.find(name);
        if (it != _vars.end()) return const_cast<Ref*>(&it->second);
        return nullptr;
    }

  private:
    int _nextOffset = 0;
    std::map<std::string, Ref> _vars;
};

}  // namespace

#endif
