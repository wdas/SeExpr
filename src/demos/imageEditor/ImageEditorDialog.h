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

/**
   @file ImageEditorDialog.h
*/

#include <QDialog>

#include "../common/ImageSynthesizer.h"

class QLabel;

namespace SeExpr2 {

class ExprEditor;
};

class ImageEditorDialog : public QDialog {
    Q_OBJECT
  public:
    ImageEditorDialog(QWidget* parent = 0);

  private:
    QLabel* _imageLabel;
    SeExpr2::ExprEditor* _editor;
    png::Image<png::RGB32Pixel> _image;
    ImageSynthesizer<png::Image<png::RGB32Pixel>> _imageSynthesizer;
  private slots:
    void applyExpression();
};
