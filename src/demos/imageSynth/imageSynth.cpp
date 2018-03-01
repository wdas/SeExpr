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
#include <iostream>
#include <Expression.h>
#include <Interpreter.h>
#include <Platform.h>
#include <fstream>
#include "../common/ImageSynthesizer.h"

using namespace SeExpr2;

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <image file> <width> <height> <exprFile>" << std::endl;
        return 1;
    }

    // parse arguments
    const char* imageFile = argv[1];
    const char* exprFile = argv[4];
    int width = atoi(argv[2]), height = atoi(argv[3]);
    if (width < 0 || height < 0) {
        std::cerr << "invalid width/height" << std::endl;
        return 1;
    }

    std::ifstream istream(exprFile);
    if (!istream) {
        std::cerr << "Cannot read file " << exprFile << std::endl;
        return 1;
    }
    std::string exprStr((std::istreambuf_iterator<char>(istream)), std::istreambuf_iterator<char>());

    png::Image<png::RGBPixel<uint8_t>> image(width, height);
    ImageSynthesizer<png::Image<png::RGBPixel<uint8_t>>> synthesizer(image);
    synthesizer.evaluateExpression(exprStr);

    std::ofstream out(imageFile);
    if (!out.is_open()) {
        std::cerr << "Failed to open '" << imageFile << "' for writing" << std::endl;
        return 1;
    }

    image.write(out);
}
