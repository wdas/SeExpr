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
#ifndef ExprEnv_h
#define ExprEnv_h

#include <vector>
#include <map>
#include <cassert>
#include <memory>

#include "ExprType.h"
#include "ExprLLVM.h"
#include <iostream>

namespace SeExpr2 {
class ExprVarRef;
class ExprLocalVar;
class ExprNode;
class ExprLocalFunctionNode;
class Interpreter;

//! ExprLocalVar reference, all local variables in seexpr are subclasses of this or this itself
class ExprLocalVar {
  protected:
    ExprType _type;
    ExprLocalVar* _phi;
    mutable LLVM_VALUE _varPtr;

  public:
    ExprLocalVar(const ExprType& type) : _type(type), _phi(0), _varPtr(0) {}

    virtual ~ExprLocalVar() {}

    //! get the primary representative phi node (i.e. the global parent of a dependent phi node)
    const ExprLocalVar* getPhi() const { return _phi; }
    //! returns type of the variable
    ExprType type() const { return _type; }
    //! sets the representative phi node (like a brute force set unioning operation) phi is the set representative
    virtual void setPhi(ExprLocalVar* phi) { _phi = phi; }

    //! LLVM value that has been allocated
    virtual LLVM_VALUE codegen(LLVM_BUILDER, const std::string& name, LLVM_VALUE referenceType) LLVM_BODY;

    //! LLVM value that has been pre-done
    virtual LLVM_VALUE varPtr() { return _varPtr; }

    //! Allocates variable for interpreter
    int buildInterpreter(Interpreter* interpreter) const;
};

//! ExprLocalVar join (merge) references. Remembers which variables are possible assigners to this
// This is basically like single assignment form inspired. hence the phi node nomenclature.
class ExprLocalVarPhi : public ExprLocalVar {
  public:
    ExprLocalVarPhi(ExprType condLife, ExprLocalVar* thenVar, ExprLocalVar* elseVar)
        : ExprLocalVar(ExprType()), _thenVar(thenVar), _elseVar(elseVar) {
        // find the compatible common-denominator lifetime
        ExprType firstType = _thenVar->type(), secondType = _elseVar->type();
        if (ExprType::valuesCompatible(_thenVar->type(), _elseVar->type())) {
            _type = ((firstType.isFP(1) ? secondType : firstType).setLifetime(firstType, secondType));
        }
        // lifetime should be the minimum (error=0,varying=1,uniform=2,constant=3).
        // i.e. you can only guarantee something is constant if the condition, ifvar, and else var are the same
        _type.setLifetime(firstType, secondType, condLife);
    }

    bool valid() const { return !_type.isError(); }

    void setPhi(ExprLocalVar* phi) {
        _phi = phi;
        _thenVar->setPhi(phi);
        _elseVar->setPhi(phi);
    }

    ExprNode* _condNode;
    ExprLocalVar* _thenVar, *_elseVar;
};

//! Variable scope for tracking variable lookup
class ExprVarEnv {
  private:
    typedef std::map<std::string, std::unique_ptr<ExprLocalVar>> VarDictType;
    VarDictType _map;
    typedef std::map<std::string, ExprLocalFunctionNode*> FuncDictType;
    FuncDictType _functions;

    //! Variables that have been superceded (and thus are inaccessible)
    // i.e. a=3;a=[1,2,3];a=[2];a will  yield 2 entries in shadowedVariables
    std::vector<std::unique_ptr<ExprLocalVar>> shadowedVariables;

    //! Keep track of all merged variables in
    std::vector<std::vector<std::pair<std::string, ExprLocalVarPhi*>>> _mergedVariables;

    //! Parent variable environment has all variablesf rom previou scope (for lookup)
    ExprVarEnv* _parent;

  protected:
    ExprVarEnv(ExprVarEnv& other);
    ExprVarEnv& operator=(ExprVarEnv& other);

  public:
    // TODO: figure out when anotherOwns is needed
    //! Create a scope with no parent
    ExprVarEnv() : _parent(0) {};

    ~ExprVarEnv();

    //! Resets the scope (deletes all variables) and sets parent
    void resetAndSetParent(ExprVarEnv* parent);
    //! Find a function by name (recursive to parents)
    ExprLocalFunctionNode* findFunction(const std::string& name);
    //! Find a variable name by name (recursive to parents)
    ExprLocalVar* find(const std::string& name);
    //! Find a const variable reference name by name (recursive to parents)
    ExprLocalVar const* lookup(const std::string& name) const;
    //! Add a function
    void addFunction(const std::string& name, ExprLocalFunctionNode* prototype);
    //! Add a variable refernece
    void add(const std::string& name, std::unique_ptr<ExprLocalVar> var);
    //! Add all variables into scope by name, but modify their lifetimes to the given type's lifetime
    //    void add(ExprVarEnv & env,const ExprType & modifyingType);
    //! Checks if each branch shares the same items and the same types!
    // static bool branchesMatch(const ExprVarEnv & env1, const ExprVarEnv & env2);
    size_t mergeBranches(const ExprType& type, ExprVarEnv& env1, ExprVarEnv& env2);
    // Code generate merges.
    LLVM_VALUE codegenMerges(LLVM_BUILDER builder, int mergeIndex) LLVM_BODY;
    // Query merges
    std::vector<std::pair<std::string, ExprLocalVarPhi*>>& merge(size_t index) { return _mergedVariables[index]; }
};

//! Variable scope builder is used by the type checking and code gen to track visiblity of variables and changing of
//scopes
// It is inspired by IRBuilder's notion of a basic block insertion point
class ExprVarEnvBuilder {
  public:
    //! Creates an empty builder with one current scope entry
    ExprVarEnvBuilder() { reset(); }
    //! Reset to factory state (one empty environment that is current)
    void reset() {
        std::unique_ptr<ExprVarEnv> newEnv(new ExprVarEnv);
        _currentEnv = newEnv.get();
        all.emplace_back(std::move(newEnv));
    }
    //! Return the current variable scope
    ExprVarEnv* current() { return _currentEnv; }
    //! Set a new current variable scope
    void setCurrent(ExprVarEnv* env) { _currentEnv = env; }
    //! Create a descendant scope from the provided parent, does not clobber current
    ExprVarEnv* createDescendant(ExprVarEnv* parent) {
        std::unique_ptr<ExprVarEnv> newEnv(new ExprVarEnv);
        newEnv->resetAndSetParent(parent);
        all.emplace_back(std::move(newEnv));
        return all.back().get();
    }

  private:
    //! All owned symbol tables
    std::vector<std::unique_ptr<ExprVarEnv>> all;
    //! The current symbol table (should be a pointer owned by all)
    ExprVarEnv* _currentEnv;
};

//! Evaluation result.
struct ExprEvalResult {
    ExprEvalResult() : n(0), fp(0), str(0) {}
    ExprEvalResult(int n, double* fp) : n(n), fp(fp), str(0) {}
    ExprEvalResult(const char** c) : n(1), fp(0), str(c) {}
    ExprEvalResult(int n, double* fp, const char** c) : n(n), fp(fp), str(c) {}

    int n;
    double* fp;
    const char** str;
};
}
#endif
