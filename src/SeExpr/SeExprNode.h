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

#ifndef SeExprNode_h
#define SeExprNode_h

#ifndef MAKEDEPEND
#include <string.h>
#include <string>
#include <vector>
#endif

#include "SeExpression.h"
#include "SeVec3d.h"

class SeExprFunc;

/// Expression node base class.  Always constructed by parser in SeExprParser.y
class SeExprNode {
public:
    SeExprNode(const SeExpression* expr);
    /// These constructors supply one or more children.
    SeExprNode(const SeExpression* expr, SeExprNode* a);
    SeExprNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b);
    SeExprNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b, SeExprNode* c);
    virtual ~SeExprNode();

    /// True if node has a vector result.
    bool isVec() const { return _isVec; }

    /// Evaluation method.  Note: v[1] and v[2] are undefined if !isVec
    virtual void eval(SeVec3d& v) const;

    /// Access expression
    const SeExpression* expr() const { return _expr; }

    /// Access parent node - root node has no parent
    const SeExprNode* parent() const { return _parent; }
    /// Access children
    int numChildren() const { return _children.size(); }
    const SeExprNode* child(int i) const { return _children[i]; }
    SeExprNode* child(int i) { return _children[i]; }

    /// Add a child to the child list (for parser use only)
    void addChild(SeExprNode* child);

    /// Transfer children from surrogate parent (for parser use only)
    void addChildren(SeExprNode* surrogate);

    /** Prepare the node (for parser use only).  See the discussion at
	the start of SeExprNode.cpp for more info.
    */
    virtual bool prep(bool wantVec);

    /// Remember the line and column position in the input string 
    inline void setPosition(const short int startPos,const short int endPos)
    {_startPos=startPos;_endPos=endPos;}

    /// Register error
    inline void addError(const std::string& error)
    {_expr->addError(error,_startPos,_endPos);}

protected:
    /// Owning expression (node can't modify)
    const SeExpression* _expr;

    /// Parent node (null if this the the root)
    SeExprNode* _parent;

    /// List of children
    std::vector<SeExprNode*> _children;

    /// True if node has a vector result
    bool _isVec;

    /// Position line and collumn
    unsigned short int _startPos,_endPos;
};


/// Node that computes local variables before evaluating expression
class SeExprBlockNode : public SeExprNode
{
public:
    SeExprBlockNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprNode(expr, a, b) {}

    virtual bool prep(bool wantVec);
    virtual void eval(SeVec3d& result) const;
};


/// Node that computes local variables before evaluating expression
class SeExprIfThenElseNode : public SeExprNode
{
public:
    SeExprIfThenElseNode(const SeExpression* expr,
			 SeExprNode* a, SeExprNode* b, SeExprNode* c) :
	SeExprNode(expr, a, b, c) {}

    virtual bool prep(bool wantVec);
    virtual void eval(SeVec3d& result) const;
};


/// Node that compute a local variable assignment
class SeExprAssignNode : public SeExprNode
{
public:
    SeExprAssignNode(const SeExpression* expr, const char* name, SeExprNode* e) :
	SeExprNode(expr, e), _name(name), _var(0) {}

    virtual bool prep(bool wantVec);
    virtual void eval(SeVec3d& result) const;

private:
    const char* _name; // this is owned by the SeExprNode's parent SeExpression
    SeExprLocalVarRef* _var; // this is owned by the SeExprNode's parent SeExpression
};


/// Node that constructs a vector from three scalars
class SeExprVecNode : public SeExprNode
{
public:
    SeExprVecNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b, SeExprNode* c) :
	SeExprNode(expr, a, b, c) {}

    virtual bool prep(bool wantVec);
    virtual void eval(SeVec3d& result) const;
};


/// Node that computes a negation (scalar or vector)
class SeExprNegNode : public SeExprNode
{
public:
    SeExprNegNode(const SeExpression* expr, SeExprNode* a) :
	SeExprNode(expr, a) {}

    virtual void eval(SeVec3d& result) const;
};

