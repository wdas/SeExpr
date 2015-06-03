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

plugin "libSeExprOp2";

//! Shader that allows an expression for color and opacitry
surface testse(
    uniform string CExpr="[1,1,1]";
    uniform string OExpr="1";
)
{
    // Compute some useful quantities
    normal Nn=normalize(N);
    normal Nobj=transform("object",Nn);
    point Pw=transform("world",P);
    point Pobj=transform("object",P);

    // This will store which variables are used, but we don't care in this example
    uniform float used[];

    // We bind Pw, P and N, Cs, Os
    uniform string varNames="(error) Pw P N Cs Os";
    varying color varValues[]={color(Pw),color(Pobj),color(Nobj),Cs,Os};

    // Evaluate color and put into Ci
    uniform float CExprHandle=SeExprBind(CExpr,varNames,used);
    SeExprEval(CExprHandle,varValues,Ci);

    // Evaluate color and put into Oi
    uniform float OExprHandle=SeExprBind(OExpr,varNames,used);
    SeExprEval(OExprHandle,varValues,Oi);

    // Final diffuse ligthign
    normal Nf=faceforward(Nn,I);
    Ci=Ci*(diffuse(Nf)+ambient())*Oi;
}
