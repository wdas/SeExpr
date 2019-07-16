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

#ifndef ExprNode_h
#define ExprNode_h

#include <cstdlib>

// TODO: get rid of makedepends everywhere
#ifndef MAKEDEPEND
#include <string.h>
#include <string>
#include <vector>
#endif

#include "ExprConfig.h"
#include "ExprLLVM.h"
#include "Expression.h"
#include "ExprType.h"
#include "ExprEnv.h"
#include "Vec.h"
#include "Interpreter.h"

namespace SeExpr2 {
class ExprFunc;
class ExprFuncX;

/** Expression node base class.  Always constructed by parser in ExprParser.y
   Parse tree nodes - this is where the expression evaluation happens.

   Some implementation notes:

   1) Vector vs scalar - Any node can accept vector or scalar inputs
   and return a vector or scalar result.  If a node returns a scalar,
   it is only required to set the [0] component and the other
   components must be assumed to be invalid.

   2) ExprNode::prep - This is called for all nodes during parsing
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

class ExprNode {
  public:
    ExprNode(const Expression* expr);
    ExprNode(const Expression* expr, const ExprType& type);
    /// @{ @name These constructors supply one or more children.
    ExprNode(const Expression* expr, ExprNode* a);
    ExprNode(const Expression* expr, ExprNode* a, const ExprType& type);
    ExprNode(const Expression* expr, ExprNode* a, ExprNode* b);
    ExprNode(const Expression* expr, ExprNode* a, ExprNode* b, const ExprType& type);
    ExprNode(const Expression* expr, ExprNode* a, ExprNode* b, ExprNode* c);
    ExprNode(const Expression* expr, ExprNode* a, ExprNode* b, ExprNode* c, const ExprType& type);
    /// @}
    virtual ~ExprNode();

    /// @{ @name Interface to implement for subclasses

    /// Prepare the node (for parser use only).  See the discussion at
    /// the start of SeExprNode.cpp for more info.
    virtual ExprType prep(bool dontNeedScalar, ExprVarEnvBuilder& envBuilder);

    /// builds an interpreter. Returns the location index for the evaluated data
    virtual int buildInterpreter(Interpreter* interpreter) const;
    /// @}

    virtual LLVM_VALUE codegen(LLVM_BUILDER) LLVM_BODY;

    /// True if node has a vector result.
    bool isVec() const { return _isVec; }

    /// Access expression
    const Expression* expr() const { return _expr; }

    /// Access to original string representation of current expression
    std::string toString() const {
        return expr()->getExpr().substr(startPos(), length());
    };

    /// @{ @name Relationship Queries and Manipulation

    /// Access parent node - root node has no parent
    const ExprNode* parent() const { return _parent; }
    /// Number of children
    int numChildren() const { return static_cast<int>(_children.size()); }

    /// Get 0 indexed child
    const ExprNode* child(size_t i) const { return _children[i]; }

    /// Get 0 indexed child
    ExprNode* child(size_t i) { return _children[i]; }

    /// Swap children, do not use unless you know what you are doing
    void swapChildren(size_t i, size_t j) {
        assert(i != j && i < _children.size() && j < _children.size());
        std::swap(_children[i], _children[j]);
    }

    /// Remove last child and delete the entry
    void removeLastChild() {
        if (_children.size()) {
            delete _children.back();
            _children.pop_back();
        }
    }

    /// Add a child to the child list (for parser use only)
    void addChild(ExprNode* child);

    /// Transfer children from surrogate parent (for parser use only)
    void addChildren(ExprNode* surrogate);

    /// @}

    /// The type of the node
    const ExprType& type() const {
        return _type;
    };

    /// @{ @name Access position in the input buffer that created this node

    /// Remember the line and column position in the input string
    inline void setPosition(const short int startPos, const short int endPos) {
        _startPos = startPos;
        _endPos = endPos;
    }
    /// Access start position in input string
    inline short int startPos() const { return _startPos; }
    /// Access end position in input string
    inline short int endPos() const { return _endPos; }
    /// Access length of input string
    inline short int length() const {
        return endPos() - startPos();
    };

    /// @}

    /// Register error. This will allow users and sophisticated editors to highlight where in code problem was
    inline void addError(const std::string& error) const { _expr->addError(error, _startPos, _endPos); }

  protected: /*protected functions*/
    //! Set type of parameter
    inline void setType(const ExprType& t) {
        _type = t;
    };
    //! Set's the type to the argument but uses the children to determine lifetime
    inline void setTypeWithChildLife(const ExprType& t) {
        setType(t);
        int num = numChildren();
        if (num > 0) {
            _type.setLifetime(child(0)->type());
            for (int i = 1; i < num; i++) _type.setLifetime(_type, child(i)->type());
        } else  // no children life is constant!
            _type.Constant();
    };