/// Node that computes an inversion (1-x) (scalar or vector)
class SeExprInvertNode : public SeExprNode
{
public:
    SeExprInvertNode(const SeExpression* expr, SeExprNode* a) :
	SeExprNode(expr, a) {}

    virtual void eval(SeVec3d& result) const;
};


/// Note that computes a logical not
class SeExprNotNode : public SeExprNode
{
public:
    SeExprNotNode(const SeExpression* expr, SeExprNode* a) :
	SeExprNode(expr, a) {}

    virtual void eval(SeVec3d& result) const;
};


/// Node that evaluates a conditional (if-then-else) expression
class SeExprCondNode : public SeExprNode
{
public:
    SeExprCondNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b, SeExprNode* c) :
	SeExprNode(expr, a, b, c) {}

    virtual bool prep(bool wantVec);
    virtual void eval(SeVec3d& result) const;
};


/// Node that evaluates a logical AND
class SeExprAndNode : public SeExprNode
{
public:
    SeExprAndNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprNode(expr, a, b) {}

    virtual bool prep(bool wantVec);
    virtual void eval(SeVec3d& result) const;
};


/// Node that evaluates a logical OR
class SeExprOrNode : public SeExprNode
{
public:
    SeExprOrNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprNode(expr, a, b) {}

    virtual bool prep(bool wantVec);
    virtual void eval(SeVec3d& result) const;
};


/// Node that evaluates a component of a vector
class SeExprSubscriptNode : public SeExprNode
{
public:
    SeExprSubscriptNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprNode(expr, a, b) {}

    virtual bool prep(bool wantVec);
    virtual void eval(SeVec3d& result) const;
};


/// Node that implements a numeric comparison
class SeExprCompareEqNode : public SeExprNode
{
public:
    SeExprCompareEqNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprNode(expr, a, b) {}

    virtual bool prep(bool wantVec);
};


/// Node that implements a numeric comparison
class SeExprCompareNode : public SeExprNode
{
public:
    SeExprCompareNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprNode(expr, a, b) {}

    virtual bool prep(bool wantVec);
};


/// Node that implements a numeric comparison
class SeExprEqNode : public SeExprCompareEqNode
{
public:
    SeExprEqNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprCompareEqNode(expr, a, b) {}

    virtual void eval(SeVec3d& result) const;
};


/// Node that implements a numeric comparison
class SeExprNeNode : public SeExprCompareEqNode
{
public:
    SeExprNeNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprCompareEqNode(expr, a, b) {}

    virtual void eval(SeVec3d& result) const;
};


/// Node that implements a numeric comparison
class SeExprLtNode : public SeExprCompareNode
{
public:
    SeExprLtNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprCompareNode(expr, a, b) {}

    virtual void eval(SeVec3d& result) const;
};


/// Node that implements a numeric comparison
class SeExprGtNode : public SeExprCompareNode
{
public:
    SeExprGtNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprCompareNode(expr, a, b) {}

    virtual void eval(SeVec3d& result) const;
};


/// Node that implements a numeric comparison
class SeExprLeNode : public SeExprCompareNode
{
public:
    SeExprLeNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprCompareNode(expr, a, b) {}

    virtual void eval(SeVec3d& result) const;
};


/// Node that implements a numeric comparison
class SeExprGeNode : public SeExprCompareNode
{
public:
    SeExprGeNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprCompareNode(expr, a, b) {}

    virtual void eval(SeVec3d& result) const;
};


/// Node that implements an arithmetic operator
class SeExprAddNode : public SeExprNode
{
public:
    SeExprAddNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprNode(expr, a, b) {}

    virtual void eval(SeVec3d& result) const;
};


/// Node that implements an arithmetic operator
class SeExprSubNode : public SeExprNode
{
public:
    SeExprSubNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprNode(expr, a, b) {}

    virtual void eval(SeVec3d& result) const;
};


/// Node that implements an arithmetic operator
class SeExprMulNode : public SeExprNode
{
public:
    SeExprMulNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprNode(expr, a, b) {}

    virtual void eval(SeVec3d& result) const;
};


