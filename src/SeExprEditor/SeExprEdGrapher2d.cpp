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
* @file SeExprEdGrapherView.cpp
* @brief A 2d image graph view for expression editing previewing
* @author  jlacewel
*/

#include "SeExprEdGrapher2d.h"
#include <QtGui/QGridLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QDoubleValidator>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>


SeExprEdGrapherWidget::SeExprEdGrapherWidget(QWidget* parent,int width,int height)
    :view(new SeExprEdGrapherView(*this,this,width,height)),expr("",false)
{
    Q_UNUSED(parent);
    setFixedSize(width, height+30);
    QVBoxLayout* vbox=new QVBoxLayout;
    vbox->setMargin(0);
    setLayout(vbox);
    vbox->addWidget(view,0,Qt::AlignLeft | Qt::AlignTop);
    QHBoxLayout* hbox=new QHBoxLayout;
    vbox->addLayout(hbox);
    hbox->setMargin(0);

    float xmin,xmax,ymin,ymax,z;
    view->getWindow(xmin,xmax,ymin,ymax,z);
    scale=new QLineEdit();
    QDoubleValidator *valValidator = new QDoubleValidator(0.0,10000000.0,6,scale);
    scale->setValidator(valValidator);
    scale->setValidator(valValidator);
    scaleValueManipulated();


    connect(scale, SIGNAL(returnPressed()), this, SLOT(scaleValueEdited()));
    connect(view, SIGNAL(scaleValueManipulated()), this, SLOT(scaleValueManipulated()));
    connect(view, SIGNAL(clicked()), this, SLOT(forwardPreview()));

    hbox->addWidget(new QLabel("Width"),0);
    hbox->addWidget(scale,0);
}

void SeExprEdGrapherWidget::scaleValueEdited()
{
    float xmin,xmax,ymin,ymax,z;
    view->getWindow(xmin,xmax,ymin,ymax,z);
    float xdiff=xmax-xmin,ydiff=ymax-ymin;
    float xcenter=.5*(xmax+xmin),ycenter=.5*(ymin+ymax);
    float newScale=atof(scale->text().toStdString().c_str());

    float aspect=ydiff/xdiff;

    xmin=xcenter-newScale;
    xmax=xcenter+newScale;
    ymin=ycenter-aspect*newScale;
    ymax=ycenter+aspect*newScale;
    view->setWindow(xmin,xmax,ymin,ymax,z);
}

void SeExprEdGrapherWidget::scaleValueManipulated()
{
    float xmin,xmax,ymin,ymax,z;
    view->getWindow(xmin,xmax,ymin,ymax,z);
    scale->setText(QString("%1").arg(.5*(xmax-xmin)));
}

void SeExprEdGrapherWidget::update()
{
    expr.setWantVec(true);

    view->update();
}

void SeExprEdGrapherWidget::forwardPreview()
{
    emit preview();
}


SeExprEdGrapherView::SeExprEdGrapherView(SeExprEdGrapherWidget& widget,QWidget* parent, int width, int height)
    : QGLWidget(parent), widget(widget), _image(NULL), _width(width), _height(height),
     scaling(false),translating(false)
{
    this->setFixedSize(width, height);

    _image = new float[3*_width*_height];
    setWindow(-1,1,-1,1,0);
    clear();

    setCursor(Qt::OpenHandCursor);

}

SeExprEdGrapherView::~SeExprEdGrapherView()
{
    delete [] _image;
}

void SeExprEdGrapherView::setWindow(float xmin,float xmax,float ymin,float ymax,float z)
{
    this->z=z;
    this->xmin=xmin;
    this->xmax=xmax;
    this->ymin=ymin;
    this->ymax=ymax;
    
    dx=(xmax-xmin)/_width;
    dy=(ymax-ymin)/_height;
}

void SeExprEdGrapherView::getWindow(float& xmin,float& xmax,float& ymin,float& ymax,float& z)
{
    z=this->z;
    xmin=this->xmin;
    xmax=this->xmax;
    ymin=this->ymin;
    ymax=this->ymax;
}

void SeExprEdGrapherView::clear()
{
    for (int row = 0; row < _height; ++row)  {
        for (int col = 0; col < _width; ++col) {
            int index = 3*row*_width + 3*col;
            _image[index] = 1.0f;
            _image[index+1] = 0.0f;
            _image[index+2] = 0.0f;
        }
    }
}

void SeExprEdGrapherView::mousePressEvent(QMouseEvent* event)
{
    if(event->button()==Qt::MidButton){
        setCursor(Qt::ClosedHandCursor);
        translating=true;
    }
    if(event->button()==Qt::RightButton){
        setCursor(Qt::SizeAllCursor);
        scaling=true;
    }
    event_oldx=event->x();
    event_oldy=event->y();

}
void SeExprEdGrapherView::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->button()==Qt::LeftButton)
        emit clicked();
    scaling=translating=false;
    setCursor(Qt::OpenHandCursor);
}
void SeExprEdGrapherView::mouseMoveEvent(QMouseEvent* event)
{
    int x=event->x(),y=event->y();
    float offsetx=dx*(x-event_oldx);
    float offsety=-dy*(y-event_oldy);

    if(translating){
        xmin-=offsetx;
        xmax-=offsetx;
        ymin-=offsety;
        ymax-=offsety;
        update();
        repaint();
    }else if(scaling){
        float offset=(fabs(offsetx)>fabs(offsety))?offsetx:offsety;

        float width=.5*(xmax-xmin),height=.5*(ymax-ymin);
        float xcenter=.5*(xmin+xmax),ycenter=.5*(ymin+ymax);
        // Use float args for pow() to fix Windows compile error
        float scale_factor=pow(10.f,-offset/(xmax-xmin));
        width*=scale_factor;
        height*=scale_factor;
        setWindow(xcenter-width,xcenter+width,ycenter-height,ycenter+height,z);
        emit scaleValueManipulated();
        update();
        repaint();
    }
    event_oldx=x;
    event_oldy=y;
}


void SeExprEdGrapherView::update()
{

    if (!widget.expr.isValid()) {
        clear(); 
        updateGL();
        return;
    }


    float dv = 1.0f / _height;
    float du = 1.0f / _width;

    float y=.5*dy+ymin;
    float v=.5*dv;
    int index=0;
    for(int row=0;row<_height;row++,y+=dy,v+=dv){
        float x=.5*dx+xmin;
        float u=.5*du;
        widget.expr.v.value=v;
        for(int col=0;col<_width;col++,x+=dy,u+=du){
            widget.expr.u.value=u;
            widget.expr.P.value=SeVec3d(x,y,z);
            double* value=widget.expr.evaluate();
            _image[index] = value[0];
            _image[index+1] = value[1];
            _image[index+2] = value[2];
            index+=3;
        }
    }

    updateGL();
}

void SeExprEdGrapherView::paintGL()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, (GLfloat)_width, 0.0, (GLfloat)_height, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glDisable(GL_DEPTH_TEST);
    glDepthFunc(0);
    glClearColor(1,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    glRasterPos2i(0,0);
    glDrawPixels(_width, _height, GL_RGB, GL_FLOAT, _image); 

}

