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
#include <string.h>
#include <string>
#include <vector>
#endif

#include "ExprNode.h"
#include "ExprPatterns.h"
#include "ExprWalker.h"

namespace SeExpr2 {

template <bool constnode>
void Walker<constnode>::walk(T_NODE* examinee)
{
    _examiner->reset();
    internalWalk(examinee);
};

template <bool constnode>
void Walker<constnode>::internalWalk(T_NODE* examinee)
{
    /// If examine returns false, do not recurse
    if (_examiner->examine(examinee))
        walkChildren(examinee);
    _examiner->post(examinee);
};

template <bool constnode>
void Walker<constnode>::walkChildren(T_NODE* parent)
{
    for (int i = 0; i < parent->numChildren(); i++)
        internalWalk(parent->child(i));
};

template class Walker<false>;
template class Walker<true>;
}