    /// @{ @name Error Checking Helpers for Type Checking

  public:
    /// Checks the boolean value and records an error string with node if it is false
    inline bool checkCondition(bool check, const std::string& message, bool& error) {
        if (!check) {
            addError(message);
            error = true;
        }
        return check;
    };
    /// Checks if the type is a value (i.e. string or float[d])
    bool checkIsValue(const ExprType& type, bool& error) {
        return checkCondition(type.isValue(), "Expected String or Float[d]", error);
    }
    /// Checks if the type is a float[d] for any d
    bool checkIsFP(const ExprType& type, bool& error) {
        return checkCondition(type.isFP(), "Expected Float[d]", error);
    }
    /// Checks if the type is a float[d] for a specific d
    bool checkIsFP(int d, const ExprType& type, bool& error) {
        if (!type.isFP(d)) {  // Defer creating expensive string creation unless error
            std::stringstream s;
            s << "Expected Float[" << d << "]" << std::endl;
            return checkCondition(false, s.str(), error);
        }
        return false;
    }
    /// types match (true if they do)
    inline bool checkTypesCompatible(const ExprType& first, const ExprType& second, bool& error) {
        if (!ExprType::valuesCompatible(first, second)) {
            return checkCondition(
                false, "Type mismatch. First: " + first.toString() + " Second: " + second.toString(), error);
        } else
            return false;
    }
    /// @}
  protected: /*protected data members*/
    /// Owning expression (node can't modify)
    const Expression* _expr;

    /// Parent node (null if this the the root)
    ExprNode* _parent;

    /// List of children
    std::vector<ExprNode*> _children;

    /// True if node has a vector result
    bool _isVec;

    // Type of node
    ExprType _type;
    int _maxChildDim;

    /// Position line and collumn
    unsigned short int _startPos, _endPos;
};

/// Node that contains entire program
class ExprModuleNode : public ExprNode {
  public:
    ExprModuleNode(const Expression* expr) : ExprNode(expr) {}

    virtual ExprType prep(bool wantScalar, ExprVarEnvBuilder& envBuilder);
    virtual int buildInterpreter(Interpreter* interpreter) const;
    virtual LLVM_VALUE codegen(LLVM_BUILDER) LLVM_BODY;
};

/// Node that contains prototype of function
class ExprPrototypeNode : public ExprNode {
  public:
    ExprPrototypeNode(const Expression* expr, const std::string& name, const ExprType& retType)
        : ExprNode(expr), _name(name), _retTypeSet(true), _retType(retType), _argTypes() {}

    ExprPrototypeNode(const Expression* expr, const std::string& name)
        : ExprNode(expr), _name(name), _retTypeSet(false), _argTypes() {}

    virtual ExprType prep(bool wantScalar, ExprVarEnvBuilder& envBuilder);

    void addArgTypes(ExprNode* surrogate);
    void addArgs(ExprNode* surrogate);

    inline void setReturnType(const ExprType& type) {
        _retType = type;
        _retTypeSet = true;
    };

    inline bool isReturnTypeSet() const {
        return _retTypeSet;
    };

    inline ExprType returnType() const {
        return (_retTypeSet ? _retType : ExprType().Error().Varying());
    };

    inline ExprType argType(int i) const {
        return _argTypes[i];
    };
    inline const ExprNode* arg(int i) const {
        return child(i);
    };

    const std::string& name() const { return _name; }

    /// Build the interpreter
    int buildInterpreter(Interpreter* interpreter) const;
    virtual LLVM_VALUE codegen(LLVM_BUILDER) LLVM_BODY;
    /// Return op for interpreter
    int interpreterOps(int c) const { return _interpreterOps.at(c); }

  private:
    std::string _name;
    bool _retTypeSet;
    ExprType _retType;
    std::vector<ExprType> _argTypes;
    mutable std::vector<int> _interpreterOps;  // operands for interpreter // TODO: this sucks... maybe a better place
                                               // for this.
};

