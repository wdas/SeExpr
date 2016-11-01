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

// Set up helper methods and classes for generating images from test expressions

#include <map>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <assert.h>
#include <dirent.h>
//#include <gtest/pcrecpp.h>

#include <png.h>

#include <SeExpr2/Expression.h>
#include <functional>

#include <SeExpr2/VarBlock.h>

#include <SeExpr2/ExprFunc.h>
#include <SeExpr2/ExprFuncX.h>
#include <SeExpr2/Platform.h>  // performance timing
#include <memory>

#include <gtest/gtest.h>

double clamp(double x) { return std::max(0., std::min(255., x)); }

#undef USE_OLD_VARS

namespace SeExpr2 {

class RandFuncX : public ExprFuncSimple {

    struct Data : public ExprFuncNode::Data {
        std::vector<std::pair<int, int> > ranges;
        std::string format;
    };

    virtual ExprType prep(ExprFuncNode* node, bool wantScalar, ExprVarEnvBuilder& envBuilder) const {
        bool valid = true;
        for (int i = 0; i < node->numChildren(); i++)
            valid &= node->checkArg(i, ExprType().FP(1).Varying(), envBuilder);
        return valid ? ExprType().FP(1).Varying() : ExprType().Error();
    }

    virtual ExprFuncNode::Data* evalConstant(const ExprFuncNode* node, ArgHandle args) const { return new Data; }

    virtual void eval(ArgHandle args) {
        if (args.nargs() >= 2) {
            args.outFp = (args.inFp<1>(0)[0] - args.inFp<1>(1)[0]) / 2.0;
        } else
            args.outFp = 0.5;
    }

  public:
    RandFuncX() : ExprFuncSimple(true) {}  // Thread Safe
    virtual ~RandFuncX() {}
} rand;

// map(string name, [float format-arg], [float u], [float v], [int channel])
class MapFunc : public ExprFuncSimple {
    struct Data : public ExprFuncNode::Data {
        std::vector<std::pair<int, int> > ranges;
        std::string format;
    };

  public:
    MapFunc() : ExprFuncSimple(true) {}  // Thread Safe
    virtual ~MapFunc() {}

    virtual ExprType prep(ExprFuncNode* node, bool wantScalar, ExprVarEnvBuilder& envBuilder) const {
        bool valid = true;
        valid &= node->checkArg(0, ExprType().String().Constant(), envBuilder);
        for (int i = 1; i < node->numChildren(); i++)
            valid &= node->checkArg(i, ExprType().FP(1).Varying(), envBuilder);
        return valid ? ExprType().FP(3).Varying() : ExprType().Error();
    }

    virtual ExprFuncNode::Data* evalConstant(const ExprFuncNode* node, ArgHandle args) const { return new Data; }

    virtual void eval(ArgHandle args) {
        double* out = &args.outFp;

        double val = 0.5;
        int num = args.nargs();
        if (num > 2)
            for (int k = 2; k < num; k++) val += args.inFp<1>(k)[0];

        for (int k = 0; k < 3; k++) out[k] = val;
    }

} mapStubX;

// triplanar(string name, [vector scale], [float blend], [vector rotation],
//           [vector translation])
class TriplanarFuncX : public ExprFuncSimple {

    virtual ExprType prep(ExprFuncNode* node, bool wantScalar, ExprVarEnvBuilder& envBuilder) const {
        bool valid = true;
        valid &= node->checkArg(0, ExprType().String().Constant(), envBuilder);
        for (int i = 1; i < node->numChildren(); i++)
            valid &= node->checkArg(i, ExprType().FP(3).Varying(), envBuilder);
        return valid ? ExprType().FP(3).Varying() : ExprType().Error();
    }

    virtual ExprFuncNode::Data* evalConstant(const ExprFuncNode* node, ArgHandle args) const { return nullptr; }

    virtual void eval(ArgHandle args) {
        double* out = &args.outFp;

        double val = 0.5;
        int num = args.nargs();
        if (num > 1)
            for (int k = 1; k < num; k++) val += (args.inFp<3>(k)[0] + args.inFp<3>(k)[1] + args.inFp<3>(k)[2]);

        for (int k = 0; k < 3; k++) out[k] = val;
    }

  public:
    TriplanarFuncX() : ExprFuncSimple(true) {}  // Thread Safe
    virtual ~TriplanarFuncX() {}
} triplanarX;

ExprFunc mapStub(mapStubX, 1, 5);
ExprFunc triplanar(triplanarX, 1, 5);
}

