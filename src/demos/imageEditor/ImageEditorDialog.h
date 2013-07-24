/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
*/

/**
   @file ImageEditorDialog.h
*/

#include <QtGui/QDialog>

class QLabel;
class SeExprEditor;
class ImageSynthesizer;

class ImageEditorDialog: public QDialog
{
    Q_OBJECT
public:
    ImageEditorDialog(QWidget *parent=0);
private:
    QLabel *_imageLabel;
    SeExprEditor *_editor;
    ImageSynthesizer *_imageSynthesizer;
private slots:
    void applyExpression();
};
