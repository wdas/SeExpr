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
#ifndef Expression_h
#define Expression_h

#include <string>
#include <map>
#include <set>
#include <vector>
#include <iomanip>
#include <stdint.h>
#include "ExprConfig.h"
#include "Vec.h"
#include "Context.h"
#include "ExprEnv.h"

namespace llvm {
class ExecutionEngine;
class LLVMContext;
}

namespace SeExpr2 {

class ExprNode;
class ExprVarNode;
class ExprFunc;
class Expression;
class Interpreter;

//! abstract class for implementing variable references
class ExprVarRef {
    ExprVarRef() : _type(ExprType().Error().Varying()) {};

  public:
    ExprVarRef(const ExprType& type) : _type(type) {};

    virtual ~ExprVarRef() {}

    //! sets (current) type to given type
    virtual void setType(const ExprType& type) {
        _type = type;
    };

    //! returns (current) type
    virtual ExprType type() const {
        return _type;
    };

    //! returns this variable's value by setting result
    virtual void eval(double* result) = 0;
    virtual void eval(const char** resultStr) = 0;

  private:
    ExprType _type;
};

class LLVMEvaluator;
class VarBlock;
class VarBlockCreator;

/// main expression class
class Expression {
  public:
    //! Types of evaluation strategies that are available
    enum EvaluationStrategy {
        UseInterpreter,
        UseLLVM
    };
    //! What evaluation strategy to use by default
    static EvaluationStrategy defaultEvaluationStrategy;
    //! Whether to debug expressions
    static bool debugging;

    // typedef std::map<std::string, ExprLocalVarRef> LocalVarTable;

    //! Represents a parse or type checking error in an expression
    struct Error {
        //! Text of error
        std::string error;

        //! Error start offset in expression string
        int startPos;

        //! Error end offset in expression string
        int endPos;

        Error(const std::string& errorIn, const int startPosIn, const int endPosIn)
            : error(errorIn), startPos(startPosIn), endPos(endPosIn) {}
    };

    Expression(EvaluationStrategy be = Expression::defaultEvaluationStrategy);
    Expression(const std::string& e,
               const ExprType& type = ExprType().FP(3),
               EvaluationStrategy be = Expression::defaultEvaluationStrategy,
               const Context& context = Context::global());

    virtual ~Expression();

    /** Sets desired return value.
        This will allow the evaluation to potentially be optimized. */
    void setDesiredReturnType(const ExprType& type);

    /** Set expression string to e.
        This invalidates all parsed state. */
    void setExpr(const std::string& e);

    //! Get the string that this expression is currently set to evaluate
    const std::string& getExpr() const { return _expression; }

    /** Check expression syntax.  Expr will be parsed if needed.  If
        this returns false, the error message can be accessed via
        parseError() */
    bool syntaxOK() const;

    /** Check if expression is valid.  Expr will be parsed if
    needed. Variables and functions will also be bound.  If this
    returns false, the error message can be accessed via
    parseError() */
    bool isValid() const {
        prepIfNeeded();
        return _isValid;
    }

    /** Get parse error (if any).  First call syntaxOK or isValid
        to parse (and optionally bind) the expression. */
    const std::string& parseError() const { return _parseError; }

    /** Get a reference to a list of parse errors in the expression.
        The error structure gives location information as well as the errors itself. */
    const std::vector<Error>& getErrors() const { return _errors; }

    /** Get a reference to a list of the ranges where comments occurred */
    const std::vector<std::pair<int, int> >& getComments() const { return _comments; }

    /** Check if expression is constant.
        Expr will be parsed if needed.  No binding is required. */
    bool isConstant() const;

    /** Determine whether expression uses a particular external variable.
        Expr will be parsed if needed.  No binding is required. */
    bool usesVar(const std::string& name) const;

    /** Determine whether expression uses a particular function.
        Expr will be parsed if needed.  No binding is required. */
    bool usesFunc(const std::string& name) const;

    /** Returns whether the expression contains and calls to non-threadsafe */
    bool isThreadSafe() const { return _threadUnsafeFunctionCalls.size() == 0; }

    /** Internal function where parse tree nodes can register violations in
        thread safety with the main class. */
    void setThreadUnsafe(const std::string& functionName) const { _threadUnsafeFunctionCalls.push_back(functionName); }

    /** Returns a list of functions that are not threadSafe **/
    const std::vector<std::string>& getThreadUnsafeFunctionCalls() const { return _threadUnsafeFunctionCalls; }

    /** Get wantVec setting */
    bool wantVec() const { return _wantVec; }

    /** Determine if expression computes a vector (may be false even
        if wantVec is true).  Expr will be parsed and variables and
        functions will be bound if needed. */
    bool isVec() const;

    /** Return the return type of the expression.  Currently may not
        match the type set in setReturnType.  Expr will be parsed and
        variables and functions will be bound if needed. */
    const ExprType& returnType() const;

