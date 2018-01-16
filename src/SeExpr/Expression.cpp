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
#ifndef MAKEDEPEND
#include <iostream>
#include <math.h>
#include <stack>
#include <algorithm>
#include <sstream>
#endif

#include "ExprConfig.h"
#include "ExprNode.h"
#include "ExprParseAction.h"
#include "ExprFunc.h"
#include "Expression.h"
#include "ExprType.h"
#include "ExprEnv.h"
#include "Platform.h"

#include "LLVMEvaluator.h"

#include <cstdio>
#include <typeinfo>
#include <ExprWalker.h>

namespace SeExpr2 {

// Get debugging flag from environment
bool Expression::debugging = getenv("SE_EXPR_DEBUG") != 0;
// Choose the defeault strategy based on what we've compiled with (SEEXPR_ENABLE_LLVM)
// And the environment variables SE_EXPR_DEBUG
static Expression::EvaluationStrategy chooseDefaultEvaluationStrategy() {
    if (Expression::debugging) {
        std::cerr << "SeExpr2 Debug Mode Enabled " << __VERSION__
                  << std::endl;
    }
#ifdef SEEXPR_ENABLE_LLVM
    if (char* env = getenv("SE_EXPR_EVAL")) {
        if (Expression::debugging) std::cerr << "Overriding SeExpr Evaluation Default to be " << env << std::endl;
        return !strcmp(env, "LLVM") ? Expression::UseLLVM : !strcmp(env, "INTERPRETER") ? Expression::UseInterpreter
                                                                                        : Expression::UseInterpreter;
    } else
        return Expression::UseLLVM;
#else
    return Expression::UseInterpreter;
#endif
}
Expression::EvaluationStrategy Expression::defaultEvaluationStrategy = chooseDefaultEvaluationStrategy();

class TypePrintExaminer : public SeExpr2::Examiner<true> {
  public:
    virtual bool examine(const SeExpr2::ExprNode* examinee);
    virtual void reset() {};
};

bool TypePrintExaminer::examine(const ExprNode* examinee) {
    const ExprNode* curr = examinee;
    int depth = 0;
    char buf[1024];
    while (curr != 0) {
        depth++;
        curr = curr->parent();
    }
    sprintf(buf, "%*s", depth * 2, " ");
    std::cout << buf << "'" << examinee->toString() << "' " << typeid(*examinee).name()
              << " type=" << examinee->type().toString() << std::endl;

    return true;
};

class NullEvaluator : public Evaluator {
  public:
    virtual ~NullEvaluator() {}

    virtual void setDebugging(bool) override { /* do nothing */ }
    virtual void dump() const override {}

    virtual bool prep(ExprNode* parseTree, ExprType desiredReturnType) { return false; }
    virtual bool isValid() const override { return false; }

