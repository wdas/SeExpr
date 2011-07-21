/*
 SEEXPR SOFTWARE
 Copyright 2011 Disney Enterprises, Inc. All rights reserved
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are
 met:
 
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in
 the documentation and/or other materials provided with the
 distribution.
 
 * The names "Disney", "Walt Disney Pictures", "Walt Disney Animation
 Studios" or the names of its contributors may NOT be used to
 endorse or promote products derived from this software without
 specific prior written permission from Walt Disney Pictures.
 
 Disclaimer: THIS SOFTWARE IS PROVIDED BY WALT DISNEY PICTURES AND
 CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR PURPOSE, NONINFRINGEMENT AND TITLE ARE DISCLAIMED.
 IN NO EVENT SHALL WALT DISNEY PICTURES, THE COPYRIGHT HOLDER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND BASED ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*/
#ifndef SeExpression_h
#define SeExpression_h

#include <string>
#include <map>
#include <set>
#include <vector>
#include "SeVec3d.h"

#include "SeExprType.h"
#include "SeExprEnv.h"

class SeExprNode;
class SeExprVarNode;
class SeExprLocalVarRef;
class SeExprFunc;
class SeExpression;

//! abstract class for implementing variable references
class SeExprVarRef
{
    SeExprVarRef()
        : _type(SeExprType::ErrorType())
    {};

 public:
    SeExprVarRef(const SeExprType & type)
        : _type(type)
    {};

    virtual ~SeExprVarRef() {}

    //! sets (current) type to given type
    virtual void setType(const SeExprType & type) { _type = type; };

    //! returns (current) type
    virtual SeExprType type() const { return _type; };

    //! returns this variable's value by setting result, node refers to 
    //! where in the parse tree the evaluation is occurring
    virtual void eval(const SeExprVarNode* node, SeVec3d& result) = 0;

 private:
    SeExprType _type;
};

/// simple vector variable reference reference base class
class SeExprVectorVarRef : public SeExprVarRef
{
 public:
    SeExprVectorVarRef()
        : SeExprVarRef(SeExprType::FPNType(3))
    {};

    virtual bool isVec() { return 1; }
};


/// simple scalar variable reference reference base class
class SeExprScalarVarRef : public SeExprVarRef
{
 public:
    SeExprScalarVarRef()
        : SeExprVarRef(SeExprType::FP1Type())
    {};

    virtual bool isVec() { return 0; }
};

/// uses internally to represent local variables
class SeExprLocalVarRef : public SeExprVarRef
{
    SeExprLocalVarRef()
        : SeExprVarRef(SeExprType::ErrorType())
    {};

 public:
    SeVec3d val;

    SeExprLocalVarRef(const SeExprType & type)
        :SeExprVarRef(type)
    {};

    virtual void eval(const SeExprVarNode*, SeVec3d& result) 
    { result = val; }
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
    SeExpression( const std::string &e, bool wantVec=true );
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

    /** Get a reference to a list of the ranges where comments occurred */
    const std::vector<std::pair<int,int> >& getComments() const
    {return _comments;}

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
    virtual SeExprVarRef* resolveVar(const std::string& name) const {return 0;}

    /** override resolveFunc to add external functions */
    virtual SeExprFunc* resolveFunc(const std::string& name) const {return 0;}

    /** records an error in prep or parse stage */
    void addError(const std::string& error,const int startPos,const int endPos) const

    {_errors.push_back(Error(error,startPos,endPos));}

    /** records a comment */
    void addComment(int pos,int length)
    {_comments.push_back(std::pair<int,int>(pos,length+pos-1));}

    /** Returns a read only map of local variables that were set **/
    const LocalVarTable& getLocalVars() const {return _localVars;}

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
    
 protected:
    /** Parse tree (null if syntax is bad). */
    mutable SeExprNode *_parseTree;

 private:
    /** Flag set once expr is parsed/prepped (parsing is automatic and lazy) */
    mutable bool _parsed, _prepped;
    
    /** Cached parse error (returned by isValid) */
    mutable std::string _parseError;

    /** Cached parse error location {startline,startcolumn,endline,endcolumn} */
    mutable std::vector<Error> _errors;

    /** Cached comments */
    mutable std::vector<std::pair<int,int> > _comments;

    /** Variables used in this expr */
    mutable std::set<std::string> _vars;

    /** Functions used in this expr */
    mutable std::set<std::string> _funcs;

    /** Local variable table */
    mutable LocalVarTable _localVars;

    /** Whether or not we have unsafe functions */
    mutable std::vector<std::string> _threadUnsafeFunctionCalls;

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
    /* SeExprLocalVarRef* getLocalVar(const char* n) const { */
    /*     return &_localVars[n];  */
    /* } */
};

#endif
