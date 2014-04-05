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
#include "SeInterpreter.h"

class SeExprFunc;
class SeExprFuncX;

/** Expression node base class.  Always constructed by parser in SeExprParser.y
   Parse tree nodes - this is where the expression evaluation happens.

   Some implementation notes: 

   1) Vector vs scalar - Any node can accept vector or scalar inputs
   and return a vector or scalar result.  If a node returns a scalar,
   it is only required to set the [0] component and the other
   components must be assumed to be invalid.

   2) SeExprNode::prep - This is called for all nodes during parsing
   once the syntax has been checked.  Anything can be done here, such
   as function binding, variable lookups, etc., but the only thing
   that must be done is to determine whether the result is going to be
   vector or scalar.  This can in some cases depend on whether the
   children are vector or scalar so the parser calls prep on the root
   node and each node is expected to call prep on its children and
   then set its own _isVec variable.  The wantVec param provides
   context from the parent (and ultimately the owning expression) as
   to whether a vector is desired, but nodes are not bound by this and
   may produce a scalar even when a vector is wanted.
   
   The base class method implements the default behavior which is to
   pass down the wantVec flag to all children and set isVec to true if
   any child is a vec.

   If the prep method fails, an error string should be set and false
   should be returned.
*/

class SeExprNode {
public:
    SeExprNode(const SeExpression* expr);
    SeExprNode(const SeExpression* expr,
               const SeExprType & type);
    /// @{ @name These constructors supply one or more children.
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
    /// @}
    virtual ~SeExprNode();


    /// @{ @name Interface to implement for subclasses

    /// Prepare the node (for parser use only).  See the discussion at
    /// the start of SeExprNode.cpp for more info.
    virtual SeExprType prep(bool dontNeedScalar, SeExprVarEnv & env);

    /// builds an interpreter. Returns the location index for the evaluated data
    virtual int buildInterpreter(SeInterpreter* interpreter) const;
    /// @}
    
    /// True if node has a vector result.
    bool isVec() const { return _isVec; }

    /// Access expression
    const SeExpression* expr() const { return _expr; }

    /// Access to original string representation of current expression
    std::string toString() const { return expr()->getExpr().substr(startPos(), length()); };

    /// @{ @name Relationship Queries and Manipulation

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

    /// @}

    /// The type of the node
    const SeExprType & type() const { return _type; };

    /// @{ @name Access position in the input buffer that created this node 

    /// Remember the line and column position in the input string 
    inline void setPosition(const short int startPos,const short int endPos)
    {_startPos=startPos;_endPos=endPos;}
    /// Access start position in input string
    inline short int startPos() const { return _startPos; }
    /// Access end position in input string
    inline short int endPos() const { return _endPos; }
    /// Access length of input string
    inline short int length() const { return endPos() - startPos(); };

    /// @}

    /// Register error. This will allow users and sophisticated editors to highlight where in code problem was
    inline void addError(const std::string& error) { _expr->addError(error, _startPos, _endPos); }

 protected: /*protected functions*/

    //! Set type of parameter
    inline void setType         (const SeExprType & t)  { _type = t;                                    };
    //! Set's the type to the argument but uses the children to determine lifetime
    inline void setTypeWithChildLife(const SeExprType & t)  {
        setType(t);
        int num = numChildren();
        if(num > 0){
            _type.setLifetime(child(0)->type());
            for(int i = 1; i < num; i++)
                _type.setLifetime(_type, child(i)->type());
        }else // no children life is constant!
            _type.Constant();
    };

    /// @{ @name Error Checking Helpers for Type Checking

public:
    /// Checks the boolean value and records an error string with node if it is false 
    inline bool checkCondition(bool check,const std::string & message,bool& error) {
        if(!check) {
            addError(message);
            error = true;
        }
        return check;
    };
    /// Checks if the type is a value (i.e. string or float[d])
    bool checkIsValue(const SeExprType& type,bool& error){
        return checkCondition(type.isValue(),"Expected String or Float[d]",error);
    }
    /// Checks if the type is a float[d] for any d 
    bool checkIsFP(const SeExprType& type,bool& error){
        return checkCondition(type.isFP(),"Expected Float[d]",error);
    }
    /// Checks if the type is a float[d] for a specific d
    bool checkIsFP(int d,const SeExprType& type,bool& error){
        if(!type.isFP(d)){ // Defer creating expensive string creation unless error
            std::stringstream s;
            s<<"Expected Float["<<d<<"]"<<std::endl;
            return checkCondition(false,s.str(),error);
        }
        return false;
    }
    /// types match (true if they do)
    inline bool checkTypesCompatible(const SeExprType & first,const SeExprType & second,bool & error) {
        if(!SeExprType::valuesCompatible(first,second)){
            return checkCondition(false,
                "Type mismatch. First: " + first .toString() +
                " Second: "              + second.toString(),error);
        }else return false;
    }
    /// @}
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
    int _maxChildDim;

    /// Position line and collumn
    unsigned short int _startPos,_endPos;