/// Node that implements an arithmetic operator
class SeExprDivNode : public SeExprNode
{
public:
    SeExprDivNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprNode(expr, a, b) {}

    virtual void eval(SeVec3d& result) const;
};


/// Node that implements an arithmetic operator
class SeExprModNode : public SeExprNode
{
public:
    SeExprModNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprNode(expr, a, b) {}

    virtual void eval(SeVec3d& result) const;
};


/// Node that implements an arithmetic operator
class SeExprExpNode : public SeExprNode
{
public:
    SeExprExpNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprNode(expr, a, b) {}

    virtual void eval(SeVec3d& result) const;
};

/// Node that references a variable
class SeExprVarNode : public SeExprNode
{
public:
    SeExprVarNode(const SeExpression* expr, const char* name) :
	SeExprNode(expr), _name(name), _var(0), _data(0) 
    { expr->addVar(name); }

    virtual bool prep(bool wantVec);
    virtual void eval(SeVec3d& result) const;
    const char* name() const { return _name; }
    
    /// base class for custom instance data
    struct Data { virtual ~Data() {} };
    void setData(Data* data) const { _data = data; }
    Data* getData() const { return _data; }

private:
    const char* _name; // this is owned by the SeExprNode's parent SeExpression
    SeExprVarRef* _var; // this is owned by somebody else
    mutable Data* _data;
};


/// Node that stores a numeric constant
class SeExprNumNode : public SeExprNode
{
public:
    SeExprNumNode(const SeExpression* expr, double val) :
	SeExprNode(expr), _val(val) {}

    virtual void eval(SeVec3d& result) const { result[0] = _val; }

private:
    double _val;
};


/// Node that stores a string
class SeExprStrNode : public SeExprNode
{
public:
    SeExprStrNode(const SeExpression* expr, const char* str) :
	SeExprNode(expr), _str(str) {}

    virtual bool prep(bool /*wantVec*/)
    { addError("Invalid string parameter: "+_str); return 0; }

    virtual void eval(SeVec3d& result) const { result[0] = 0; }
    const char* str() const { return _str.c_str(); }

private:
    std::string _str;
};


/// Node that calls a function
class SeExprFuncNode : public SeExprNode
{
public:
    SeExprFuncNode(const SeExpression* expr, const char* name) :
	SeExprNode(expr), _name(name), _func(0), _nargs(0), _data(0) 
    {
	expr->addFunc(name);
    }
    virtual ~SeExprFuncNode() { delete _data; }

    virtual bool prep(bool wantVec);
    virtual void eval(SeVec3d& result) const;
    void setIsVec(bool isVec) { _isVec = isVec; }
    const char* name() const { return _name.c_str(); }


    //! return the number of arguments
    int nargs() const { return _nargs; }

    double* scalarArgs() const { return &_scalarArgs[0]; }
    SeVec3d* vecArgs() const { return &_vecArgs[0]; }

    //! eval all arguments (use in eval())
    SeVec3d* evalArgs() const;

    //! eval an argument (use in eval())
    SeVec3d evalArg(int n) const;

    //! returns whether the nth argument is a string (use in prep)
    bool isStrArg(int n) const;

    //! get nth string argument (use in prep)
    std::string getStrArg(int n) const;

    //! base class for custom instance data
    struct Data { virtual ~Data() {} };

    //! associate blind data with this node (subsequently owned by this object)
    /***
        Use this to set data associated with the node. Equivalently this is data
        associated with a specific evaluation point of a function.
        Examples would be tokenized values, 
        sorted lists for binary searches in curve evaluation, etc. This should be done
        in SeExprFuncX::prep().
    */
    void setData(Data* data) const { _data = data; }

    //! get associated blind data (returns 0 if none)
    /***
        Use this to get data associated in the prep() routine. This is typically
        used from SeExprFuncX::eval()
    */
    Data* getData() const { return _data; }

private:
    std::string _name;
    const SeExprFunc* _func;
    int _nargs;
    mutable std::vector<double> _scalarArgs;
    mutable std::vector<SeVec3d> _vecArgs;
    mutable Data* _data;
};

#endif
