/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
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




    

#endif










