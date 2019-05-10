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
   @file imageEditor.cpp
*/

#include <iostream>
#include <string>
#include <png.h>

#include <QApplication>
#include <QDialog>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QImage>
#include <QPushButton>
#include <QMessageBox>
#include <QStyleFactory>

#include "ExprControlCollection.h"
#include "ExprEditor.h"
#include "ExprBrowser.h"
#include "Expression.h"

#include "../common/palette.h"
#include "ImageEditorDialog.h"

using namespace SeExpr2;

//-- IMAGE EDITOR DIALOG METHODS --//

ImageEditorDialog::ImageEditorDialog(QWidget* parent) : QDialog(parent), _image(256, 256), _imageSynthesizer(_image)
{
    this->setWindowTitle("Image Synthesis Editor");

    // Image Previewer
    _imageLabel = new QLabel();
    _imageLabel->setFixedSize(_image.width(), _image.height());
    _imageLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    // Locate logo image relative to location of the app itself
    QString imageFile = QCoreApplication::applicationDirPath() + "/../share/doc/SeExpr2/seexprlogo.png";
    QImage image(imageFile);  // just a fun default

    QPixmap imagePixmap = QPixmap::fromImage(image);
    imagePixmap = imagePixmap.scaled(_image.width(), _image.height(), Qt::KeepAspectRatio);
    _imageLabel->setPixmap(imagePixmap);
    QWidget* imagePreviewWidget = new QWidget();
    QHBoxLayout* imagePreviewLayout = new QHBoxLayout(imagePreviewWidget);
    imagePreviewLayout->addStretch();
    imagePreviewLayout->addWidget(_imageLabel);
    imagePreviewLayout->addStretch();

    // Expression controls
    ExprControlCollection* controls = new ExprControlCollection();
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setMinimumHeight(100);
    scrollArea->setFixedWidth(450);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(controls);

    // Expression editor
    _editor = new ExprEditor(this, controls);

    // Expression browser
    ExprBrowser* browser = new ExprBrowser(0, _editor);

    // Add user expressions, example expressions to browser list.
    browser->addUserExpressionPath("imageEditor");
#ifdef IMAGE_EDITOR_ROOT
    std::string exPathStr = IMAGE_EDITOR_ROOT;
    exPathStr += "/share/SeExpr2/expressions";
    browser->addPath("Examples", exPathStr);
#else
    browser->addPath("Examples", "./src/demos/imageEditor");
#endif
    browser->update();

    // Create apply button and connect to image preview.
    QPushButton* applyButton = new QPushButton("Apply");
    connect(applyButton, SIGNAL(clicked()), (ImageEditorDialog*)this, SLOT(applyExpression()));

    // Layout widgets: Top section contains left side with previewer and
    // controls, right side with browser.  Bottom section contains editor
    // and apply button.
    QVBoxLayout* rootLayout = new QVBoxLayout();
    this->setLayout(rootLayout);

    QWidget* topWidget = new QWidget();
    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->setContentsMargins(0, 0, 0, 0);
    topWidget->setLayout(topLayout);

    QWidget* leftWidget = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftWidget->setLayout(leftLayout);
    leftLayout->addWidget(imagePreviewWidget);
    leftLayout->addWidget(scrollArea, 1);

    QWidget* bottomWidget = new QWidget();
    QVBoxLayout* bottomLayout = new QVBoxLayout();
    bottomLayout->setContentsMargins(0, 0, 0, 0);
    bottomWidget->setLayout(bottomLayout);

    QWidget* buttonWidget = new QWidget();
    QHBoxLayout* buttonLayout = new QHBoxLayout(0);
    buttonWidget->setLayout(buttonLayout);
    buttonLayout->addWidget(applyButton);

    topLayout->addWidget(leftWidget);
    topLayout->addWidget(browser, 1);

    bottomLayout->addWidget(_editor);
    bottomLayout->addWidget(buttonWidget);

    rootLayout->addWidget(topWidget);
    rootLayout->addWidget(bottomWidget);
}

// Apply expression, if any, from the editor contents to the preview image
void ImageEditorDialog::applyExpression()
{
    std::string exprStr = _editor->getExpr();
    if (exprStr.empty()) {
        QMessageBox msgBox;
        msgBox.setText("No expression entered in the editor.");
        msgBox.exec();
    } else {
        if (_imageSynthesizer.evaluateExpression(exprStr)) {
            QImage image((const unsigned char*)_image.data(), _image.width(), _image.height(), QImage::Format_RGB32);
            QPixmap imagePixmap = QPixmap::fromImage(image);
            _imageLabel->setPixmap(imagePixmap);
        } else {
            QMessageBox msgBox;
            msgBox.setText("Error evaluating expression to create preview image.");
            msgBox.exec();
        }
    }
}

//-- MAIN --//

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create("Fusion"));
    app.setPalette(createDefaultColorPalette());
    app.setFont(QFont("Consolas", 12));

    ImageEditorDialog* dialog = new ImageEditorDialog(0);
    dialog->show();
    app.exec();
    return 0;
}