using namespace SeExpr2;

//! Simple image synthesizer expression class to test example expressions
class ImageSynthExpr : public Expression {
  public:
    //! Constructor that takes the expression to parse
    ImageSynthExpr(const std::string& expr) : Expression(expr, ExprType().FP(3)) {}

    //! Simple variable that just returns its internal value
    struct Var : public ExprVarRef {
        Var(const double val) : ExprVarRef(ExprType().FP(1).Varying()), val(val) {}

        Var() : ExprVarRef(ExprType().FP(1).Varying()), val(0.0) {}

        double val;  // independent variable
        void eval(double* result) { result[0] = val; }

        void eval(const char** result) { assert(false); }
    };

    struct VecVar : public ExprVarRef {
        VecVar() : ExprVarRef(ExprType().FP(3).Varying()), val(0.0) {}

        Vec<double, 3, false> val;  // independent variable

        void eval(double* result) {
            for (int k = 0; k < 3; k++) result[k] = val[k];
        }

        void eval(const char** reuslt) {}
    };

    //! variable map
    mutable std::map<std::string, Var> vars;
    mutable std::map<std::string, VecVar> vecvars;
    mutable VarBlockCreator blockCreator;

    //! resolve function that only supports one external variable 'x'
    ExprVarRef* resolveVar(const std::string& name) const {
#ifdef USE_OLD_VARS
        {
            std::map<std::string, Var>::iterator i = vars.find(name);
            if (i != vars.end()) return &i->second;
        }
        {
            std::map<std::string, VecVar>::iterator i = vecvars.find(name);
            if (i != vecvars.end()) return &i->second;
        }
        {  // default to color for any unknown vars
            std::map<std::string, VecVar>::iterator i = vecvars.find("Cs");
            if (i != vecvars.end()) return &i->second;
        }
        return nullptr;
#else
        if (ExprVarRef* blockVar = blockCreator.resolveVar(name))
            return blockVar;
        else if (ExprVarRef* blockVar = blockCreator.resolveVar("Cs"))
            return blockVar;
        else
            return nullptr;
#endif
    }

    ExprFunc* resolveFunc(const std::string& name) const {
        if (name == "map") return &mapStub;
        if (name == "triplanar") return &triplanar;
        return nullptr;
    }
};

//! Class for evaluating expression and generating 2D image
class TestImage {
  public:
    TestImage();
    bool generateImage(const std::string& exprStr);
    template <class TFUNC>
    bool generateImageWithoutExpression(TFUNC func);
    bool writePNGImage(const char* imageFile);

  private:
    int _width;
    int _height;
    std::vector<unsigned char> _image;
};

TestImage::TestImage() : _width(256), _height(256) {}

template <class TFUNC>
bool TestImage::generateImageWithoutExpression(TFUNC func) {
    Timer totalTime;
    totalTime.start();
    Timer prepareTiming;
    prepareTiming.start();

    testing::Test::RecordProperty("prepareTime", prepareTiming.elapsedTime());
    // evaluate expression
    std::vector<unsigned char> image(_width * _height * 4);
    double one_over_width = 1. / _width, one_over_height = 1. / _height;
    double u = 0;
    double v = 0;
    Vec<double, 3, false> P;
    Vec<double, 3, false> Cs;
    Vec<double, 3, false> Ci;
    Vec<double, 3, false> result;
    double faceId;

    unsigned char* pixel = image.data();

#ifdef SEEXPR_PERFORMANCE
    PrintTiming timer("[ EVAL     ] v2 eval time: ");
#endif
    Timer evalTiming;
    evalTiming.start();

    for (int row = 0; row < _height; row++) {
        for (int col = 0; col < _width; col++) {
            u = one_over_width * (col + .5);
            v = one_over_height * (row + .5);

            faceId = floor(u * 5);
            P[0] = u * 10;
            P[1] = v * 10;
            P[2] = 0.5 * 10;
            Cs[0] = 0.2;
            Cs[1] = 0.4;
            Cs[2] = 0.6;
            Ci[0] = 0.2;
            Ci[1] = 0.4;
            Ci[2] = 0.6;
            func(u, v, P, Cs, Ci, faceId, result);
            pixel[0] = clamp(result[0] * 256.);
            pixel[1] = clamp(result[1] * 256.);
            pixel[2] = clamp(result[2] * 256.);
            pixel[3] = 255;
            pixel += 4;
        }
    }
    _image = std::move(image);
    testing::Test::RecordProperty("evalTime", evalTiming.elapsedTime());
    testing::Test::RecordProperty("totalTime", totalTime.elapsedTime());
    return true;
}

