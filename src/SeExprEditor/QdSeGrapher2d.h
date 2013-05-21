/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
*
* @file QdSeGrapher2d.h
* @brief A 2d image graph view for expression editing previewing
* @author  jlacewel
*/
#ifndef QdSeGrapher_h
#define QdSeGrapher_h

#include <QtCore/QObject>
#include <QtGui/QPalette>
#include <QtOpenGL/QGLWidget>
#include <QtGui/QMouseEvent>

#include "QdSeExpr.h"

class QdSeGrapherWidget;
class QLineEdit;

class QdSeGrapherView : public QGLWidget
{
    Q_OBJECT;
    QdSeGrapherWidget& widget;
public:
        QdSeGrapherView(QdSeGrapherWidget& widget,QWidget* parent, int width, int height);
        virtual ~QdSeGrapherView();

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

class QdSeGrapherWidget : public QWidget
{
    Q_OBJECT
         QLineEdit* scale;

    public:
        QdSeGrapherView* view;
        QdSeExpr expr;

        QdSeGrapherWidget(QWidget* parent, int width, int height);

        void update();
signals:
        void preview();
private slots:
        void scaleValueEdited();
        void scaleValueManipulated();
        void forwardPreview();

};

#endif
