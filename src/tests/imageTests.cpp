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

#include <map>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <assert.h>
#include <dirent.h>
#include <pcrecpp.h>
#include <sys/time.h>

#include <gtest.h>
#include <ftw.h>
#include <png.h>

#ifdef SEEXPR_ENABLE_LLVM
#include <ExpressionLLVM.h>
#else
#include <Expression.h>
#endif

#include <ExprFunc.h>
#include <ExprFuncX.h>
#include <Platform.h> # performance timing

double clamp(double x){return std::max(0.,std::min(255.,x));}

// Get timestamp in microseconds, for recording elapsed time
typedef unsigned long long timestamp_t;
static timestamp_t get_timestamp ()
{
    struct timeval now;
    gettimeofday (&now, NULL);
    return  now.tv_usec + (timestamp_t)now.tv_sec * 1000000;
}

namespace SeExpr2 {

class RandFuncX:public ExprFuncSimple
{

    struct Data : public ExprFuncNode::Data {
        std::vector<std::pair<int,int> > ranges;
        std::string format;
    };

    virtual ExprType prep(ExprFuncNode* node, bool wantScalar,
                          ExprVarEnv & env) const {
        bool valid=true;
        for(int i=0;i<node->numChildren();i++)
            valid &= node->checkArg(i,ExprType().FP(1).Varying(),env);
        return valid ? ExprType().FP(1).Varying():ExprType().Error();
    }

    virtual ExprFuncNode::Data* evalConstant(ArgHandle args) const {
        return new Data;
    }

    virtual void eval(ArgHandle args) {
        if(args.nargs() >= 2) {
            args.outFp = (args.inFp<1>(0)[0] - args.inFp<1>(1)[0])/2.0;
        } else args.outFp = 0.5;
    }

    public:
    RandFuncX():ExprFuncSimple(true){}  // Thread Safe
    virtual ~RandFuncX() {}
} rand;

// map(string name, [float format-arg], [float u], [float v], [int channel])
class MapFunc : public ExprFuncSimple {
    struct Data : public ExprFuncNode::Data {
        std::vector<std::pair<int,int> > ranges;
        std::string format;
    };

public:
    MapFunc() : ExprFuncSimple(true) {} // Thread Safe
    virtual ~MapFunc() {}

    virtual ExprType prep(ExprFuncNode* node, bool wantScalar,
                          ExprVarEnv & env) const {
        bool valid=true;
        valid &= node->checkArg(0,ExprType().String().Constant(),env);
        for(int i=1;i<node->numChildren();i++)
            valid &= node->checkArg(i,ExprType().FP(1).Varying(),env);
        return valid ? ExprType().FP(3).Varying():ExprType().Error();
    }

    virtual ExprFuncNode::Data* evalConstant(ArgHandle args) const {
        return new Data;
    }

    virtual void eval(ArgHandle args) {
        double* out=&args.outFp;

        double val = 0.5;
        int num = args.nargs();
        if(num > 2)
            for(int k=2;k<num;k++)
                val += args.inFp<1>(k)[0];

        for(int k=0;k<3;k++) out[k]=val;
    }

} mapStubX;

// triplanar(string name, [vector scale], [float blend], [vector rotation],
//           [vector translation])
class TriplanarFuncX:public ExprFuncSimple
{
    struct Data : public ExprFuncNode::Data {
        std::vector<std::pair<int,int> > ranges;
        std::string format;
    };

    virtual ExprType prep(ExprFuncNode* node, bool wantScalar,
                          ExprVarEnv & env) const {
        bool valid=true;
        valid &= node->checkArg(0,ExprType().String().Constant(),env);
        for(int i=1;i<node->numChildren();i++)
            valid &= node->checkArg(i,ExprType().FP(1).Varying(),env);
        return valid ? ExprType().FP(3).Varying():ExprType().Error();
    }

    virtual ExprFuncNode::Data* evalConstant(ArgHandle args) const {
        return new Data;
    }

    virtual void eval(ArgHandle args) {
        double* out=&args.outFp;

        double val = 0.5;
        int num = args.nargs();
        if(num > 1)
            for(int k=1;k<num;k++)
                val += (args.inFp<3>(k)[0]+args.inFp<3>(k)[1]+args.inFp<3>(k)[2]);

        for(int k=0;k<3;k++) out[k]=val;
    }

    public:
    TriplanarFuncX():ExprFuncSimple(true){}  // Thread Safe
    virtual ~TriplanarFuncX() {}
} triplanar;

}


using namespace SeExpr2;

//! Simple image synthesizer expression class to test example expressions
class ImageSynthExpr:public Expression
{
public:
    //! Constructor that takes the expression to parse
    ImageSynthExpr(const std::string& expr)
#ifdef SEEXPR_ENABLE_LLVM
        :Expression(expr, ExprType().FP(3), UseLLVM)
#else
        :Expression(expr, ExprType().FP(3), UseInterpreter)
#endif
    {}

