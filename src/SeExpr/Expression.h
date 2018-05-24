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

#include <stdint.h>

#include <atomic>
#include <iomanip>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <vector>

#include "Context.h"
#include "Evaluator.h"
#include "ExprConfig.h"
#include "Vec.h"

namespace llvm {
class ExecutionEngine;
class LLVMContext;
}

namespace SeExpr2 {

class ExprNode;
class ExprVarNode;
class ExprFunc;
class Expression;

//! abstract class for implementing variable references
class ExprVarRef {
    ExprVarRef() : _type(ExprType().Error().Varying()){};

  public:
    ExprVarRef(const ExprType& type) : _type(type){};

    virtual ~ExprVarRef()
    {
    }

    //! sets (current) type to given type
    virtual void setType(const ExprType& type)
    {
        _type = type;
    };

    //! returns (current) type
    virtual ExprType type() const
    {
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
    enum EvaluationStrategy { UseInterpreter, UseLLVM };
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
            : error(errorIn), startPos(startPosIn), endPos(endPosIn)
        {
        }
    };

    Expression(EvaluationStrategy hint = Expression::defaultEvaluationStrategy);
    Expression(const std::string& e,
               const ExprType& type = ExprType().FP(3),
               EvaluationStrategy hint = Expression::defaultEvaluationStrategy,
               const Context& context = Context::global());

    virtual ~Expression();

    /** Sets desired return value.
        This will allow the evaluation to potentially be optimized. */
    virtual void setDesiredReturnType(const ExprType& type) final;

    /** Set expression string to e.
        This invalidates all parsed state. */
    virtual void setExpr(const std::string& e) final;

    //! Get the string that this expression is currently set to evaluate
    const std::string& getExpr() const
    {
        return _expression;
    }

    /** Check expression syntax.  Expr will be parsed if needed.  If
        this returns false, the error message can be accessed via
        parseError() */
    inline bool syntaxOK() const
    {
        return (bool)parseTree();
    }

    /** Check if expression is valid.  Expr will be parsed if
    needed. Variables and functions will also be bound.  If this
    returns false, the error message can be accessed via
    parseError() */
    inline bool isValid() const
    {
        return evaluator()->isValid();
    }

    ExprNode* parseTree() const
    {
        if (_parseTree)
            return _parseTree;
        parse();
        return _parseTree;
    }

    Evaluator* evaluator() const
    {
        if (_evaluator)
            return _evaluator;
        prep();
        return _evaluator;
    }

    /** Get parse error (if any).  First call syntaxOK or isValid
        to parse (and optionally bind) the expression. */
    const std::string& parseError() const
    {
        return _parseError;
    }

    /** Get a reference to a list of parse errors in the expression.
        The error structure gives location information as well as the errors itself. */
    const std::vector<Error>& getErrors() const
    {
        return _errors;
    }

    /** Get a reference to a list of the ranges where comments occurred */
    const std::vector<std::pair<int, int> >& getComments() const
    {
        return _comments;
    }

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
    bool isThreadSafe() const
    {
        return _threadUnsafeFunctionCalls.size() == 0;
    }

    /** Internal function where parse tree nodes can register violations in
        thread safety with the main class. */
    void setThreadUnsafe(const std::string& functionName) const
    {
        _threadUnsafeFunctionCalls.push_back(functionName);
    }

    /** Returns a list of functions that are not threadSafe **/
    const std::vector<std::string>& getThreadUnsafeFunctionCalls() const
    {
        return _threadUnsafeFunctionCalls;
    }

    /** Get wantVec setting */
    bool wantVec() const
    {
        return _wantVec;
    }

    /** Determine if expression computes a vector (may be false even
        if wantVec is true).  Expr will be parsed and variables and
        functions will be bound if needed. */
    bool isVec() const;

    /** Return the return type of the expression.  Currently may not
        match the type set in setReturnType.  Expr will be parsed and
        variables and functions will be bound if needed. */
    const ExprType& returnType() const;

    /// Evaluate multiple blocks
    inline void evalMultiple(VarBlock* varBlock, double* outputBuffer, size_t rangeStart, size_t rangeEnd) const
    {
        evaluator()->evalMultiple(varBlock, outputBuffer, rangeStart, rangeEnd);
    }

