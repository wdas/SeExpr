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

#pragma once

#include <algorithm>
#include <limits>

#include <SeExpr2/VarBlock.h>

#include "png++.h"

struct SimpleExpression : public SeExpr2::Expression {
    SimpleExpression(const SeExpr2::VarBlockCreator& creator_, const std::string& expr)
        : SeExpr2::Expression(), _creator(creator_) {
        setExpr(expr);
        setVarBlockCreator(&_creator);
    }

  private:
    SeExpr2::ExprVarRef* resolveVar(const std::string& name) const override { return _creator.resolveVar(name); }

    const SeExpr2::VarBlockCreator& _creator;
};

struct ImageSynthVars : public SeExpr2::VarBlockCreator {
    ImageSynthVars() {
        u = registerVariable("u", SeExpr2::ExprType().FP(1).Varying());
        v = registerVariable("v", SeExpr2::ExprType().FP(1).Varying());
        w = registerVariable("w", SeExpr2::ExprType().FP(1).Varying());
        h = registerVariable("h", SeExpr2::ExprType().FP(1).Varying());
    }

    int u;
    int v;
    int w;
    int h;
};

template <typename T>
void toPixel(png::GrayPixel<T>& pixel, const double* result) {
    pixel.value = std::numeric_limits<T>::max() * result[0];
}

template <typename T>
void toPixel(png::RGBPixel<T>& pixel, const double* result) {
    pixel.r = std::numeric_limits<T>::max() * result[0];
    pixel.g = std::numeric_limits<T>::max() * result[1];
    pixel.b = std::numeric_limits<T>::max() * result[2];
}

template <typename T>
void toPixel(png::RGBAPixel<T>& pixel, const double* result) {
    pixel.r = std::numeric_limits<T>::max() * result[0];
    pixel.g = std::numeric_limits<T>::max() * result[1];
    pixel.b = std::numeric_limits<T>::max() * result[2];
    pixel.a = std::numeric_limits<T>::max();
}

template <typename ImageT>
class ImageSynthesizer {
    typedef typename ImageT::pixel_type PixelT;
    typedef typename PixelT::component_type ComponentT;

  public:
    ImageSynthesizer(ImageT& image_) : _image(image_) {}

    bool evaluateExpression(const std::string& exprStr) {
        static ImageSynthVars creator;

        SimpleExpression expr(creator, exprStr);

        if (!expr.isValid()) {
            std::cerr << "Invalid expression " << std::endl;
            std::cerr << expr.parseError() << std::endl;
            return false;
        }

        double width = _image.width();
        double height = _image.height();
        double one_over_width = 1. / width, one_over_height = 1. / height;
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                double u = one_over_width * (x + .5);
                double v = one_over_height * (y + .5);

                SeExpr2::VarBlock varBlock = creator.create();
                varBlock.Pointer(creator.u) = &u;
                varBlock.Pointer(creator.v) = &v;
                varBlock.Pointer(creator.w) = &width;
                varBlock.Pointer(creator.h) = &height;

                toPixel<ComponentT>(_image[y][x], expr.evalFP(&varBlock));
            }
        }

        return true;
    }

  private:
    ImageT& _image;
};