    //! Simple variable that just returns its internal value
    struct Var:public ExprVarRef
    {
        Var(const double val)
        : ExprVarRef(ExprType().FP(1).Varying()), val(val)
        {}

        Var()
        : ExprVarRef(ExprType().FP(1).Varying()), val(0.0)
        {}

        double val; // independent variable
        void eval(double* result)
        {result[0]=val;}

        void eval(const char** result)
        {assert(false);}
    };

    struct VecVar:public ExprVarRef
    {
        VecVar()
        : ExprVarRef(ExprType().FP(3).Varying()), val(0.0)
        {}

        Vec<double,3,false> val; // independent variable

        void eval(double* result) {
            for(int k=0;k<3;k++) result[k] = val[k];
        }

        void eval(const char** reuslt)
        {}
    };

    //! variable map
    mutable std::map<std::string,Var> vars;
    mutable std::map<std::string,VecVar> vecvars;

    //! resolve function that only supports one external variable 'x'
    ExprVarRef* resolveVar(const std::string& name) const
    {
        {
        std::map<std::string,Var>::iterator i=vars.find(name);
        if(i != vars.end()) return &i->second;
        }
        {
        std::map<std::string,VecVar>::iterator i=vecvars.find(name);
        if(i != vecvars.end()) return &i->second;
        }
        { // default to color for any unknown vars
        std::map<std::string,VecVar>::iterator i=vecvars.find("Cs");
        if(i != vecvars.end()) return &i->second;
        }
        return 0;
    }

    ExprFunc mapStub;

    ExprFunc* resolveFunc(const std::string& name) const
    {
        ExprFunc* func = 0;
        if(name=="map")
            func = (ExprFunc *)&mapStub;
        return func;
    }
};


//! Class for evaluating expression and generating 2D image
class TestImage
{
public:
    TestImage();
    bool generateImage(const std::string &exprStr);
    bool writePNGImage(const char* imageFile);
private:
    int _width;
    int _height;
    unsigned char *_image;
};

TestImage::TestImage()
{
    _width = 256;
    _height = 256;
    _image = NULL;
}

//! Evaluate given expression string and generate image
bool TestImage::generateImage(const std::string &exprStr)
{
    ImageSynthExpr expr(exprStr);

    // make variables
    expr.vars["u"]=ImageSynthExpr::Var(0.);
    expr.vars["v"]=ImageSynthExpr::Var(0.);
    expr.vars["w"]=ImageSynthExpr::Var(_width);
    expr.vars["h"]=ImageSynthExpr::Var(_height);

    expr.vars["faceId"]=ImageSynthExpr::Var(0.);
    expr.vecvars["P"]=ImageSynthExpr::VecVar();
    expr.vecvars["Cs"]=ImageSynthExpr::VecVar();
    expr.vecvars["Ci"]=ImageSynthExpr::VecVar();

    // check if expression is valid
    bool valid=expr.isValid();
    if(!valid){
        std::cerr<<"Invalid expression "<<std::endl;
        std::cerr<<expr.parseError()<<std::endl;
        std::cerr<<"string = " << exprStr <<std::endl;
        return valid;
    }

    // evaluate expression
    unsigned char* image=new unsigned char[_width*_height*4];
    double one_over_width=1./_width, one_over_height=1./_height;
    double& u=expr.vars["u"].val;
    double& v=expr.vars["v"].val;


    double& faceId=expr.vars["faceId"].val;
    Vec<double,3,false> &P = expr.vecvars["P"].val;
    Vec<double,3,false> &Cs = expr.vecvars["Cs"].val;
    Vec<double,3,false> &Ci = expr.vecvars["Ci"].val;

    unsigned char* pixel=image;

    for(int row=0;row<_height;row++){
        for(int col=0;col<_width;col++){
            u=one_over_width*(col+.5);
            v=one_over_height*(row+.5);

            faceId=floor(u*5);
            P[0]=u*10;
            P[1]=v*10;
            P[2]=0.5*10;
            Cs[0]=0.2;
            Cs[1]=0.4;
            Cs[2]=0.6;
            Ci[0]=0.2;
            Ci[1]=0.4;
            Ci[2]=0.6;

            const double* result=expr.evalFP();

            pixel[0]=clamp(result[0]*256.);
            pixel[1]=clamp(result[1]*256.);
            pixel[2]=clamp(result[2]*256.);
            pixel[3]=255;
            pixel+=4;
        }
    }
    _image = image;
    return valid;
}