    virtual const double* evalFP(VarBlock* varBlock) { return {}; }
    virtual const char* evalStr(VarBlock* varBlock) { return ""; }
    virtual void evalMultiple(VarBlock* varBlock, int outputVarBlockOffset, size_t rangeStart, size_t rangeEnd) {}
};

Expression::Expression(Expression::EvaluationStrategy evaluationStrategyHint)
    : _wantVec(true)
    , _expression("")
    , _evaluationStrategyHint(evaluationStrategyHint)
    , _context(&Context::global())
    , _desiredReturnType(ExprType().FP(3).Varying())
    , _parseTree(0)
    , _evaluator(nullptr) {
    ExprFunc::init();
}

Expression::Expression(const std::string& e,
                       const ExprType& type,
                       EvaluationStrategy evaluationStrategyHint,
                       const Context& context)
    : _wantVec(true)
    , _expression(e)
    , _evaluationStrategyHint(evaluationStrategyHint)
    , _context(&context)
    , _desiredReturnType(type)
    , _parseTree(0)
    , _evaluator(nullptr) {
    ExprFunc::init();
}

Expression::~Expression() {
    reset();
}

void Expression::reset() {
    delete _evaluator;
    delete _parseTree;
    _parseError = "";
    _vars.clear();
    _funcs.clear();
    //_localVars.clear();
    _errors.clear();
    _envBuilder.reset();
    _threadUnsafeFunctionCalls.clear();
    _comments.clear();
}

void Expression::setContext(const Context& context) {
    reset();
    _context = &context;
}

void Expression::setDesiredReturnType(const ExprType& type) {
    if (_desiredReturnType != type) {
        reset();
        _desiredReturnType = type;
    }
}

void Expression::setVarBlockCreator(const VarBlockCreator* creator) {
    if (_varBlockCreator != creator) {
        reset();
        _varBlockCreator = creator;
    }
}

void Expression::setExpr(const std::string& e) {
    if (_expression != e) {
        if (!_expression.empty()) reset();
        _expression = e;
    }
}

bool Expression::isConstant() const {
    parse();
    return returnType().isLifetimeConstant();
}

bool Expression::usesVar(const std::string& name) const {
    parse();
    return _vars.find(name) != _vars.end();
}

bool Expression::usesFunc(const std::string& name) const {
    parse();
    return _funcs.find(name) != _funcs.end();
}

void Expression::parse() const {
    if (_parseTree) return;
    int tempStartPos, tempEndPos;
    ExprParseAction(_parseTree, _parseError, tempStartPos, tempEndPos, _comments, this, _expression.c_str(), _wantVec);
    if (!_parseTree) {
        addError(_parseError, tempStartPos, tempEndPos);
        delete _parseTree;
    } else {
        // TODO: need promote
        _returnType = _parseTree->type();
    }
}

void Expression::prep() const {
    if (_evaluator) return;
#ifdef SEEXPR_PERFORMANCE
    PrintTiming timer("[ PREP     ] v2 prep time: ");
#endif
    parse();

    bool error = false;
    Evaluator* evaluator = nullptr;

    if (!_parseTree) {
        // parse error
        error = true;
    } else if (!_parseTree->prep(_desiredReturnType.isFP(1), _envBuilder).isValid()) {
        // prep error
        error = true;
    } else if (!ExprType::valuesCompatible(_parseTree->type(), _desiredReturnType)) {
        // incompatible type error
        error = true;
        _parseTree->addError("Expression generated type " + _parseTree->type().toString() +
                             " incompatible with desired type " + _desiredReturnType.toString());
    } else {
        // optimize for constant values - if we have a module of just one constant float, avoid using LLVM.
        //   Querying typing information during prep()-time is a bit difficult. For the most part, our type
        //   information is uninitialized until after prep()-time. The only exception is for constant,
        //   single floating point values.
        //
        // TODO: separate Object Representation (ExprNode) from ParseTree (which should just be cheap tokens)
        bool isConstant_ = isConstant() || (_parseTree && _parseTree->numChildren() == 1 && _parseTree->child(0)->type().isLifetimeConstant());
        EvaluationStrategy strategy = isConstant_ ? EvaluationStrategy::UseInterpreter : _evaluationStrategyHint;

        evaluator = (strategy == UseInterpreter) ? (Evaluator*)new Interpreter() : (Evaluator*)new LLVMEvaluator();
        evaluator->setDebugging(debugging);
        error = !evaluator->prep(_parseTree, _desiredReturnType);
    }

    if (error) {
        if (evaluator) delete evaluator;
        _returnType = ExprType().Error();

        // build line lookup table
        std::vector<int> lines;
        const char* start = _expression.c_str();
        const char* p = _expression.c_str();
        while (*p != 0) {
            if (*p == '\n') lines.push_back(p - start);
            p++;
        }
        lines.push_back(p - start);

        std::stringstream sstream;
        for (unsigned int i = 0; i < _errors.size(); i++) {
            int* bound = std::lower_bound(&*lines.begin(), &*lines.end(), _errors[i].startPos);
            int line = bound - &*lines.begin() + 1;
            int lineStart = line == 1 ? 0 : lines[line - 1];
            int col = _errors[i].startPos - lineStart;
            sstream << "  Line " << line << " Col " << col << " " << _errors[i].error << std::endl;
        }
        _parseError = std::string(sstream.str());
    }

    if (debugging) {
        std::cerr << "parse error \n" << parseError() << std::endl;
    }

    if (!evaluator) evaluator = new NullEvaluator();
    _evaluator = evaluator;
    assert(_evaluator);
}

bool Expression::isVec() const {
    prep();
    return isValid() ? _parseTree->isVec() : _wantVec;
}

const ExprType& Expression::returnType() const {
    prep();
    return _returnType;
}
}  // end namespace SeExpr2/
