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

#ifndef SeExprNode_h
#define SeExprNode_h

#include <cstdlib>

#ifndef MAKEDEPEND
#include <string.h>
#include <string>
#include <vector>
#endif

#include "SeExpression.h"
#include "SeExprType.h"
#include "SeExprEnv.h"
#include "SeVec3d.h"

class SeExprFunc;

/// Expression node base class.  Always constructed by parser in SeExprParser.y
class SeExprNode {
public:
    SeExprNode(const SeExpression* expr);
    SeExprNode(const SeExpression* expr,
               const SeExprType & type);
    /// These constructors supply one or more children.
    SeExprNode(const SeExpression* expr,
               SeExprNode* a);
    SeExprNode(const SeExpression* expr,
               SeExprNode* a,
               const SeExprType & type);
    SeExprNode(const SeExpression* expr,
               SeExprNode* a,
               SeExprNode* b);
    SeExprNode(const SeExpression* expr,
               SeExprNode* a,
               SeExprNode* b,
               const SeExprType & type);
    SeExprNode(const SeExpression* expr,
               SeExprNode* a,
               SeExprNode* b,
               SeExprNode* c);
    SeExprNode(const SeExpression* expr,
               SeExprNode* a,
               SeExprNode* b,
               SeExprNode* c,
               const SeExprType & type);
    virtual ~SeExprNode();

    /// True if node has a vector result.
    bool isVec() const { return _isVec; }

    /// Access expression
    const SeExpression* expr() const { return _expr; }

    /// Access to original string representation of current expression
    std::string toString() const { return expr()->getExpr().substr(startPos(), length()); };

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

    /// Transfer children from surrogate parent (does not delete surrogate)
    void addChildren_without_delete(SeExprNode* surrogate);

    /** Prepare the node (for parser use only).  See the discussion at
	the start of SeExprNode.cpp for more info.
    */
    virtual SeExprType prep(SeExprType wanted, SeExprVarEnv & env);

    /// Evaluation method.  Note: v[1] and v[2] are undefined if !isVec
    virtual void eval(SeVec3d& v) const;

    /// The type of the node
    const SeExprType & type() const { return _type; };

    /// Remember the line and column position in the input string 
    inline void setPosition(const short int startPos,const short int endPos)
    {_startPos=startPos;_endPos=endPos;}

    /// Access start position in input string
    inline const short int startPos() const { return _startPos; }
    /// Access end position in input string
    inline const short int endPos() const { return _endPos; }
    /// Access length of input string
    inline const short int length() const { return endPos() - startPos(); };

    /// Register error
    inline void addError(const std::string& error) { _expr->addError(error, _startPos, _endPos); }

 protected: /*protected functions*/

    ///Set type
    inline void setType         ()                      { _type = SeExprType::ErrorType_error();        };
    inline void setType         (const SeExprType & t)  { _type = t;                                    };
    inline void setType_varying (const SeExprType & t)  { setType(t); _type.becomeLifetimeVarying ();   };
    inline void setType_uniform (const SeExprType & t)  { setType(t); _type.becomeLifetimeUniform ();   };
    inline void setType_constant(const SeExprType & t)  { setType(t); _type.becomeLifetimeConstant();   };
    inline void setType_error   (const SeExprType & t)  { setType(t); _type.becomeLifetimeError();      };
    inline void setType         (const SeExprType & t,
                                 const SeExprType & t1) { setType(t); _type.becomeLifetime(t1);         };
    inline void setType         (const SeExprType & t,
                                 const SeExprType & t1,
                                 const SeExprType & t2) { setType(t); _type.becomeLifetime(t1, t2);     };
    inline void setType         (const SeExprType & t,
                                 const SeExprType & t1,
                                 const SeExprType & t2,
                                 const SeExprType & t3) { setType(t); _type.becomeLifetime(t1, t2, t3); };
    inline void setType_std     (const SeExprType & t)  {
        setType(t);
        int num = numChildren();
        if(num > 0)
            _type.becomeLifetime(child(0)->type());
        for(int i = 1; i < num; i++)
            _type.combineLifetime(child(i)->type());
    };

    /// Prep system error abstraction
    //   generalCheck passes (no error) if check is true
    inline bool generalCheck(      bool          check,
                             const std::string & message) {
        if(!check) addError(message);
        return check;
    };

    /// Prep system error abstraction
    //   generalCheck passes (no error) if check is true
    inline bool generalCheck(      bool          check,
                             const std::string & message,
                                   bool        & error  ) {
        if(!check) {
            addError(message);
            error = true;
        }
        return check;
    };

    /// Prep system error abstraction (with prep check of children)
    //   generalCheck passes (no error) if check is true
    inline bool generalCheck(      bool           check,
                             const std::string  & message,
                                   bool         & error,
                                   SeExprVarEnv & env    ) {
        if(!check) {
            addError(message);
            error = true;
            SeExprNode::prep(SeExprType::AnyType(), env);
        }
        return check;
    };