//! Write image to file in PNG format
bool TestImage::writePNGImage(const char* imageFile)
{
    if(_image && imageFile){
        // write image as png
        std::cerr<<"[ WRITE    ] Image: "<<imageFile<<std::endl;
        FILE *fp=fopen(imageFile,"wb");
        if(!fp){
            perror("fopen");
            return false;
        }
        png_structp png_ptr;
        png_infop info_ptr;
        png_ptr=png_create_write_struct(PNG_LIBPNG_VER_STRING,0,0,0);
        info_ptr=png_create_info_struct(png_ptr);
        png_init_io(png_ptr,fp);
        int color_type=PNG_COLOR_TYPE_RGBA;
        png_set_IHDR(png_ptr,info_ptr,_width,_height,8,color_type,
                     PNG_INTERLACE_NONE,PNG_COMPRESSION_TYPE_DEFAULT,
                     PNG_FILTER_TYPE_DEFAULT);
        const unsigned char *ptrs[_height];
        for(int i=0;i<_height;i++){
            ptrs[i]=&_image[_width*i*4];
        }
        png_set_rows(png_ptr,info_ptr,(png_byte**)ptrs);
        png_write_png(png_ptr,info_ptr,PNG_TRANSFORM_IDENTITY,0);

        fclose(fp);
        return true;
    }
    return false;
}

/**************************************************/
/* Testing example expressions to generate images */
/**************************************************/

std::string rootDir("./");
std::string outDir = rootDir;

// Callback from file tree walk function, ftw(), to process each expr file
int evalExpressionFile(const char*filepath,
                       const struct stat */*sb*/,
                       int typeflag)
{
    // Process file types that match *.se.
    if(typeflag == FTW_F) {
        pcrecpp::RE seFileExt(".*\\.se");
        if( seFileExt.FullMatch(filepath) ) {
            // Get expression content of .se file
            std::ifstream ifs(filepath);
            if(ifs.good()){
                std::string exprStr( (std::istreambuf_iterator<char>(ifs) ),
                                     (std::istreambuf_iterator<char>() ) );
                TestImage *_testImage = new TestImage();
                bool result;

                {
#               if 1 //SEEXPR_PERFORMANCE
#               ifdef SEEXPR_ENABLE_LLVM
                    string info("v2 LLVM eval time for ");
#               else
                    string info("v2 interpreter eval time for ");
#               endif
                    PrintTiming timer(info + filepath + " : ");
                    result = _testImage->generateImage(exprStr);
#               endif
                }

                if(result) {
                    std::string outFile(outDir+basename(filepath)+".png");
                    _testImage->writePNGImage(outFile.c_str());
                } else
                    std::cerr<<"Evaluation failure: "<<filepath<<std::endl;
            } else
                std::cerr<<"Failure to open file: "<<filepath<<std::endl;
        }
    }
    return 0;
}

// Test example *.se files in SeExpr demo expressions.
TEST(Examples, SeExprDemos)
{
    outDir = rootDir+"build/demo-images/";

#include <sys/stat.h>
    // make outDir if it doesn't already exist
    struct stat info;
    if(stat( outDir.c_str(), &info ) != 0){
        int status = mkdir( outDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if(status != 0 ){
            std::cerr<<"Failure to mkdir: "<< outDir.c_str() <<std::endl;
            return;
        }
    }

    // Output log file for cerr messages
    std::string logfile(outDir);
    logfile.append("imageTests.log");
    std::ofstream ofs(logfile.c_str());
    std::streambuf *curr_cerr = std::cerr.rdbuf();
    std::cerr.rdbuf(ofs.rdbuf());

    std::string examplePath("./src/demos/imageSynth2/examples2/");

    // Walk files in tree, evaluating expression in each
    timestamp_t start = get_timestamp();
    ftw(examplePath.c_str(), evalExpressionFile, 16);
    timestamp_t end = get_timestamp();

    double elapsedTime = (end - start) / 1000000.0L;
    std::cerr << "Total evaluation time, all files: " << elapsedTime << std::endl;

    std::cerr.rdbuf(curr_cerr);
    ofs.close();
    std::cerr << "[ Logfile  ] " << logfile.c_str() << std::endl;
}

// Test example *.se files in show paint3d expressions.
TEST(Examples, Paint3dShow)
{
    outDir = rootDir+"build/p3d-images/";

#include <sys/stat.h>
    // make outDir if it doesn't already exist
    struct stat info;
    if(stat( outDir.c_str(), &info ) != 0){
        int status = mkdir( outDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if(status != 0 ){
            std::cerr<<"Failure to mkdir: "<< outDir.c_str() <<std::endl;
            return;
        }
    }

    // Output log file for cerr messages
    std::string logfile(outDir+"imageTests.log");
    std::ofstream ofs(logfile.c_str());
    std::streambuf *curr_cerr = std::cerr.rdbuf();
    std::cerr.rdbuf(ofs.rdbuf());

    // Walk files in tree, evaluating expression in each
    timestamp_t start = get_timestamp();
    ftw("/disney/shows/default/rel/global/expressions/",
        evalExpressionFile,
        16);
    timestamp_t end = get_timestamp();

    double elapsedTime = (end - start) / 1000000.0L;
    std::cerr << "Total evaluation time, all files: " << elapsedTime << std::endl;

    std::cerr.rdbuf(curr_cerr);
    ofs.close();
    std::cerr << "[ Logfile  ] " << logfile.c_str() << std::endl;
}
