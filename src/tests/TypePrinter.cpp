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
#include <SeExpression.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "SeExprWalker.h"
#include "SeExprNode.h"
#include "SeExprFunc.h"
#include "TypeBuilder.h"
#include "TypePrinter.h"

//! Simple expression class to print out all intermediate types
class TypePrinterExpr : public TypeBuilderExpr
{
public:
    TypePrinterExpr()
         : TypeBuilderExpr(),
          _examiner(),
          _walker(&_examiner)
    {};

    TypePrinterExpr(const std::string &e)
        :  TypeBuilderExpr(e),
          _examiner(),
          _walker(&_examiner)
    {};

    inline void walk () { if(_parseTree) _walker.walk(_parseTree); };

private:
    TypePrintExaminer _examiner;
    SeExpr::ConstWalker  _walker;

protected:
    SeExprVarRef* resolveVar(const std::string& name) const {
        return TypeBuilderExpr::resolveVar(name);
    };

    SeExprFunc* resolveFunc(const std::string& name) const
    {
        return TypeBuilderExpr::resolveFunc(name);
    }
};

void get_or_quit(std::string & str) {
    getline(std::cin, str);

    if(std::cin.eof())
        exit(0);
};


int main(int argc,char *argv[])
{
    TypePrinterExpr expr;
    std::string     str;
    bool            givenTest = false;

    if(argc == 2) {
        givenTest = true;
        str = argv[1];
    };

    if(givenTest) {
        expr.setExpr(str);
        if(expr.isValid()) {
            std::cout << "Expression types:" << std::endl;
        } else
	    std::cerr << "Expression failed: " << expr.parseError() << std::endl;
    } else {
        std::cout << "SeExpr Basic Pattern Matcher (Iteractive Mode):";

        while(true) {
            std::cout << std::endl << "> ";

            get_or_quit(str);
            expr.setExpr(str);
            bool valid=expr.isValid();
            std::cout << "Expression types:" << std::endl;
            expr.walk();
            if(!valid)
                std::cerr << "Expression failed: " << expr.parseError() << std::endl;
            else if(expr.returnType().isFP() && expr.returnType().dim()<=16){

                double foo[16];
                for(int c=0;c<16;c++) foo[c]=0;
                SeExprEvalResult res(16,foo);
                
                expr.evalNew(res);
                for(int i=0;i<expr.returnType().dim();i++){
                    std::cerr<<res.fp[i]<<" ";
                }
                std::cerr<<std::endl;
            }else{
                std::cerr<<"can't eval things that are not FP[<=16]"<<std::endl;
            }
        }
    }

    return 0;
}
