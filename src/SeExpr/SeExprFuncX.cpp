/*
 SEEXPR SOFTWARE
 Copyright 2011 Disney Enterprises, Inc. All rights reserved
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are
 met:
 
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in
 the documentation and/or other materials provided with the
 distribution.
 
 * The names "Disney", "Walt Disney Pictures", "Walt Disney Animation
 Studios" or the names of its contributors may NOT be used to
 endorse or promote products derived from this software without
 specific prior written permission from Walt Disney Pictures.
 
 Disclaimer: THIS SOFTWARE IS PROVIDED BY WALT DISNEY PICTURES AND
 CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR PURPOSE, NONINFRINGEMENT AND TITLE ARE DISCLAIMED.
 IN NO EVENT SHALL WALT DISNEY PICTURES, THE COPYRIGHT HOLDER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND BASED ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*/
#include "SeExprFuncX.h"
#include "SeInterpreter.h"
#include "SeExprNode.h"
#include <cstdio>

int SeExprFuncSimple::EvalOp(int* opData,double* fp,char** c)
{
    SeExprFuncSimple* simple=reinterpret_cast<SeExprFuncSimple*>(c[opData[0]]);
    ArgHandle args(opData,fp,c);
    simple->eval(args);
    return 1;
}

int SeExprFuncSimple::buildInterpreter(const SeExprFuncNode* node,SeInterpreter* interpreter) const
{
    std::vector<int> operands;
    for(int c=0;c<node->numChildren();c++){
        const SeExprNode* child=node->child(c);
        int operand=node->child(c)->buildInterpreter(interpreter);
        std::cerr<<"we are "<<node->promote(c)<<" "<<c<<std::endl;
        if(node->promote(c) != 0) {
            interpreter->addOp(getTemplatizedOp<Promote>(node->promote(c)));
            int promotedOperand=interpreter->allocFP(node->promote(c));
            interpreter->addOperand(operand);
            interpreter->addOperand(promotedOperand);
            operand=promotedOperand;
        }
        operands.push_back(operand);
    }
    int outoperand=-1;
    if(node->type().isFP()) outoperand=interpreter->allocFP(node->type().dim());
    else if(node->type().isString()) outoperand=interpreter->allocPtr();
    else assert(false);

    interpreter->addOp(EvalOp);
    int ptrLoc=interpreter->allocPtr();
    interpreter->s[ptrLoc]=(char*)this;
    interpreter->addOperand(ptrLoc);
    interpreter->addOperand(outoperand);
    for(size_t c=0;c<operands.size();c++){
        interpreter->addOperand(operands[c]);
    }
    interpreter->print();

    return outoperand;
}










