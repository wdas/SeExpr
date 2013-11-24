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
/**
   @file imageSynth.cpp
*/
#include <map>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <SeExpression.h>
#include <png.h>
#include <fstream>

//! Simple image synthesizer expression class to support our function grapher
class ImageSynthExpr:public SeExpression
{
public:
    //! Constructor that takes the expression to parse
    ImageSynthExpr(const std::string& expr)
        :SeExpression(expr)
    {}

    //! Simple variable that just returns its internal value
    struct Var:public SeExprScalarVarRef
    {
        Var(const double val):val(val){}
        Var(){}
        double val; // independent variable
        void eval(const SeExprVarNode* /*node*/,SeVec3d& result)
        {result[0]=val;}
    };
    //! variable map
    mutable std::map<std::string,Var> vars;

    //! resolve function that only supports one external variable 'x'
    SeExprVarRef* resolveVar(const std::string& name) const
    {
        std::map<std::string,Var>::iterator i=vars.find(name);
        if(i != vars.end()) return &i->second;
        return 0;
    }
};


double clamp(double x){return std::max(0.,std::min(255.,x));}

int main(int argc,char *argv[]){
    if(argc != 5){
        std::cerr<<"Usage: "<<argv[0]<<" <image file> <width> <height> <exprFile>"<<std::endl;
        return 1;
    }

    // parse arguments
    const char* imageFile=argv[1];
    const char* exprFile=argv[4];
    int width=atoi(argv[2]),height=atoi(argv[3]);
    if(width<0 || height<0){
        std::cerr<<"invalid width/height"<<std::endl;
        return 1;
    }

    std::ifstream istream(exprFile);
    if(!istream){
        std::cerr<<"Cannot read file "<<exprFile<<std::endl;
        return 1;
    }
    std::string exprStr((std::istreambuf_iterator<char>(istream)),std::istreambuf_iterator<char>());
    ImageSynthExpr expr(exprStr);

    // make variables
    expr.vars["u"]=ImageSynthExpr::Var(0.);
    expr.vars["v"]=ImageSynthExpr::Var(0.);
    expr.vars["w"]=ImageSynthExpr::Var(width);
    expr.vars["h"]=ImageSynthExpr::Var(height);
    
    // check if expression is valid
    bool valid=expr.isValid();
    if(!valid){
        std::cerr<<"Invalid expression "<<std::endl;
        std::cerr<<expr.parseError()<<std::endl;
    }

    // evaluate expression
    std::cerr<<"Evaluating expresion...from "<<exprFile<<std::endl;
    unsigned char* image=new unsigned char[width*height*4];
    double one_over_width=1./width,one_over_height=1./height;
    double& u=expr.vars["u"].val;
    double& v=expr.vars["v"].val;
    unsigned char* pixel=image;
    for(int row=0;row<height;row++){
        for(int col=0;col<width;col++){
            u=one_over_width*(col+.5);
            v=one_over_height*(row+.5);
            SeVec3d result=expr.evaluate();
            pixel[0]=clamp(result[0]*256.);
            pixel[1]=clamp(result[1]*256.);
            pixel[2]=clamp(result[2]*256.);
            pixel[3]=255;
            pixel+=4;
        }
    }

    // write image as png
    std::cerr<<"Writing image..."<<imageFile<<std::endl;
    FILE *fp=fopen(imageFile,"wb");
    if(!fp){
        perror("fopen");
        return 1;
    }
    png_structp png_ptr;
    png_infop info_ptr;
    png_ptr=png_create_write_struct(PNG_LIBPNG_VER_STRING,0,0,0);
    info_ptr=png_create_info_struct(png_ptr);
    png_init_io(png_ptr,fp);
    int color_type=PNG_COLOR_TYPE_RGBA;
    png_set_IHDR(png_ptr,info_ptr,width,height,8,color_type,PNG_INTERLACE_NONE,PNG_COMPRESSION_TYPE_DEFAULT,PNG_FILTER_TYPE_DEFAULT);
    // Use vector instead of const array to fix Windows compile error -jb
    std::vector<unsigned char*> ptrs; 
    for(int i=0;i<height;i++){
        ptrs.push_back(&image[width*i*4]);
    }
    png_set_rows(png_ptr,info_ptr,(png_byte**)ptrs.data());
    png_write_png(png_ptr,info_ptr,PNG_TRANSFORM_IDENTITY,0);
    
    delete [] image;
    ptrs.clear();

    fclose(fp);
}
