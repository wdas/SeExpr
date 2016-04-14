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
#ifndef _TypePrinter_h_
#define _TypePrinter_h_

#include <ExprNode.h>
#include <ExprWalker.h>
#include <Expression.h>
#include <typeinfo>

using namespace SeExpr2;

/**
   @file TypePrinter.cpp
*/

class TypePrintExaminer : public SeExpr2::Examiner<true> {
  public:
    virtual bool examine(const ExprNode* examinee);
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

#endif
