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
#ifndef _Interpreter_h_
#define _Interpreter_h_

#include <vector>
#include <stack>

#include "Evaluator.h"
#include "ExprNode.h"

namespace SeExpr2 {
class ExprLocalVar;
class VarBlock;

//! Promotes a FP[1] to FP[d]
template <int d>
struct Promote {
    // TODO: this needs a name that is prefixed by Se!
    static int f(const int* opData, double* fp, char**)
    {
        int posIn = opData[0];
        int posOut = opData[1];
        for (int k = posOut; k < posOut + d; k++)
            fp[k] = fp[posIn];
        return 1;
    }
};

/// Non-LLVM manual interpreter. This is a simple computation machine. There are no dynamic activation records
/// just fixed locations, because we have no recursion!
class Interpreter : public Evaluator {
  public:
    // TODO: To make the Interpreter thread-safe, State must
    // be able to be thread-local (or if tbb is used, task-local).
    struct State {
        std::vector<double> d;  // double data (constants and evaluated)
        std::vector<char*> s;   // constant and evaluated pointer data
    };

    mutable State state;

    /// Not needed for eval only building
    typedef std::map<const ExprLocalVar*, int> VarToLoc;
    VarToLoc varToLoc;

    /// Op function pointer arguments are (int* currOpData,double* currD,char** c)
    typedef int (*OpF)(const int*, double*, char**);

    std::vector<std::pair<OpF, int> > ops;

    std::vector<int> opData;  // operands to op

  private:
    mutable std::mutex _m;
    bool _debugging;
    int _returnSlot;
    ExprType _desiredReturnType;
    bool _startedOp;
    int _pcStart;

  public:
    Interpreter() : _debugging(false), _returnSlot(0), _desiredReturnType(), _startedOp(false), _pcStart(0)
    {
        allocPtr();  // reserved for double** of variable block
        allocPtr();  // reserved for indirectIndex of variable block
    }

    /// Return the position that the next instruction will be placed at
    int nextPC()
    {
        return ops.size();
    }

    ///! adds an operator to the program (pointing to the data at the current location)
    int addOp(OpF op)
    {
        if (_startedOp) {
            assert(false && "addOp called within another addOp");
        }
        _startedOp = true;
        int pc = static_cast<int>(ops.size());
        ops.push_back(std::make_pair(op, static_cast<int>(opData.size())));
        return pc;
    }

    void endOp(bool execute = true)
    {
        _startedOp = false;
        if (execute) {
            double* fp = &state.d[0];
            char** str = &state.s[0];
            int pc = static_cast<int>(ops.size()) - 1;
            const std::pair<OpF, int>& op = ops[pc];
            int* opCurr = &opData[0] + op.second;
            pc += op.first(opCurr, fp, str);
        }
    }

    ///! Adds an operand. Note this should be done after doing the addOp!
    int addOperand(int param)
    {
        assert(_startedOp);
        int ret = static_cast<int>(opData.size());
        opData.push_back(param);
        return ret;
    }

    ///! Allocate a floating point set of data of dimension n
    int allocFP(int n)
    {
        int ret = static_cast<int>(state.d.size());
        for (int k = 0; k < n; k++)
            state.d.push_back(0);
        return ret;
    }

    /// Allocate a pointer location (can be anything, but typically space for char*)
    int allocPtr()
    {
        int ret = static_cast<int>(state.s.size());
        state.s.push_back(nullptr);
        return ret;
    }

    virtual void setDebugging(bool debugging) override
    {
        _debugging = debugging;
    }

    virtual bool prep(ExprNode* parseTree, ExprType desiredReturnType) override;

    virtual inline void evalStr(char* dst, VarBlock* varBlock) const override
    {
        std::lock_guard<std::mutex> guard(_m);
        eval(varBlock);
        memcpy((char*)dst, (const char*)&state.s[_returnSlot], sizeof(char*));
    }

    virtual inline void evalFP(double* dst, VarBlock* varBlock) const override
    {
        std::lock_guard<std::mutex> guard(_m);
        eval(varBlock);
        memcpy((char*)dst, (const char*)&state.d[_returnSlot], sizeof(double) * _desiredReturnType.dim());
    }

    virtual inline void evalMultiple(VarBlock* varBlock,
                                     double* outputBuffer,
                                     size_t rangeStart,
                                     size_t rangeEnd) const override;

    virtual bool isValid() const override
    {
        return true;
    }

    virtual inline void dump() const override
    {
        print();
    }

    /// Debug by printing program
    void print(int pc = -1) const;

    void setPCStart(int pcStart)
    {
        _pcStart = pcStart;
    }

  private:
    void eval(VarBlock* varBlock, bool debug = false) const;
};

//! Return the function f encapsulated in class T for the dynamic i converted to a static d.
template <template <int d> class T, class T_FUNCTYPE = Interpreter::OpF>
T_FUNCTYPE getTemplatizedOp(int i) {
    switch (i) {
        case 1:
            return T<1>::f;
        case 2:
            return T<2>::f;
        case 3:
            return T<3>::f;
        case 4:
            return T<4>::f;
        case 5:
            return T<5>::f;
        case 6:
            return T<6>::f;
        case 7:
            return T<7>::f;
        case 8:
            return T<8>::f;
        case 9:
            return T<9>::f;
        case 10:
            return T<10>::f;
        case 11:
            return T<11>::f;
        case 12:
            return T<12>::f;
        case 13:
            return T<13>::f;
        case 14:
            return T<14>::f;
        case 15:
            return T<15>::f;
        case 16:
            return T<16>::f;
        default:
            assert(false && "Invalid dynamic parameter (not supported template)");
            break;
    }
    return 0;
}
}

#endif
