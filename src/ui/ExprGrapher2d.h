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
*
* @file ExprGrapher2d.h
* @brief A 2d image graph view for expression editing previewing
* @author  jlacewel
*/
#ifndef ExprGrapher2d_h
#define ExprGrapher2d_h

#include <QLabel>
#include <QObject>
#include <QPalette>
#include <QGLWidget>
#include <QMouseEvent>

#include <SeExpr2/VarBlock.h>

#include "BasicExpression.h"

class ExprGrapherWidget;
class QLineEdit;

class ExprGrapherView : public QGLWidget {
    Q_OBJECT;
    ExprGrapherWidget& widget;

  public:
    ExprGrapherView(ExprGrapherWidget& widget, QWidget* parent, int width, int height);
    virtual ~ExprGrapherView();

    void update();
    void setWindow(float xmin, float xmax, float ymin, float ymax, float z);
    void getWindow(float& xmin, float& xmax, float& ymin, float& ymax, float& z);

    inline float* pixel(int x, int y)
    {
        return &_image[3 * (x + y * _height)];
    }

  protected:
    void clear();
    void paintGL();
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    int event_oldx, event_oldy;

  signals:
    void scaleValueManipulated();
    void clicked();
    void pixelHovered(int, int);

  private:
    float* _image;
    int _width;
    int _height;

    float xmin, xmax, ymin, ymax, z;
    float dx, dy;

    bool scaling, translating;
};

struct ExprGrapherSymbols : public SeExpr2::VarBlockCreator {
    ExprGrapherSymbols()
    {
        u = registerVariable("u", SeExpr2::ExprType().FP(1).Varying());
        v = registerVariable("v", SeExpr2::ExprType().FP(1).Varying());
        P = registerVariable("P", SeExpr2::ExprType().FP(3).Varying());
    }

    int u;
    int v;
    int P;
};

class ExprGrapherWidget : public QWidget {
    Q_OBJECT
    QLineEdit* scale;
    QLabel* _pixelLabel;

  public:
    BasicExpression expr;
    ExprGrapherView* view;

    ExprGrapherWidget(QWidget* parent, int width, int height);

    inline bool exprValid() const
    {
        return expr.getExpr().empty() || expr.isValid();
    }

    void update();

  signals:
    void preview();
  private slots:
    void scaleValueEdited();
    void scaleValueManipulated();
    void forwardPreview();
    void updatePixelLabel(int, int);
};

#endif