class ExprFuncNode;
/// Node that contains local function
class ExprLocalFunctionNode : public ExprNode {
  public:
    ExprLocalFunctionNode(const Expression* expr, ExprPrototypeNode* prototype, ExprNode* block)
        : ExprNode(expr, prototype, block) {}

    /// Preps the definition of this site
    virtual ExprType prep(bool wantScalar, ExprVarEnvBuilder& envBuilder);
    /// Preps a caller (i.e. we use callerNode to check arguments)
    virtual ExprType prep(ExprFuncNode* callerNode, bool scalarWanted, ExprVarEnvBuilder& envBuilder) const;
    /// TODO: Accessor for prototype (probably not needed when we use prep right)
    const ExprPrototypeNode* prototype() const { return static_cast<const ExprPrototypeNode*>(child(0)); }

    /// Build the interpreter
    int buildInterpreter(Interpreter* interpreter) const;
    /// Build interpreter if we are called
    int buildInterpreterForCall(const ExprFuncNode* callerNode, Interpreter* interpreter) const;
    virtual LLVM_VALUE codegen(LLVM_BUILDER) LLVM_BODY;

  private:
    mutable int _procedurePC;
    mutable int _returnedDataOp;
};

/// Node that computes local variables before evaluating expression
class ExprBlockNode : public ExprNode {
  public:
    ExprBlockNode(const Expression* expr, ExprNode* a, ExprNode* b) : ExprNode(expr, a, b) {}

    virtual ExprType prep(bool wantScalar, ExprVarEnvBuilder& envBuilder);
    virtual int buildInterpreter(Interpreter* interpreter) const;
    virtual LLVM_VALUE codegen(LLVM_BUILDER) LLVM_BODY;
};

/// Node that computes local variables before evaluating expression
class ExprIfThenElseNode : public ExprNode {
  public:
    ExprIfThenElseNode(const Expression* expr, ExprNode* a, ExprNode* b, ExprNode* c)
        : ExprNode(expr, a, b, c), _varEnv(nullptr), _varEnvMergeIndex(0) {}

    virtual ExprType prep(bool wantScalar, ExprVarEnvBuilder& envBuilder);
    virtual int buildInterpreter(Interpreter* interpreter) const;
    virtual LLVM_VALUE codegen(LLVM_BUILDER) LLVM_BODY;

    ExprVarEnv* _varEnv;
    size_t _varEnvMergeIndex;
};

/// Node that compute a local variable assignment
class ExprAssignNode : public ExprNode {
  public:
    ExprAssignNode(const Expression* expr, const char* name, ExprNode* e)
        : ExprNode(expr, e), _name(name), _localVar(0) {}

    virtual ExprType prep(bool wantScalar, ExprVarEnvBuilder& envBuilder);
    virtual int buildInterpreter(Interpreter* interpreter) const;
    // virtual void eval(Vec3d& result) const;
    virtual LLVM_VALUE codegen(LLVM_BUILDER) LLVM_BODY;

    const std::string& name() const {
        return _name;
    };
    const ExprType& assignedType() const {
        return _assignedType;
    };
    const ExprLocalVar* localVar() const { return _localVar; }

  private:
    std::string _name;
    ExprLocalVar* _localVar;
    ExprType _assignedType;
};

// TODO three scalars?  Or 2 to 16 scalars??
/// Node that constructs a vector from three scalars
class ExprVecNode : public ExprNode {
  public:
    ExprVecNode(const Expression* expr) : ExprNode(expr) {}

    virtual ExprType prep(bool wantScalar, ExprVarEnvBuilder& envBuilder);
    virtual int buildInterpreter(Interpreter* interpreter) const;
    virtual LLVM_VALUE codegen(LLVM_BUILDER) LLVM_BODY;

    Vec3d value() const;
};

/// NOde that computes with a single operand
class ExprUnaryOpNode : public ExprNode {
  public:
    //! Construct with specific op ('!x' is logical negation, '~x' is 1-x, '-x' is -x)
    ExprUnaryOpNode(const Expression* expr, ExprNode* a, char op) : ExprNode(expr, a), _op(op) {}

    virtual ExprType prep(bool wantScalar, ExprVarEnvBuilder& envBuilder);
    virtual int buildInterpreter(Interpreter* interpreter) const;
    virtual LLVM_VALUE codegen(LLVM_BUILDER) LLVM_BODY;

    char _op;
};

