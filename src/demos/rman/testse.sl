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