    /// Evaluate multiple blocks
    void evalMultiple(VarBlock* varBlock, int outputVarBlockOffset, size_t rangeStart, size_t rangeEnd) const;

    // TODO: make this deprecated
    /** Evaluates and returns float (check returnType()!) */
    const double* evalFP(VarBlock* varBlock = nullptr) const;

    // TODO: make this deprecated
    /** Evaluates and returns string (check returnType()!) */
    const char* evalStr(VarBlock* varBlock = nullptr) const;

    /** Reset expr - force reparse/rebind */
    void reset();

    /** override resolveVar to add external variables */
    virtual ExprVarRef* resolveVar(const std::string& name) const { return 0; }

    /** override resolveFunc to add external functions */
    virtual ExprFunc* resolveFunc(const std::string& name) const { return 0; }

    /** records an error in prep or parse stage */
    void addError(const std::string& error, const int startPos, const int endPos) const {
        _errors.push_back(Error(error, startPos, endPos));
    }

    /** records a comment */
    void addComment(int pos, int length) { _comments.push_back(std::pair<int, int>(pos, length + pos - 1)); }

    /** Returns a read only map of local variables that were set **/
    // const LocalVarTable& getLocalVars() const {return _localVars;}

    /** An immutable reference to access context parameters from say ExprFuncX's */
    const Context& context() const { return *_context; }
    void setContext(const Context& context);

    /** Debug printout of parse tree */
    void debugPrintParseTree() const;

    /** Debug printout of interpreter evaluation program  **/
    void debugPrintInterpreter() const;

    /** Debug printout of LLVM evaluation  **/
    void debugPrintLLVM() const;

    /** Set variable block creator (lifetime of expression must be <= block) **/
    void setVarBlockCreator(const VarBlockCreator* varBlockCreator);

    const VarBlockCreator* varBlockCreator() const { return _varBlockCreator; }

  private:
    /** No definition by design. */
    Expression(const Expression& e);
    Expression& operator=(const Expression& e);

    /** Parse, and remember parse error if any */
    void parse() const;

    /** Parse, but only if not yet parsed */
    void parseIfNeeded() const {
        if (!_parsed) parse();
    }

    /** Prepare expression (bind vars/functions, etc.)
    and remember error if any */
    void prep() const;

    /** True if the expression wants a vector */
    bool _wantVec;

    /** Computed return type. */
    mutable ExprType _returnType;

    /** The expression. */
    std::string _expression;

    EvaluationStrategy _evaluationStrategy;

    /** Context for out of band function parameters */
    const Context* _context;

  protected:
    /** Computed return type. */
    mutable ExprType _desiredReturnType;

    /** Variable environment */
    mutable ExprVarEnvBuilder _envBuilder;
    /** Parse tree (null if syntax is bad). */
    mutable ExprNode* _parseTree;

    /** Prepare, but only if not yet prepped */
    void prepIfNeeded() const {
        if (!_prepped) prep();
    }

  private:
    /** Flag if we are valid or not */
    mutable bool _isValid;
    /** Flag set once expr is parsed/prepped (parsing is automatic and lazy) */
    mutable bool _parsed, _prepped;

    /** Cached parse error (returned by isValid) */
    mutable std::string _parseError;

    /** Cached parse error location {startline,startcolumn,endline,endcolumn} */
    mutable std::vector<Error> _errors;

    /** Cached comments */
    mutable std::vector<std::pair<int, int> > _comments;

    /** Variables used in this expr */
    mutable std::set<std::string> _vars;

    /** Functions used in this expr */
    mutable std::set<std::string> _funcs;

    /** Local variable table */
    // mutable LocalVarTable _localVars;

    /** Whether or not we have unsafe functions */
    mutable std::vector<std::string> _threadUnsafeFunctionCalls;

    /** Interpreter */
    mutable Interpreter* _interpreter;
    mutable int _returnSlot;

    // LLVM evaluation layer
    mutable LLVMEvaluator* _llvmEvaluator;

    // Var block creator
    const VarBlockCreator* _varBlockCreator = 0;

    /* internal */ public:

    //! add local variable (this is for internal use)
    void addVar(const char* n) const { _vars.insert(n); }

    //! add function evaluation (this is for internal use)
    void addFunc(const char* n) const { _funcs.insert(n); }

    ////! get local variable reference (this is for internal use)
    // ExprVarRef* resolveLocalVar(const char* n) const {
    //    LocalVarTable::iterator iter = _localVars.find(n);
    //    if (iter != _localVars.end()) return &iter->second;
    //    return 0;
    //}

    /** get local variable reference. This is potentially useful for expression debuggers
        and/or uses of expressions where mutable variables are desired */
    /* ExprLocalVarRef* getLocalVar(const char* n) const { */
    /*     return &_localVars[n];  */
    /* } */
};
}

#endif