//! Write image to file in PNG format
bool TestImage::writePNGImage(const char* imageFile) {
    if (_image.size() && imageFile) {
        // write image as png
        std::cout << "[ WRITE    ] Image: " << imageFile << std::endl;
        FILE* fp = fopen(imageFile, "wb");
        if (!fp) {
            perror("fopen");
            return false;
        }
        png_structp png_ptr;
        png_infop info_ptr;
        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
        info_ptr = png_create_info_struct(png_ptr);
        png_init_io(png_ptr, fp);
        int color_type = PNG_COLOR_TYPE_RGBA;
        png_set_IHDR(png_ptr,
                     info_ptr,
                     _width,
                     _height,
                     8,
                     color_type,
                     PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_DEFAULT,
                     PNG_FILTER_TYPE_DEFAULT);
        const unsigned char* ptrs[_height];
        for (int i = 0; i < _height; i++) {
            ptrs[i] = &_image[_width * i * 4];
        }
        png_set_rows(png_ptr, info_ptr, (png_byte**)ptrs);
        png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, 0);

        fclose(fp);
        return true;
    }
    return false;
}

//! Evaluate given expression string and generate image
bool TestImage::generateImage(const std::string& exprStr) {
    Timer totalTime;
    totalTime.start();
    Timer prepareTiming;
    prepareTiming.start();
    ImageSynthExpr expr(exprStr);

#ifdef USE_OLD_VARS
    // make variables
    expr.vars["u"] = ImageSynthExpr::Var(0.);
    expr.vars["v"] = ImageSynthExpr::Var(0.);
    expr.vars["w"] = ImageSynthExpr::Var(_width);
    expr.vars["h"] = ImageSynthExpr::Var(_height);

    expr.vars["faceId"] = ImageSynthExpr::Var(0.);
    expr.vecvars["P"] = ImageSynthExpr::VecVar();
    expr.vecvars["Cs"] = ImageSynthExpr::VecVar();
    expr.vecvars["Ci"] = ImageSynthExpr::VecVar();

#else
    int dummyH = expr.blockCreator.registerVariable("dummy", ExprType().FP(3).Varying());
    int uH = expr.blockCreator.registerVariable("u", ExprType().FP(1).Varying());
    int vH = expr.blockCreator.registerVariable("v", ExprType().FP(1).Varying());
    int wH = expr.blockCreator.registerVariable("w", ExprType().FP(1).Uniform());
    int hH = expr.blockCreator.registerVariable("h", ExprType().FP(1).Uniform());
    int faceIdH = expr.blockCreator.registerVariable("faceId", ExprType().FP(1).Varying());
    int PH = expr.blockCreator.registerVariable("P", ExprType().FP(3).Varying());
    int CsH = expr.blockCreator.registerVariable("Cs", ExprType().FP(3).Varying());
    int CiH = expr.blockCreator.registerVariable("Ci", ExprType().FP(3).Varying());
#endif
    VarBlock varBlock = expr.blockCreator.create();

    // check if expression is valid
    bool valid = expr.isValid();
    if (!valid) {
        std::cerr << "Invalid expression " << std::endl;
        std::cerr << expr.parseError() << std::endl;
        return valid;
    }
    testing::Test::RecordProperty("prepareTime", prepareTiming.elapsedTime());
    // evaluate expression
    std::vector<unsigned char> image(_width * _height * 4);
    double one_over_width = 1. / _width, one_over_height = 1. / _height;
#ifdef USE_OLD_VARS
    double& u = expr.vars["u"].val;
    double& v = expr.vars["v"].val;
    double& faceId = expr.vars["faceId"].val;
    Vec<double, 3, false>& P = expr.vecvars["P"].val;
    Vec<double, 3, false>& Cs = expr.vecvars["Cs"].val;
    Vec<double, 3, false>& Ci = expr.vecvars["Ci"].val;
#else
    Vec<double, 3> dummy(0.);
    varBlock.Pointer(dummyH) = &dummy[0];
    double widthDouble = _width, heightDouble = _height;
    varBlock.Pointer(wH) = &widthDouble;
    varBlock.Pointer(hH) = &heightDouble;
    double u, v, faceId;
    Vec<double, 3> P, Cs, Ci;
    varBlock.Pointer(uH) = &u;
    varBlock.Pointer(vH) = &v;
    varBlock.Pointer(faceIdH) = &faceId;
    varBlock.Pointer(PH) = &P[0];
    varBlock.Pointer(CsH) = &Cs[0];
    varBlock.Pointer(CiH) = &Ci[0];
#endif
    unsigned char* pixel = image.data();

#ifdef SEEXPR_PERFORMANCE
    PrintTiming timer("[ EVAL     ] v2 eval time: ");
#endif
    Timer evalTiming;
    evalTiming.start();
    for (int row = 0; row < _height; row++) {
        for (int col = 0; col < _width; col++) {
            u = one_over_width * (col + .5);
            v = one_over_height * (row + .5);

            faceId = floor(u * 5);
            P[0] = u * 10;
            P[1] = v * 10;
            P[2] = 0.5 * 10;
            Cs[0] = 0.2;
            Cs[1] = 0.4;
            Cs[2] = 0.6;
            Ci[0] = 0.2;
            Ci[1] = 0.4;
            Ci[2] = 0.6;

            const double* result = expr.evalFP(&varBlock);

            pixel[0] = clamp(result[0] * 256.);
            pixel[1] = clamp(result[1] * 256.);
            pixel[2] = clamp(result[2] * 256.);
            pixel[3] = 255;
            pixel += 4;
        }
    }
    _image = std::move(image);
    testing::Test::RecordProperty("evalTime", evalTiming.elapsedTime());
    testing::Test::RecordProperty("totalTime", totalTime.elapsedTime());
    return valid;
}

