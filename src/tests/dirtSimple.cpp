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

#include <SeExpr2/Expression.h>
#include <SeExpr2/Vec.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>

using namespace SeExpr2;
int main()
{
    Expression e("1+2");

    if (!e.isValid()) {
        throw std::runtime_error(e.parseError());
    }
    e.evaluator()->dump();
    const double* val = e.evalFP();
    std::cout << "val is " << val[0] << std::endl;

    return 0;
}
