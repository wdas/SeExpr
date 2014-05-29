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
#include "SeExprMultiExpr.h"
using namespace SeExpr2;

//int usageWithMultiple(){
//    Expresisons exprs(5);
//    VariableHandle xHandle = exprs.addExternalVariable(x,3);
//    ExprHandle exprHandle = expr.addExpression("fooExpr", "x*3");
//    double xMultiple[] = {0.0, 1.0, 2.0, 3.0, 4.0};
//    setVariable(xHandle, xMultiple);
//    double* vals = exprs.evaluateFP(exprHandle);
//    for(int i=0;i<5;i++){
//        std::cerr<<"i="<<i<<" "<<vals[i]<<std::endl;
//    }
//
//}

//int main(){
//
//    Expressions ee(1);
//    // dev program time
//    VariableHandle xHandle = ee.addExternalVariable("x", 1, &x);
//    VariableHandle vHandle = ee.addExternalVariable("v", 3, &v);
//
//    // user time.
//    std::string input1;
//    std::string input2;
//    ExprHandle parabola = ee.addExpression("parabola", input1);
//    ExprHandle cubic = ee.addExpression("cubic", input2);
//    //   ExprHandle parabola = ee.addExpression("parabola", "x*x");
//    //   ExprHandle cubic = ee.addExpression("cubic", "parabola*v");
//    ee.isValid();
//
//    //ee.prep();
//
//    double dx = (xmax-xmin) / (npoints-1);
//    for(int i=0;i<npoints;i++){
//        if(ee.isVariableUsed(xHandle)) {
//            ee.setVariableWithoutMarkDirty(xHandle, i*dx+xmin);
//        }
//        if(ee.isVariableUsed(vHandle)){
//            ee.setVariableWithoutMarkDirty(vHandle, {0.0, 1.0, 2.0});
//        }
//        double* xCubed = ee.evaluate(cubic);
//    }
//}

//int main(){
//    Expressions ee();
//
//    // dev program time
//    VariableHandle xHandle = ee.addExternalVariable("x", ExprType.FP(1).Varying());
//    VariableHandle vHandle = ee.addExternalVariable("v", ExprType.FP(3).Varying());
//
//    // user time.
//    std::string input1;
//    std::string input2;
//    ExprHandle parabola = ee.addExpression("parabola", ExprType.FP(1).Varying(), input1);
//    ExprHandle cubic = ee.addExpression("cubic", ExprType.FP(1).Varying(), input2);
//    //   ExprHandle parabola = ee.addExpression("parabola", "x*x");
//    //   ExprHandle cubic = ee.addExpression("cubic", "parabola*v");
//    ee.isValid();
//
//    //ee.prep();
//
//    double dx = (xmax-xmin) / (npoints-1);
//    for(int i=0;i<npoints;i++){
//        if(ee.isVariableUsed(xHandle)) {
//            ee.setVariableWithoutMarkDirty(xHandle, i*dx+xmin);
//        }
//        if(ee.isVariableUsed(vHandle)){
//            ee.setVariableWithoutMarkDirty(vHandle, {0.0, 1.0, 2.0});
//        }
//        double* xCubed = ee.evaluate(cubic);
//    }
//}

int main(){
   Expressions ee;

   VariableHandle xHandle = ee.addExternalVariable("x", ExprType().FP(1).Varying());
   VariableHandle vHandle = ee.addExternalVariable("v", ExprType().FP(3).Varying());

   ExprHandle parabola = ee.addExpression("parabola", ExprType().FP(3).Varying(), "x*x");
   ExprHandle cubic = ee.addExpression("cubic", ExprType().FP(3).Varying(), "parabola*v");
   ExprHandle e1 = ee.addExpression("e1", ExprType().FP(3).Varying(), "cubic*parabola+x+1");

   if(!ee.isValid()) {
       std::cerr << "Not valid\n";
       return 1;
   }

   //VariableSetHandle xSHandle = ee.getLoopVarSetHandle(xHandle);
   VariableSetHandle vSHandle = ee.getLoopVarSetHandle(vHandle);
   ExprEvalHandle Scubic = ee.getExprEvalHandle(cubic);
   std::cout << "size of Scubic is " << Scubic.second.size() << std::endl;
   //ExprEvalHandle Se1 = ee.getExprEvalHandle(e1);
   //std::cout << "size of Se1 is " << Se1.second.size() << std::endl;

   ee.setVariable(xHandle, 8);

   double xmax = 100;
   double xmin = 1;
   double npoints = 1000;
   double dx = (xmax-xmin) / (npoints-1);
   for(int i=0;i<npoints;i++){
       //ee.setVariable(xSHandle, i*dx+xmin);
       double myints[] =  {2.0, 3.0, 4.0};
       ee.setLoopVariable(vSHandle, myints, 3);
       std::vector<double> xCubed = ee.evalFP(Scubic);
       std::cout << xCubed[0] << ","
                 << xCubed[1] << ","
                 << xCubed[2] << std::endl;

//       std::vector<double> xe1 = ee.evalFP(Se1);
//       std::cout << xe1[0] << ","
//                 << xe1[1] << ","
//                 << xe1[2] << std::endl;
   }

   return 0;
}
