/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
*/

plugin "libSeExprOp";

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