    /// Fast evaluation function pointer
    typedef void(*EvalFunction)(SeExprNode* self,const SeExprEvalResult& result);
public:
    EvalFunction evaluate;
};


/// Node that contains entire program
class SeExprModuleNode : public SeExprNode
{
public:
    SeExprModuleNode(const SeExpression* expr) :
	SeExprNode(expr)
    {}

    virtual SeExprType prep(bool wantScalar, SeExprVarEnv & env);
    virtual int buildInterpreter(SeInterpreter* interpreter) const;
};


/// Node that contains prototype of function
class SeExprPrototypeNode : public SeExprNode
{
public:
    SeExprPrototypeNode(const SeExpression * expr,
                        const std::string  & name,
                        const SeExprType   & retType)
        : SeExprNode(expr),
        _name(name),
        _retTypeSet(true),
        _retType(retType),
        _argTypes()
    {}

    SeExprPrototypeNode(const SeExpression * expr,
                        const std::string  & name)
        : SeExprNode(expr),
        _name(name),
        _retTypeSet(false),
        _argTypes()
    {}

    virtual SeExprType prep(bool wantScalar, SeExprVarEnv & env);

    void addArgTypes(SeExprNode * surrogate);
    void addArgs    (SeExprNode * surrogate);

    inline void setReturnType(const SeExprType & type) { _retType = type; _retTypeSet = true; };

    inline bool isReturnTypeSet() const { return _retTypeSet; };

    inline SeExprType returnType() const { return (_retTypeSet ? _retType : SeExprType().Error().Varying()); };

    inline       SeExprType     argType(int i) const { return _argTypes[i]; };
    inline const SeExprNode   * arg    (int i) const { return child(i);     };

    const std::string& name() const{return _name;}
 private:
    std::string             _name;
    bool                    _retTypeSet;
    SeExprType              _retType;
    std::vector<SeExprType> _argTypes;
};


class SeExprFuncNode;
/// Node that contains local function
class SeExprLocalFunctionNode : public SeExprNode
{
public:
    SeExprLocalFunctionNode(const SeExpression        * expr,
                                  SeExprPrototypeNode * prototype,
                                  SeExprNode          * block)
        : SeExprNode(expr, prototype, block)
    {}

    /// Preps the definition of this site
    virtual SeExprType prep(bool wantScalar, SeExprVarEnv & env);
    /// Preps a caller (i.e. we use callerNode to check arguments)
    virtual SeExprType prep(SeExprFuncNode* callerNode,bool scalarWanted,SeExprVarEnv& env) const;
    /// TODO: Accessor for prototype (probably not needed when we use prep right)
    const SeExprPrototypeNode* prototype() const{return static_cast<const SeExprPrototypeNode*>(child(0));}
    
    /// Build the interpreter
    int buildInterpreter(SeInterpreter* interpreter) const;
    /// Build interpreter if we are called
    int buildInterpreterForCall(const SeExprFuncNode* callerNode, SeInterpreter* interpreter) const;
};


/// Node that computes local variables before evaluating expression
class SeExprBlockNode : public SeExprNode
{
public:
    SeExprBlockNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprNode(expr, a, b)
    {}

    virtual SeExprType prep(bool wantScalar, SeExprVarEnv & env);
    virtual int buildInterpreter(SeInterpreter* interpreter) const;
};


/// Node that computes local variables before evaluating expression
class SeExprIfThenElseNode : public SeExprNode
{
public:
    SeExprIfThenElseNode(const SeExpression* expr,
			 SeExprNode* a, SeExprNode* b, SeExprNode* c) :
	SeExprNode(expr, a, b, c) {}

    virtual SeExprType prep(bool wantScalar, SeExprVarEnv & env);
    virtual int buildInterpreter(SeInterpreter* interpreter) const;

    SeExprVarEnv thenEnv,elseEnv;
};


/// Node that compute a local variable assignment
class SeExprAssignNode : public SeExprNode
{
public:
    SeExprAssignNode(const SeExpression* expr, const char* name, SeExprNode* e) :
	SeExprNode(expr, e), _name(name), _localVar(0) {}

    virtual SeExprType prep(bool wantScalar, SeExprVarEnv & env);
    virtual int buildInterpreter(SeInterpreter* interpreter) const;
    //virtual void eval(SeVec3d& result) const;

    const std::string& name        () const { return _name;         };
    const SeExprType & assignedType() const { return _assignedType; };
    const SeExprLocalVar* localVar() const{return _localVar;}

private:
    std::string _name;
    SeExprLocalVar* _localVar;
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

    virtual SeExprType prep(bool wantScalar, SeExprVarEnv & env);
    virtual int buildInterpreter(SeInterpreter* interpreter) const;

    SeVec3d value() const;
};

/// NOde that computes with a single operand
class SeExprUnaryOpNode : public SeExprNode
{
public:
    //! Construct with specific op ('!x' is logical negation, '~x' is 1-x, '-x' is -x)
    SeExprUnaryOpNode(const SeExpression* expr, SeExprNode* a,char op)
        : SeExprNode(expr, a), _op(op)
    {}
    