    /// Register type mismatch
    inline std::string expectedTypeMismatchString(const SeExprType & expected,
                                                  const SeExprType & received) {
        return ("Type mismatch. Expected: " + expected.toString() +
                " Received: "               + received.toString()); };


    /// Register type mismatch
    inline std::string generalTypeMismatchString(const SeExprType & first,
                                                 const SeExprType & second) {
        return ("Type mismatch. First: " + first .toString() +
                " Second: "              + second.toString()); };

    /// types match (true if they do)
    inline bool isa_with_error(const SeExprType & expected,
                               const SeExprType & received,
                               bool & error) {
        return generalCheck(received.isa(expected),
                            expectedTypeMismatchString(expected, received),
                            error); };

    /// types match (true if they do)
    inline bool isUnder_with_error(const SeExprType & expected,
                                   const SeExprType & received,
                                         bool       & error   ) {
        return generalCheck(received.isUnder(expected),
                            expectedTypeMismatchString(expected, received),
                            error); };

    /// types match (true if they do)
    inline bool typesMatch(const SeExprType & first,
                           const SeExprType & second,
                                 bool       & error  ) {
        return generalCheck(first.isa(second),
                            generalTypeMismatchString(first, second),
                            error); };

protected: /*protected data members*/
    /// Owning expression (node can't modify)
    const SeExpression* _expr;

    /// Parent node (null if this the the root)
    SeExprNode* _parent;

    /// List of children
    std::vector<SeExprNode*> _children;

    /// True if node has a vector result
    bool _isVec;

    // Type of node
    SeExprType _type;

    /// Position line and collumn
    unsigned short int _startPos,_endPos;
};


/// Node that computes local variables before evaluating expression
class SeExprBlockNode : public SeExprNode
{
public:
    SeExprBlockNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprNode(expr, a, b) {}

    virtual SeExprType prep(SeExprType wanted, SeExprVarEnv & env);
    virtual void eval(SeVec3d& result) const;
};


/// Node that computes local variables before evaluating expression
class SeExprIfThenElseNode : public SeExprNode
{
public:
    SeExprIfThenElseNode(const SeExpression* expr,
			 SeExprNode* a, SeExprNode* b, SeExprNode* c) :
	SeExprNode(expr, a, b, c) {}

    virtual SeExprType prep(SeExprType wanted, SeExprVarEnv & env);
    virtual void eval(SeVec3d& result) const;
};


/// Node that compute a local variable assignment
class SeExprAssignNode : public SeExprNode
{
public:
    SeExprAssignNode(const SeExpression* expr, const char* name, SeExprNode* e) :
	SeExprNode(expr, e), _name(name), _var(0) {}

    virtual SeExprType prep(SeExprType wanted, SeExprVarEnv & env);
    virtual void eval(SeVec3d& result) const;

    const std::string& name        () const { return _name;         };
    const SeExprType & assignedType() const { return _assignedType; };

private:
    std::string _name;
    SeExprLocalVarRef* _var;
    SeExprType _assignedType;
};


/// Node that constructs a vector from three scalars
class SeExprVecNode : public SeExprNode
{
public:
    SeExprVecNode(const SeExpression* expr, SeExprNode* surrogate)
        : SeExprNode(expr)
    {
        addChildren_without_delete(surrogate);
    };

    SeExprVecNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b, SeExprNode* c)
        : SeExprNode(expr, a, b, c)
    {};

    virtual SeExprType prep(SeExprType wanted, SeExprVarEnv & env);
    virtual void eval(SeVec3d& result) const;

    SeVec3d value() const;
};

/// NOde that computes with a single operand
class SeExprUnaryOpNode : public SeExprNode
{
public:
    SeExprUnaryOpNode(const SeExpression* expr, SeExprNode* a)
        : SeExprNode(expr, a)
    {}

    virtual SeExprType prep(SeExprType wanted, SeExprVarEnv & env);
};

/// Node that computes a negation (scalar or vector)
class SeExprNegNode : public SeExprUnaryOpNode
{
public:
    SeExprNegNode(const SeExpression* expr, SeExprNode* a) :
	SeExprUnaryOpNode(expr, a) {}

    virtual void eval(SeVec3d& result) const;
};

/// Node that computes an inversion (1-x) (scalar or vector)
class SeExprInvertNode : public SeExprUnaryOpNode
{
public:
    SeExprInvertNode(const SeExpression* expr, SeExprNode* a) :
	SeExprUnaryOpNode(expr, a) {}

    virtual void eval(SeVec3d& result) const;
};

/// Note that computes a logical not
class SeExprNotNode : public SeExprUnaryOpNode
{
public:
    SeExprNotNode(const SeExpression* expr, SeExprNode* a) :
	SeExprUnaryOpNode(expr, a) {}

    virtual void eval(SeVec3d& result) const;
};


/// Node that evaluates a conditional (if-then-else) expression
class SeExprCondNode : public SeExprNode
{
public:
    SeExprCondNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b, SeExprNode* c) :
	SeExprNode(expr, a, b, c) {}

    virtual SeExprType prep(SeExprType wanted, SeExprVarEnv & env);
    virtual void eval(SeVec3d& result) const;
};


