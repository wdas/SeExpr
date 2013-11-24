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
* @file SeExprEdGrapher2d.h
* @brief A 2d image graph view for expression editing previewing
* @author  jlacewel
*/
#ifndef SeExprEdGrapher2d_h
#define SeExprEdGrapher2d_h

#include <QtCore/QObject>
#include <QtGui/QPalette>
#include <QtOpenGL/QGLWidget>
#include <QtGui/QMouseEvent>

#include "SeExprEdExpression.h"

class SeExprEdGrapherWidget;
class QLineEdit;

class SeExprEdGrapherView : public QGLWidget
{
    Q_OBJECT;
    SeExprEdGrapherWidget& widget;
public:
        SeExprEdGrapherView(SeExprEdGrapherWidget& widget,QWidget* parent, int width, int height);
        virtual ~SeExprEdGrapherView();

        void update();
        void setWindow(float xmin,float xmax,float ymin,float ymax,float z);
        void getWindow(float &xmin,float& xmax,float& ymin,float &ymax,float &z);

    protected:
        void clear();
        void paintGL();
        void mousePressEvent(QMouseEvent* event);
        void mouseReleaseEvent(QMouseEvent* event);
        void mouseMoveEvent(QMouseEvent* event);
        int event_oldx,event_oldy;

    signals:
        void scaleValueManipulated();
        void clicked();

    private:
        float * _image;
        int _width;
        int _height;

        float xmin,xmax,ymin,ymax,z;
        float dx,dy;


    bool scaling,translating;

};

class SeExprEdGrapherWidget : public QWidget
{
    Q_OBJECT
         QLineEdit* scale;

    public:
        SeExprEdGrapherView* view;
        SeExprEdExpression expr;

        SeExprEdGrapherWidget(QWidget* parent, int width, int height);

        void update();
signals:
        void preview();
private slots:
        void scaleValueEdited();
        void scaleValueManipulated();
        void forwardPreview();

};

#endif