    void evalMultiple(VarBlock* varBlock, int outputVarBlockOffset, size_t rangeStart, size_t rangeEnd) const;

    // Evaluates and returns float (check returnType()!)
    // Not thread-safe
    inline const double* evalFP(VarBlock* varBlock = nullptr) const
    {
        _results.resize(_desiredReturnType.dim());
        evaluator()->evalFP(_results.data(), varBlock);
        return _results.data();
    }

    // Evaluates and returns string (check returnType()!)
    // Not thread-safe
    inline const char* evalStr(VarBlock* varBlock = nullptr) const
    {
        _results.resize(_desiredReturnType.dim());
        evaluator()->evalStr((char*)_results.data(), varBlock);
        return (const char*)_results.data();
    }

    // Evaluates and returns float (check returnType()!)
    inline void evalFP(double* dst, VarBlock* varBlock = nullptr) const
    {
        evaluator()->evalFP(dst, varBlock);
    }

    // Evaluates and returns float (check returnType()!)
    // Robustly promotes from scalar to vector by filling the dst buffer with N values if necessary
    inline void evalFP(double* dst, size_t N, VarBlock* varBlock = nullptr) const
    {
        evaluator()->evalFP(dst, varBlock);
        size_t dimComputed = _desiredReturnType.dim();
        if (dimComputed == 1 && N > dimComputed)
            std::fill_n(dst + 1, N - 1, dst[0]);
    }

    // Evaluates and returns string (check returnType()!)
    inline void evalStr(char* dst, VarBlock* varBlock = nullptr) const
    {
        evaluator()->evalStr(dst, varBlock);
    }

    /** Reset expr - force reparse/rebind */
    // if overridden, you must still call Expression::reset()!
    virtual void reset();

    /** override resolveVar to add external variables */
    virtual ExprVarRef* resolveVar(const std::string& /*name*/) const
    {
        return 0;
    }

    /** override resolveFunc to add external functions */
    virtual ExprFunc* resolveFunc(const std::string& /*name*/) const
    {
        return 0;
    }

    /** records an error in prep or parse stage */
    void addError(const std::string& error, const int startPos, const int endPos) const
    {
        _errors.push_back(Error(error, startPos, endPos));
    }

    /** records a comment */
    void addComment(int pos, int length)
    {
        _comments.push_back(std::pair<int, int>(pos, length + pos - 1));
    }

    /** Returns a read only map of local variables that were set **/
    // const LocalVarTable& getLocalVars() const {return _localVars;}

    /** An immutable reference to access context parameters from say ExprFuncX's */
    const Context& context() const
    {
        return *_context;
    }
    virtual void setContext(const Context& context) final;

    /** Set variable block creator (lifetime of expression must be <= block) **/
    virtual void setVarBlockCreator(const VarBlockCreator* varBlockCreator) final;

    const VarBlockCreator* varBlockCreator() const
    {
        return _varBlockCreator;
    }

  private:
    /** No definition by design. */
    Expression(const Expression& e);
    Expression& operator=(const Expression& e);

    /** Parse, and remember parse error if any */
    void parse() const;

    /** True if the expression wants a vector */
    bool _wantVec;

    /** Computed return type. */
    mutable ExprType _returnType;

    /** The expression. */
    std::string _expression;

    EvaluationStrategy _evaluationStrategyHint;

    /** Context for out of band function parameters */
    const Context* _context;

  protected:
    /** Computed return type. */
    mutable ExprType _desiredReturnType;

    /** Parse tree (null if syntax is bad). */
    mutable std::atomic<ExprNode*> _parseTree;

    /** Prepare expression (bind vars/functions, etc.)
    and remember error if any */
    void prep() const;

  private:
    mutable std::mutex _parseMutex;
    mutable std::mutex _prepMutex;

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

    /** Whether or not we have unsafe functions */
    mutable std::vector<std::string> _threadUnsafeFunctionCalls;

    mutable std::atomic<Evaluator*> _evaluator;

    // Var block creator
    const VarBlockCreator* _varBlockCreator = 0;

    mutable std::vector<double> _results;

    /* internal */ public:

    //! add local variable (this is for internal use)
    void addVar(const char* n) const
    {
        _vars.insert(n);
    }

    //! add function evaluation (this is for internal use)
    void addFunc(const char* n) const
    {
        _funcs.insert(n);
    }
};
}

#endif