/// Node that evaluates a conditional (if-then-else) expression
class ExprCondNode : public ExprNode {
  public:
    ExprCondNode(const Expression* expr, ExprNode* a, ExprNode* b, ExprNode* c) : ExprNode(expr, a, b, c) {}

    virtual ExprType prep(bool wantScalar, ExprVarEnvBuilder& envBuilder);
    virtual int buildInterpreter(Interpreter* interpreter) const;
    virtual LLVM_VALUE codegen(LLVM_BUILDER) LLVM_BODY;
};

/// Node that evaluates a component of a vector
class ExprSubscriptNode : public ExprNode {
  public:
    ExprSubscriptNode(const Expression* expr, ExprNode* a, ExprNode* b) : ExprNode(expr, a, b) {}

    virtual ExprType prep(bool wantScalar, ExprVarEnvBuilder& envBuilder);
    virtual int buildInterpreter(Interpreter* interpreter) const;
    virtual LLVM_VALUE codegen(LLVM_BUILDER) LLVM_BODY;
};

/// Node that implements a numeric/string comparison
class ExprCompareEqNode : public ExprNode {
  public:
    ExprCompareEqNode(const Expression* expr, ExprNode* a, ExprNode* b, char op) : ExprNode(expr, a, b), _op(op) {}

    virtual ExprType prep(bool wantScalar, ExprVarEnvBuilder& envBuilder);
    virtual int buildInterpreter(Interpreter* interpreter) const;
    virtual LLVM_VALUE codegen(LLVM_BUILDER) LLVM_BODY;

    char _op;
};

/// Node that implements a numeric comparison
class ExprCompareNode : public ExprNode {
  public:
    ExprCompareNode(const Expression* expr, ExprNode* a, ExprNode* b, char op) : ExprNode(expr, a, b), _op(op) {}

    virtual ExprType prep(bool wantScalar, ExprVarEnvBuilder& envBuilder);
    virtual int buildInterpreter(Interpreter* interpreter) const;
    virtual LLVM_VALUE codegen(LLVM_BUILDER) LLVM_BODY;

    //! _op '<' less-than, 'l' less-than-eq, '>' greater-than, 'g' greater-than-eq
    char _op;
};

/// Node that implements an binary operator
class ExprBinaryOpNode : public ExprNode {
  public:
    ExprBinaryOpNode(const Expression* expr, ExprNode* a, ExprNode* b, char op) : ExprNode(expr, a, b), _op(op), _out(0) {}
    virtual ~ExprBinaryOpNode() { free(_out); }

    virtual ExprType prep(bool wantScalar, ExprVarEnvBuilder& envBuilder);
    virtual int buildInterpreter(Interpreter* interpreter) const;
    virtual LLVM_VALUE codegen(LLVM_BUILDER) LLVM_BODY;

    char _op;
    char* _out;
};

/// Node that references a variable
class ExprVarNode : public ExprNode {
  public:
    ExprVarNode(const Expression* expr, const char* name) : ExprNode(expr), _name(name), _localVar(0), _var(0) {}

    ExprVarNode(const Expression* expr, const char* name, const ExprType& type)
        : ExprNode(expr, type), _name(name), _localVar(0), _var(0) {}

    virtual ExprType prep(bool wantScalar, ExprVarEnvBuilder& envBuilder);
    virtual int buildInterpreter(Interpreter* interpreter) const;
    virtual LLVM_VALUE codegen(LLVM_BUILDER) LLVM_BODY;
    const char* name() const { return _name.c_str(); }
    const ExprLocalVar* localVar() const { return _localVar; }
    const ExprVarRef* var() const { return _var; }

  private:
    std::string _name;
    ExprLocalVar* _localVar;
    ExprVarRef* _var;
};

/// Node that stores a numeric constant
class ExprNumNode : public ExprNode {
  public:
    ExprNumNode(const Expression* expr, double val) : ExprNode(expr), _val(val) {}

    virtual ExprType prep(bool wantScalar, ExprVarEnvBuilder& envBuilder);
    virtual int buildInterpreter(Interpreter* interpreter) const;
    virtual LLVM_VALUE codegen(LLVM_BUILDER) LLVM_BODY;
    double value() const {
        return _val;
    };

  private:
    double _val;
};

/// Node that stores a string
class ExprStrNode : public ExprNode {
  public:
    ExprStrNode(const Expression* expr, const char* str);

