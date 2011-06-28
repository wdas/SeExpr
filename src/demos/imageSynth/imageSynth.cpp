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
/**
   @file imageSynth.cpp
*/
#include <map>
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
    struct Var:public SeExprVarRef
    {
        Var(const double val)
	    : SeExprVarRef(SeExprType::FP1Type()), val(val)
	{}

	Var()
	    : SeExprVarRef(SeExprType::FP1Type()), val(0.0)
	{}

        double val; // independent variable
        void eval(const SeExprVarNode* node,SeVec3d& result)
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
    const unsigned char *ptrs[height];
    for(int i=0;i<height;i++){
        ptrs[i]=&image[width*i*4];
    }
    png_set_rows(png_ptr,info_ptr,(png_byte**)ptrs);
    png_write_png(png_ptr,info_ptr,PNG_TRANSFORM_IDENTITY,0);
    
    fclose(fp);
}
