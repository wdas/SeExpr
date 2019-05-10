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
#ifndef _ExprFuncX_h_
#define _ExprFuncX_h_

#include "ExprType.h"
#include "Vec.h"
#include "ExprNode.h"

namespace SeExpr2 {
class ExprFuncDeclaration;
class ExprFuncNode;
class ExprFuncSimple;
class ExprVarEnv;
class Interpreter;
typedef std::map<std::string, double> Statistics;

//! Extension function spec, used for complicated argument custom functions.
/** Provides the ability to handle all argument type checking and processing manually.
    Derive from this class and then make your own ExprFunc that takes this object.
    This is necessary if you need string arguments or you have variable numbers of
    arguments.  See ExprBuiltins.h for some examples */
class ExprFuncX {
  public:
    //! Create an ExprFuncX. If the functions and type checking you implement
    //! is thread safe your derived class should call this with true. If not,
    //! then false.  If you mark a function as thread unsafe,  and it is used
    //! in an expression then bool Expression::isThreadSafe() will return false
    //! and the controlling software should not attempt to run multiple threads
    //! of an expression.
    ExprFuncX(const bool threadSafe) : _threadSafe(threadSafe)
    {
    }

    /** prep the expression by doing all type checking argument checking, etc. */
    virtual ExprType prep(ExprFuncNode* node, bool scalarWanted, ExprVarEnvBuilder& env) const = 0;
    virtual ExprType type() const
    {
        return _type;
    }

    /** evaluate the expression. the given node is where in the parse tree
        the evaluation is for */
    // virtual void eval(const ExprFuncNode* node, Vec3d& result) const = 0;
    //! Build an interpreter to evaluate the expression
    virtual int buildInterpreter(const ExprFuncNode* node, Interpreter* interpreter) const = 0;
    virtual ~ExprFuncX()
    {
    }

    bool isThreadSafe() const
    {
        return _threadSafe;
    }

    /// Return memory usage of a funcX in bytes.
    virtual size_t sizeInBytes() const
    {
        return 0;
    }

    /// Give this function a chance to populate its statistics
    virtual void statistics(Statistics& /*statistics*/) const
    {
    }

    // This only exists because there is an outstanding bug in SeExpr2. A function call like map('/disk1/tmp/cube.ptx')
    // is always marked by SeExpr2's type system as being constant, despite the plugin function specifying a return type
    // of ExprType().FP(3).Varying() in it's prep() implementation. This is because SeExpr2 sees that it has a single
    // constant string argument, and assumes it knows better than the plugin function and overrides the return type.
    //
    // This is a major issue for some calling applications that use expression constness as an indicator on whether or
    // not to do data computation in preparation for expression evaluation. These sort of optimizations become either
    // functionally broken, or undoable, under the coniditoins of this type system bug. This API is an escape hatch to
    // keep calling application optimizations.
    virtual bool forceReturnType() const
    {
        return false;
    }

  protected:
    bool _isScalar;
    ExprType _type;

  private:
    bool _threadSafe;
};

// This is used for most plugin functions
class ExprFuncSimple : public ExprFuncX {
  public:
    ExprFuncSimple(const bool threadSafe) : ExprFuncX(threadSafe)
    {
    }

    class ArgHandle {
      public:
        ArgHandle(const int* opData_, double* fp_, char** c_, const char* v)
            : outFp(fp_[opData_[2]])
            , outStr(c_[opData_[2]])
            , data(reinterpret_cast<ExprFuncNode::Data*>(c_[opData_[1]]))
            , varBlock(v)
            , _nargs((int)fp_[opData_[3]])  // TODO: put the value in opData rather than fp
            , opData(opData_ + 4)           // TODO: would be good not to have to convert to int!
            , fp(fp_)
            , c(c_)
        {
        }

        template <int d>
        Vec<double, d, true> inFp(int i)
        {
            return Vec<double, d, true>(&fp[opData[i]]);
        }
        template <int d>
        Vec<const double, d, true> inFp(int i) const
        {
            return Vec<const double, d, true>(&fp[opData[i]]);
        }

        char* inStr(int i)
        {
            return c[opData[i]];
        }

        const char* inStr(int i) const
        {
            return c[opData[i]];
        }

        int nargs() const
        {
            return _nargs;
        }

        /// Return a vector handle which is easier to assign to
        template <int d>
        Vec<double, d, true> outFpHandle()
        {
            return Vec<double, d, true>(&outFp);
        }

        double& outFp;
        char*& outStr;
        const ExprFuncNode::Data* data;
        const char* varBlock;

      private:
        int _nargs;
        const int* opData;
        double* fp;
        char** c;
        // std::stack<int>& callStack;
    };

    virtual int buildInterpreter(const ExprFuncNode* node, Interpreter* interpreter) const;

    virtual ExprType prep(ExprFuncNode* node, bool scalarWanted, ExprVarEnvBuilder& envBuilder) const = 0;

    virtual ExprFuncNode::Data* evalConstant(const ExprFuncNode*, ArgHandle&) const
    {
        return ExprFuncNode::NoData();
    }

    virtual void eval(ArgHandle& args) = 0;

    static ExprType genericPrep(ExprFuncNode* node,
                                bool scalarWanted,
                                ExprVarEnvBuilder& env,
                                const ExprFuncDeclaration& decl);

  private:
    static int EvalOp(const int* opData, double* fp, char** c);
};

template <typename FunctionCodeStorage>
class ExprFuncClosure : public ExprFuncSimple {
  public:
    ExprFuncClosure() : ExprFuncSimple(true), _callable(nullptr)
    {
    }
    explicit ExprFuncClosure(FunctionCodeStorage&& callable_) : ExprFuncSimple(true), _callable(std::move(callable_))
    {
    }

    virtual ExprType prep(ExprFuncNode*, bool, ExprVarEnvBuilder&) const override
    {
        assert(false);
        return ExprType().Error();
    }

    virtual inline void eval(ArgHandle& args) override
    {
        assert(_callable);
        _callable(args);
    }

  private:
    FunctionCodeStorage _callable;
};

class ExprFuncLocal : public ExprFuncX {
    ExprFuncLocal() : ExprFuncX(true)
    {
    }

    /** prep the expression by doing all type checking argument checking, etc. */
    virtual ExprType prep(ExprFuncNode* node, bool scalarWanted, ExprVarEnvBuilder& envBuilder) const;
    //! Build an interpreter to evaluate the expression
    virtual int buildInterpreter(const ExprFuncNode* node, Interpreter* interpreter) const;
};
}

#endif
