/*
* Copyright Disney Enterprises, Inc.  All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License
* and the following modification to it: Section 6 Trademarks.
* deleted and replaced with:
*
* 6. Trademarks. This License does not grant permission to use the
* trade names, trademarks, service marks, or product names of the
* Licensor and its affiliates, except as required for reproducing
* the content of the NOTICE file.
*
* You may obtain a copy of the License at
* http://www.apache.org/licenses/LICENSE-2.0
*/
#ifndef _ExprSpecType_h_
#define _ExprSpecType_h_

#include <SeExpr2/Vec.h>
#include <vector>
#include <sstream>
#include <SeExpr2/Curve.h>
#include <cstdio>

/// Mini parse tree node... Only represents literals, and lists of literals
struct ExprSpecNode {
    int startPos, endPos;

    ExprSpecNode(int startPos, int endPos) : startPos(startPos), endPos(endPos) {}

    virtual ~ExprSpecNode() {}
};

struct ExprSpecScalarNode : public ExprSpecNode {
    double v;

    ExprSpecScalarNode(int startPos, int endPos, double scalar) : ExprSpecNode(startPos, endPos), v(scalar) {}
};

struct ExprSpecVectorNode : public ExprSpecNode {
    SeExpr2::Vec3d v;
    ExprSpecVectorNode(int startPos, int endPos, ExprSpecNode* x, ExprSpecNode* y, ExprSpecNode* z)
        : ExprSpecNode(startPos, endPos) {
        v = SeExpr2::Vec3d(static_cast<ExprSpecScalarNode*>(x)->v,
                           static_cast<ExprSpecScalarNode*>(y)->v,
                           static_cast<ExprSpecScalarNode*>(z)->v);
    }
};

struct ExprSpecListNode : public ExprSpecNode {
    std::vector<ExprSpecNode*> nodes;
    ExprSpecListNode(int startPos, int endPos) : ExprSpecNode(startPos, endPos) {}

    void add(ExprSpecNode* node) {
        startPos = std::min(node->startPos, startPos);
        endPos = std::max(node->endPos, endPos);
        nodes.push_back(node);
    }
};

struct ExprSpecStringNode : public ExprSpecNode {
    std::string v;
    ExprSpecStringNode(int startPos, int endPos, const char* s) : ExprSpecNode(startPos, endPos), v(s) {}
};

struct ExprSpecCurveNode : public ExprSpecNode {
    ExprSpecNode* args;
    ExprSpecCurveNode(ExprSpecNode* args) : ExprSpecNode(args->startPos, args->endPos), args(args) {}
};

struct ExprSpecCCurveNode : public ExprSpecNode {
    ExprSpecNode* args;
    ExprSpecCCurveNode(ExprSpecNode* args) : ExprSpecNode(args->startPos, args->endPos), args(args) {}
};

struct ExprSpecAnimCurveNode : public ExprSpecNode {
    ExprSpecNode* args;
    ExprSpecAnimCurveNode(ExprSpecNode* args) : ExprSpecNode(args->startPos, args->endPos), args(args) {}
};

struct ExprSpecColorSwatchNode : public ExprSpecNode {
    ExprSpecNode* args;
    ExprSpecColorSwatchNode(ExprSpecNode* args) : ExprSpecNode(args->startPos, args->endPos), args(args) {}
};

struct ExprSpecDeepWaterNode : public ExprSpecNode {
    ExprSpecNode* args;
    ExprSpecDeepWaterNode(ExprSpecNode* args) : ExprSpecNode(args->startPos, args->endPos), args(args) {}
};

#endif