    virtual SeExprType prep(bool wantScalar, SeExprVarEnv & env);
    virtual int buildInterpreter(SeInterpreter* interpreter) const;

    char _op;
};

/// Node that evaluates a conditional (if-then-else) expression
class SeExprCondNode : public SeExprNode
{
public:
    SeExprCondNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b, SeExprNode* c) :
	SeExprNode(expr, a, b, c) {}

    virtual SeExprType prep(bool wantScalar, SeExprVarEnv & env);
    virtual int buildInterpreter(SeInterpreter* interpreter) const;
};

/// Node that evaluates a component of a vector
class SeExprSubscriptNode : public SeExprNode
{
public:
    SeExprSubscriptNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b) :
	SeExprNode(expr, a, b){}

    virtual SeExprType prep(bool wantScalar, SeExprVarEnv & env);
    virtual int buildInterpreter(SeInterpreter* interpreter) const;
};


/// Node that implements a numeric/string comparison
class SeExprCompareEqNode : public SeExprNode
{
public:
    SeExprCompareEqNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b,char op) :
	SeExprNode(expr, a, b),_op(op) {}

    virtual SeExprType prep(bool wantScalar, SeExprVarEnv & env);
    virtual int buildInterpreter(SeInterpreter* interpreter) const;

    char _op;
};


/// Node that implements a numeric comparison
class SeExprCompareNode : public SeExprNode
{
public:
    SeExprCompareNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b,char op) :
	SeExprNode(expr, a, b), _op(op) {}

    virtual SeExprType prep(bool wantScalar, SeExprVarEnv & env);
    virtual int buildInterpreter(SeInterpreter* interpreter) const;

    //! _op '<' less-than, 'l' less-than-eq, '>' greater-than, 'g' greater-than-eq
    char _op;
};

/// Node that implements an binary operator
class SeExprBinaryOpNode : public SeExprNode
{
public:
    SeExprBinaryOpNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b, char op) :
	SeExprNode(expr, a, b),
       _op(op) {}

    virtual SeExprType prep(bool wantScalar, SeExprVarEnv & env);
    virtual int buildInterpreter(SeInterpreter* interpreter) const;

    char _op;
};


/// Node that references a variable
class SeExprVarNode : public SeExprNode
{
public:
    SeExprVarNode(const SeExpression* expr, const char* name) :
	SeExprNode(expr), _name(name), _localVar(0), _var(0)
    {}

    SeExprVarNode(const SeExpression* expr, const char* name, const SeExprType & type) :
        SeExprNode(expr, type), _name(name), _localVar(0), _var(0)
    {}

    virtual SeExprType prep(bool wantScalar, SeExprVarEnv & env);
    virtual int buildInterpreter(SeInterpreter* interpreter) const;
    const char* name() const { return _name.c_str(); }
    const SeExprLocalVar* localVar() const{return _localVar;}
    const SeExprVarRef* var() const{return _var;}

private:
    std::string _name;
    SeExprLocalVar* _localVar;
    SeExprVarRef* _var;
};


/// Node that stores a numeric constant
class SeExprNumNode : public SeExprNode
{
public:
    SeExprNumNode(const SeExpression* expr, double val) :
	SeExprNode(expr), _val(val) {}

    virtual SeExprType prep(bool wantScalar, SeExprVarEnv & env);
    virtual int buildInterpreter(SeInterpreter* interpreter) const;
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

    virtual SeExprType prep(bool wantScalar, SeExprVarEnv & env);
    virtual int buildInterpreter(SeInterpreter* interpreter) const;
    const char* str() const { return _str.c_str(); }

private:
    std::string _str;
};


/// Node that calls a function
class SeExprFuncNode : public SeExprNode
{
public:
    SeExprFuncNode(const SeExpression* expr, const char* name) :
	SeExprNode(expr), _name(name)//, _func(0), _nargs(0), _data(0) 
    {
	expr->addFunc(name);
    }
    virtual ~SeExprFuncNode() { /* TODO: fix delete _data;*/ }

    virtual SeExprType prep(bool wantScalar, SeExprVarEnv & env);
    virtual int buildInterpreter(SeInterpreter* interpreter) const;

    const char* name() const { return _name.c_str(); }
    bool checkArg(int argIndex,SeExprType type,SeExprVarEnv& env);

#if 0
    virtual void eval(SeVec3d& result) const;
    void setIsVec(bool isVec) { _isVec = isVec; }

    //! return the number of arguments
    int nargs() const { return _nargs; }

#if 0
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
#endif

#endif

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
    int promote(int i) const {return _promote[i];}
private:
    std::string _name;
    const SeExprFunc* _func;
    const SeExprLocalFunctionNode* _localFunc; // TODO: it is dirty to have to have both.
//    int _nargs;
//    mutable std::vector<double> _scalarArgs;
//    mutable std::vector<SeVec3d> _vecArgs;
    mutable std::vector<int> _promote;
    mutable Data* _data;
};

#endif