/// Node that computes a logical operation (two operands)
class SeExprLogicalOpNode : public SeExprNode
{
public:
    SeExprLogicalOpNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprNode(expr, a, b) {}

    virtual SeExprType prep(SeExprType wanted, SeExprVarEnv & env);
};


/// Node that evaluates a logical AND
class SeExprAndNode : public SeExprLogicalOpNode
{
public:
    SeExprAndNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprLogicalOpNode(expr, a, b) {}

    virtual void eval(SeVec3d& result) const;
};


/// Node that evaluates a logical OR
class SeExprOrNode : public SeExprLogicalOpNode
{
public:
    SeExprOrNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprLogicalOpNode(expr, a, b) {}

    virtual void eval(SeVec3d& result) const;
};


/// Node that evaluates a component of a vector
class SeExprSubscriptNode : public SeExprNode
{
public:
    SeExprSubscriptNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprNode(expr, a, b) {}

    virtual SeExprType prep(SeExprType wanted, SeExprVarEnv & env);
    virtual void eval(SeVec3d& result) const;
};


/// Node that implements a numeric comparison
class SeExprCompareEqNode : public SeExprNode
{
public:
    SeExprCompareEqNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprNode(expr, a, b) {}

    virtual SeExprType prep(SeExprType wanted, SeExprVarEnv & env);
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
class SeExprCompareNode : public SeExprNode
{
public:
    SeExprCompareNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprNode(expr, a, b) {}

    virtual SeExprType prep(SeExprType wanted, SeExprVarEnv & env);
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


/// Node that implements an binary operator
class SeExprBinaryOpNode : public SeExprNode
{
public:
    SeExprBinaryOpNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprNode(expr, a, b) {}

    virtual SeExprType prep(SeExprType wanted, SeExprVarEnv & env);
};


/// Node that implements an arithmetic operator
class SeExprAddNode : public SeExprBinaryOpNode
{
public:
    SeExprAddNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprBinaryOpNode(expr, a, b) {}

    virtual void eval(SeVec3d& result) const;
};


/// Node that implements an arithmetic operator
class SeExprSubNode : public SeExprBinaryOpNode
{
public:
    SeExprSubNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprBinaryOpNode(expr, a, b) {}

    virtual void eval(SeVec3d& result) const;
};


/// Node that implements an arithmetic operator
class SeExprMulNode : public SeExprBinaryOpNode
{
public:
    SeExprMulNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprBinaryOpNode(expr, a, b) {}

    virtual void eval(SeVec3d& result) const;
};


/// Node that implements an arithmetic operator
class SeExprDivNode : public SeExprBinaryOpNode
{
public:
    SeExprDivNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprBinaryOpNode(expr, a, b) {}

    virtual void eval(SeVec3d& result) const;
};


/// Node that implements an arithmetic operator
class SeExprModNode : public SeExprBinaryOpNode
{
public:
    SeExprModNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprBinaryOpNode(expr, a, b) {}

    virtual void eval(SeVec3d& result) const;
};


/// Node that implements an arithmetic operator
class SeExprExpNode : public SeExprBinaryOpNode
{
public:
    SeExprExpNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprBinaryOpNode(expr, a, b) {}

    virtual void eval(SeVec3d& result) const;
};

/// Node that references a variable
class SeExprVarNode : public SeExprNode
{
public:
    SeExprVarNode(const SeExpression* expr, const char* name) :
	SeExprNode(expr), _name(name), _var(0), _data(0) 
    { expr->addVar(name); }

    SeExprVarNode(const SeExpression* expr, const char* name, const SeExprType & type) :
       SeExprNode(expr, type), _name(name), _var(0), _data(0)
    { expr->addVar(name); }

    virtual SeExprType prep(SeExprType wanted, SeExprVarEnv & env);
    virtual void eval(SeVec3d& result) const;
    const char* name() const { return _name.c_str(); }
    
    /// base class for custom instance data
    struct Data { virtual ~Data() {} };
    void setData(Data* data) const { _data = data; }
    Data* getData() const { return _data; }

private:
    std::string _name;
    SeExprVarRef* _var;
    mutable Data* _data;
};


/// Node that stores a numeric constant
class SeExprNumNode : public SeExprNode
{
public:
    SeExprNumNode(const SeExpression* expr, double val) :
	SeExprNode(expr), _val(val) {}

    virtual SeExprType prep(SeExprType wanted, SeExprVarEnv & env);
    virtual void eval(SeVec3d& result) const { result[0] = _val; }

    double value() const { return _val; };

private:
    double _val;
};


/// Node that stores a string
class SeExprStrNode : public SeExprNode
{
public:
    SeExprStrNode(const SeExpression* expr, const char* str) :
	SeExprNode(expr), _str(str) {}

    virtual SeExprType prep(SeExprType wanted, SeExprVarEnv & env);
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

    //! return true if no errors
    bool prepArgs(std::string const & name, SeExprType wanted, SeExprVarEnv & env);

    virtual SeExprType prep(SeExprType wanted, SeExprVarEnv & env);
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