    virtual ExprType prep(bool wantScalar, ExprVarEnvBuilder& envBuilder);
    virtual int buildInterpreter(Interpreter* interpreter) const;
    virtual LLVM_VALUE codegen(LLVM_BUILDER) LLVM_BODY;
    const char* str() const { return _str.c_str(); }
    void str(const char* newstr) { _str = newstr; }

  private:
    std::string _str;
};

/// Node that calls a function
class ExprFuncNode : public ExprNode {
  public:
    ExprFuncNode(const Expression* expr, const char* name)
        : ExprNode(expr), _name(name), _func(0), _localFunc(0), _data(0) {
        expr->addFunc(name);
    }
    virtual ~ExprFuncNode() {
        if (_data != nullptr && _data->_cleanup == true) {
            delete _data;
        }
    }

    virtual ExprType prep(bool wantScalar, ExprVarEnvBuilder& envBuilder);
    virtual int buildInterpreter(Interpreter* interpreter) const;
    virtual LLVM_VALUE codegen(LLVM_BUILDER) LLVM_BODY;

    const char* name() const { return _name.c_str(); }
    bool checkArg(int argIndex, ExprType type, ExprVarEnvBuilder& envBuilder);

#if 0
    virtual void eval(Vec3d& result) const;
    void setIsVec(bool isVec) { _isVec = isVec; }

    //! return the number of arguments
    int nargs() const { return _nargs; }

#if 0
    double* scalarArgs() const { return &_scalarArgs[0]; }
    Vec3d* vecArgs() const { return &_vecArgs[0]; }

    //! eval all arguments (use in eval())
    Vec3d* evalArgs() const;

    //! eval an argument (use in eval())
    Vec3d evalArg(int n) const;

    //! returns whether the nth argument is a string (use in prep)
    bool isStrArg(int n) const;

    //! get nth string argument (use in prep)
    std::string getStrArg(int n) const;
#endif

#endif

    // TODO: Remove those two methods.
    bool isStrArg(int n) const { return n < numChildren() && dynamic_cast<const ExprStrNode*>(child(n)) != 0; }
    std::string getStrArg(int n) const {
        if (n < numChildren()) return static_cast<const ExprStrNode*>(child(n))->str();
        return "";
    }

    //! base class for custom instance data
    struct Data {
        Data(bool cleanup = false) : _cleanup(cleanup) {}
        virtual ~Data() {}
        bool _cleanup;
    };

    //! associate blind data with this node (subsequently owned by this object)
    /***
        Use this to set data associated with the node. Equivalently this is data
        associated with a specific evaluation point of a function.
        Examples would be tokenized values,
        sorted lists for binary searches in curve evaluation, etc. This should be done
        in ExprFuncX::prep().
    */
    void setData(Data* data) const { _data = data; }

    //! get associated blind data (returns 0 if none)
    /***
        Use this to get data associated in the prep() routine. This is typically
        used from ExprFuncX::eval()
    */
    Data* getData() const { return _data; }
    int promote(int i) const { return _promote[i]; }
    const ExprFunc* func() const { return _func; }

  private:
    std::string _name;
    const ExprFunc* _func;
    const ExprLocalFunctionNode* _localFunc;  // TODO: it is dirty to have to have both.
                                              //    int _nargs;
                                              //    mutable std::vector<double> _scalarArgs;
                                              //    mutable std::vector<Vec3d> _vecArgs;
    mutable std::vector<int> _promote;
    mutable Data* _data;
};

/// Policy which provides all the AST Types for the parser.
class ExprNodePolicy {
    typedef ExprNode Base;
    // TODO: fix this once we switch to a c++11 compiler
    // typedef std::unique_ptr<Base*> Ptr;

    typedef ExprModuleNode Module;
    typedef ExprPrototypeNode Prototype;
    typedef ExprLocalFunctionNode LocalFunction;
    typedef ExprBlockNode Block;
    typedef ExprIfThenElseNode IfThenElse;
    typedef ExprAssignNode Assign;
    typedef ExprVecNode Vec;
    typedef ExprUnaryOpNode UnaryOp;
    typedef ExprCondNode Cond;
    typedef ExprCompareEqNode CompareEq;
    typedef ExprCompareNode Compare;
    typedef ExprBinaryOpNode BinaryOp;
    typedef ExprVarNode Var;
    typedef ExprNumNode Num;
    typedef ExprStrNode Str;
    typedef ExprFuncNode Func;
};
}

#endif
