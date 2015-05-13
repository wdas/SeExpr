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
#ifndef _SeExprSpecType_h_
#define _SeExprSpecType_h_

#include <SeVec3d.h>
#include <vector>
#include <sstream>
#include <SeCurve.h>
#include <cstdio>

/// Mini parse tree node... Only represents literals, and lists of literals
struct SeExprSpecNode
{
    int startPos,endPos;


    SeExprSpecNode(int startPos,int endPos)
        :startPos(startPos),endPos(endPos)
    {}
    
    virtual ~SeExprSpecNode()
    {}
};

struct SeExprSpecScalarNode:public SeExprSpecNode
{
    double v;

    SeExprSpecScalarNode(int startPos,int endPos,double scalar)
        :SeExprSpecNode(startPos,endPos),v(scalar)
    {}
};

struct SeExprSpecVectorNode:public SeExprSpecNode
{
    SeVec3d v;
    SeExprSpecVectorNode(int startPos,int endPos,SeExprSpecNode* x,SeExprSpecNode* y,SeExprSpecNode* z)
        :SeExprSpecNode(startPos,endPos)
    {
        v=SeVec3d(static_cast<SeExprSpecScalarNode*>(x)->v,static_cast<SeExprSpecScalarNode*>(y)->v,static_cast<SeExprSpecScalarNode*>(z)->v);
    }

};

struct SeExprSpecListNode:public SeExprSpecNode
{
    std::vector<SeExprSpecNode*> nodes;
    SeExprSpecListNode(int startPos,int endPos)
        :SeExprSpecNode(startPos,endPos)
    {}

    void add(SeExprSpecNode* node){
        startPos=std::min(node->startPos,startPos);
        endPos=std::max(node->endPos,endPos);
        nodes.push_back(node);
    }
};

struct SeExprSpecStringNode:public SeExprSpecNode
{
    std::string v;
    SeExprSpecStringNode(int startPos,int endPos,const char* s)
        :SeExprSpecNode(startPos,endPos),v(s)
    {}
};

struct SeExprSpecCurveNode:public SeExprSpecNode
{
    SeExprSpecNode* args;
    SeExprSpecCurveNode(SeExprSpecNode* args)
        :SeExprSpecNode(args->startPos,args->endPos),args(args)
    {}
};

struct SeExprSpecCCurveNode:public SeExprSpecNode
{
    SeExprSpecNode* args;
    SeExprSpecCCurveNode(SeExprSpecNode* args)
        :SeExprSpecNode(args->startPos,args->endPos),args(args)
    {}
};

struct SeExprSpecAnimCurveNode:public SeExprSpecNode
{
    SeExprSpecNode* args;
    SeExprSpecAnimCurveNode(SeExprSpecNode* args)
        :SeExprSpecNode(args->startPos,args->endPos),args(args)
    {}
};

struct SeExprSpecColorSwatchNode:public SeExprSpecNode
{
    SeExprSpecNode* args;
    SeExprSpecColorSwatchNode(SeExprSpecNode* args)
        :SeExprSpecNode(args->startPos,args->endPos),args(args)
    {}
};

#endif










