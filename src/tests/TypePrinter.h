
#ifndef _TypePrinter_h_
#define _TypePrinter_h_

#include <SeExprNode.h>
#include <SeExprWalker.h>
#include <SeExpression.h>
#include <typeinfo>


/**
   @file TypePrinter.cpp
*/
class TypePrintExaminer : public SeExpr::Examiner<true> {
public:
    virtual bool examine(const SeExprNode* examinee);
    virtual void reset  ()                           {};
};


bool
TypePrintExaminer::examine(const SeExprNode* examinee)
{
    const SeExprNode* curr=examinee;
    int depth=0;
    char buf[1024];
    while(curr != 0) {depth++;curr=curr->parent();}
    sprintf(buf,"%*s",depth*2," ");
    std::cout <<buf<<"'"<<examinee->toString()<<"' "<<typeid(*examinee).name()
              <<" type=" << examinee->type().toString() << std::endl;

    return true;
};


#endif
