/*
* Copyright Disney Enterprises, Inc.  All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License
* and the following modification to it: Section 6 Trademarks.
* deleted and replaced with:
*
* 6. Trademarks. This License does not grant permission to use the
* trade names, trademarks, service marks, or product names of the
* Licensor and its affiliates, except as required for reproducing
* the content of the NOTICE file.
*
* You may obtain a copy of the License at
* http://www.apache.org/licenses/LICENSE-2.0
*/
#ifndef SeExpression_h
#define SeExpression_h

#include <string>
#include <map>
#include <set>
#include <vector>
#include "SeExprMacros.h"
#include "SeVec3d.h"
#include "SeContext.h"

class SeExprNode;
class SeExprVarNode;
class SeExprLocalVarRef;
class SeExprFunc;
class SeExpression;

//! abstract class for implementing variable references
class SeExprVarRef
{
 public:
    virtual ~SeExprVarRef() {}

    //! returns true for a vector type, false for a scalar type
    virtual bool isVec() = 0;

    //! returns this variable's value by setting result, node refers to
    //! where in the parse tree the evaluation is occurring
    virtual void eval(const SeExprVarNode* node, SeVec3d& result) = 0;
};

/// simple vector variable reference reference base class
class SeExprVectorVarRef : public SeExprVarRef
{
 public:
    virtual bool isVec() { return 1; }
};


/// simple scalar variable reference reference base class
class SeExprScalarVarRef : public SeExprVarRef
{
 public:
    virtual bool isVec() { return 0; }
};

/// uses internally to represent local variables
class SeExprLocalVarRef : public SeExprVarRef
{
 public:
    SeVec3d val;
    SeExprLocalVarRef() : _isVec(false) {}
    void setIsVec() { _isVec = true; }
    virtual void eval(const SeExprVarNode*, SeVec3d& result)
    { result = val; }
    virtual bool isVec() { return _isVec; }
 private:
    bool _isVec;
};


/// main expression class
class SeExpression
{
 public:
    typedef std::map<std::string, SeExprLocalVarRef> LocalVarTable;

    //! Represents a parse or type checking error in an expression
    struct Error
    {
        //! Text of error
        std::string error;

        //! Error start offset in expression string
        int startPos;

        //! Error end offset in expression string
        int endPos;

        Error(const std::string& errorIn,const int startPosIn,const int endPosIn)
            :error(errorIn),startPos(startPosIn),endPos(endPosIn)
        {}
    };

    SeExpression( );
    SeExpression( const std::string &e, bool wantVec=true, const SeContext& context=SeContext::global() );
    virtual ~SeExpression();

    /** Sets the expression to desire a vector or a scalar.
        This will allow the evaluation to potentially be optimized if
        only a scalar is desired. */
    void setWantVec(bool wantVec);

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
    bool isValid() const;

    /** Get parse error (if any).  First call syntaxOK or isValid
	to parse (and optionally bind) the expression. */
    const std::string& parseError() const { return _parseError; }

    /** Get a reference to a list of parse errors in the expression.
        The error structure gives location information as well as the errors itself. */
    const std::vector<Error>& getErrors() const
    {return _errors;}

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
    {return _threadUnsafeFunctionCalls.size()==0;}

    /** Internal function where parse tree nodes can register violations in
        thread safety with the main class. */
    void setThreadUnsafe(const std::string& functionName) const
    {_threadUnsafeFunctionCalls.push_back(functionName);}

    /** Returns a list of functions that are not threadSafe **/
    const std::vector<std::string>& getThreadUnsafeFunctionCalls() const
    {return _threadUnsafeFunctionCalls;}

    /** Get wantVec setting */
    bool wantVec() const { return _wantVec; }

    /** Determine if expression computes a vector (may be false even
	if wantVec is true).  Expr will be parsed and variables and
	functions will be bound if needed. */
    bool isVec() const;

    /** Evaluate the expression.  This will parse and bind if needed */
    SeVec3d evaluate() const;

    /** Reset expr - force reparse/rebind */
    void reset();

    /** override resolveVar to add external variables */
    virtual SeExprVarRef* resolveVar(const std::string& /*name*/) const {return 0;}

    /** override resolveFunc to add external functions */
    virtual SeExprFunc* resolveFunc(const std::string& /*name*/) const {return 0;}

    /** records an error in prep or parse stage */
    void addError(const std::string& error,const int startPos,const int endPos) const
    {_errors.push_back(Error(error,startPos,endPos));}

    /** Returns a read only map of local variables that were set **/
    const LocalVarTable& getLocalVars() const {return _localVars;}

    /** An immutable reference to access context parameters from say SeExprFuncX's */
    const SeContext& context() const {return *_context;}

    void setContext(const SeContext& context);

 private:
    /** No definition by design. */
    SeExpression( const SeExpression &e );
    SeExpression &operator=( const SeExpression &e );

    /** Parse, and remember parse error if any */
    void parse() const;

    /** Prepare expression (bind vars/functions, etc.)
	and remember error if any */
    void prep() const;

    /** Parse, but only if not yet parsed */
    void parseIfNeeded() const { if (!_parsed) parse(); }

    /** Prepare, but only if not yet prepped */
    void prepIfNeeded() const { if (!_prepped) prep(); }

    /** True if the expression wants a vector */
    bool _wantVec;

    /** The expression. */
    std::string _expression;

    /** Parse tree (null if syntax is bad). */
    mutable SeExprNode *_parseTree;

    /** Flag set once expr is parsed/prepped (parsing is automatic and lazy) */
    mutable bool _parsed, _prepped;

    /** Cached parse error (returned by isValid) */
    mutable std::string _parseError;

    /** Cached parse error location {startline,startcolumn,endline,endcolumn} */
    mutable std::vector<Error> _errors;

    /** Variables used in this expr */
    mutable std::set<std::string> _vars;

    /** Functions used in this expr */
    mutable std::set<std::string> _funcs;

    /** Local variable table */
    mutable LocalVarTable _localVars;

    /** Whether or not we have unsafe functions */
    mutable std::vector<std::string> _threadUnsafeFunctionCalls;

    /** String tokens allocated by lex */
    mutable std::vector<char*> _stringTokens;

    /** Context for out of band function parameters */
    const SeContext* _context;

    /* internal */ public:

    //! add local variable (this is for internal use)
    void addVar(const char* n) const { _vars.insert(n); }

    //! add local function (this is for internal use)
    void addFunc(const char* n) const { _funcs.insert(n); }

    //! get local variable reference (this is for internal use)
    SeExprVarRef* resolveLocalVar(const char* n) const {
	LocalVarTable::iterator iter = _localVars.find(n);
	if (iter != _localVars.end()) return &iter->second;
	return 0;
    }

    /** get local variable reference. This is potentially useful for expression debuggers
        and/or uses of expressions where mutable variables are desired */
    SeExprLocalVarRef* getLocalVar(const char* n) const {
	return &_localVars[n];
    }
};

#endif
