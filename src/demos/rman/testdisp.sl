/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
*/
plugin "libSeExprOp.so";

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
