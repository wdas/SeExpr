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

plugin "libSeExprOp2.so";

displacement testdisp(
    uniform string DispExpr="0";
)
{
    normal Nobj=transform("object",N);
    normal Nn=normalize(Nobj);
    color res=0;
    uniform float used[];
    uniform string varNames="(error) Pw P N";
    point Pw=transform("world",P);
    point Pobj=transform("object",P);
    varying color varValues[]={color(Pw),color(Pobj),color(Nobj)};

    uniform float DispExprHandle=SeExprBind(DispExpr,varNames,used);
    color disp=0;
    SeExprEval(DispExprHandle,varValues,disp);

    Pobj+=Nn*disp[0];
    // recompute position and normal
    point Ptmp=transform("object","current",Pobj);
    normal Ntmp=-calculatenormal(Ptmp);
    N=Ntmp;
    P=Ptmp;

}