/**************************************************/
/* Testing example expressions to generate images */
/**************************************************/

std::string rootDir("./");
std::string outDir = rootDir;

// Evaluate expression in given file and generate output image.
void evalExpressionFile(const char* filepath) {
    testing::Test::RecordProperty("path", filepath);
    std::ifstream ifs(filepath);
    if (ifs.good()) {
        std::string exprStr((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        TestImage* _testImage = new TestImage();
        bool result;

        result = _testImage->generateImage(exprStr);
        ASSERT_TRUE(result) << "Evaluation failure: " << filepath;

        // make outDir if it doesn't already exist
        std::string outDir("./build/images/");
#include <sys/stat.h>
        struct stat info;
        if (stat(outDir.c_str(), &info) != 0) {
            int status = mkdir(outDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            ASSERT_EQ(status, 0) << "Failure to mkdir: " << outDir.c_str();
        }
        std::string outFile(outDir + basename(const_cast<char*>(filepath)) + ".png");
        _testImage->writePNGImage(outFile.c_str());
    }
}

TEST(perf, noexpr) {
    Timer totalTime;
    totalTime.start();
    Timer prepareTime;
    prepareTime.start();
    TestImage foo;
    typedef Vec<double, 3, false> VecT;
    float prept = prepareTime.elapsedTime();

    auto func = [](double u, double v, const VecT& P, const VecT& Cs, const VecT& Ci, double faceId, VecT& result) {
        VecT foo(u * u + v * v), bar = foo * foo;
        bar += VecT(u * v);
        result = bar;
    };

    Timer evalTiming;
    evalTiming.start();
    if (true) {
        foo.generateImageWithoutExpression(func);
    } else {
        std::function<void(double, double, const VecT&, const VecT&, const VecT&, double, VecT&)> slowFunction = func;
        foo.generateImageWithoutExpression(slowFunction);
    }
    float evalt = evalTiming.elapsedTime();
    float totalt = totalTime.elapsedTime();
    std::cerr << "evalt " << evalt << " prept " << prept << " totalt " << totalt << std::endl;
    testing::Test::RecordProperty("prepareTime", std::to_string(prept).c_str());
    testing::Test::RecordProperty("evalTime", std::to_string(evalt).c_str());
    testing::Test::RecordProperty("totalTime", std::to_string(totalt).c_str());
    foo.writePNGImage("/tmp/ka.png");
}

static int forceStaticInitializationToMakeTimingBetter = []() {
    SeExpr2::ExprFunc::init();  // force static initialize
    #if 0
    ImageSynthExpr e("deepTMA(P,1,1,1,1,1,1,1,1,1,1,1,1,1)");
    if (!e.isValid()) {
        std::cerr << "INVALID!" << std::endl;
    }
    const double* foo=e.evalFP();
    std::cout<<"foo "<<foo[0]<<" "<<std::endl;
    #endif
    return 1;
}();
