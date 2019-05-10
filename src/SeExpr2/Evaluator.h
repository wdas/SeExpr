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

#pragma once

#include <cstddef>

#include "ExprType.h"

namespace SeExpr2 {

class ExprNode;
class VarBlock;

// Evaluator Interface
//   Used internally by Expression to delegate expression evaluation
//   to LLVM or an Interpreter
class Evaluator {
  public:
    virtual ~Evaluator()
    {
    }

    // dump debugging information to stderr
    virtual void setDebugging(bool debugging) = 0;

    // dump debugging information to stderr
    virtual void dump() const = 0;

    // bind variables, bind functions, and builds an necessary auxiliary structures needed for evaluation
    virtual bool prep(ExprNode* parseTree, ExprType desiredReturnType) = 0;

    // returns true if we prep()'ed without error, otherwise false
    virtual bool isValid() const = 0;

    // Evaluates and returns float (check returnType()!)
    virtual void evalFP(double* dst, VarBlock* varBlock) const = 0;

    // Evaluates and returns string (check returnType()!)
    virtual void evalStr(char* dst, VarBlock* varBlock) const = 0;

    // Evaluate multiple blocks
    virtual void evalMultiple(VarBlock* varBlock, double* outputBuffer, size_t rangeStart, size_t rangeEnd) const = 0;
};
}
