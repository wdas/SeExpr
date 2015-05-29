
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
    virtual void reset  ()                           {};
};


bool
TypePrintExaminer::examine(const ExprNode* examinee)
{
    const ExprNode* curr=examinee;
    int depth=0;
    char buf[1024];
    while(curr != 0) {depth++;curr=curr->parent();}
    sprintf(buf,"%*s",depth*2," ");
    std::cout <<buf<<"'"<<examinee->toString()<<"' "<<typeid(*examinee).name()
              <<" type=" << examinee->type().toString() << std::endl;

    return true;
};


#endif
