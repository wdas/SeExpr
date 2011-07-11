/*
 SEEXPR SOFTWARE
 Copyright 2011 Disney Enterprises, Inc. All rights reserved
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are
 met:
 
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in
 the documentation and/or other materials provided with the
 distribution.
 
 * The names "Disney", "Walt Disney Pictures", "Walt Disney Animation
 Studios" or the names of its contributors may NOT be used to
 endorse or promote products derived from this software without
 specific prior written permission from Walt Disney Pictures.
 
 Disclaimer: THIS SOFTWARE IS PROVIDED BY WALT DISNEY PICTURES AND
 CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR PURPOSE, NONINFRINGEMENT AND TITLE ARE DISCLAIMED.
 IN NO EVENT SHALL WALT DISNEY PICTURES, THE COPYRIGHT HOLDER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND BASED ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*/
#ifndef _Graph_h_
#define _Graph_h_

#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QWidget>
#include <QtGui/QStatusBar>
#include "Functions.h"
#include <iostream>

static const int divs=1;

//! Draws graph and handles the state of the graph coordinate system
class Graph:public QWidget
{
    Q_OBJECT;
public:
    enum OperationCode{NONE=0,FIND_MIN,FIND_MAX,FIND_ROOT};
private:

    Functions& funcs;
    float logBase;

    //! operation code type
    OperationCode operationCode;
    //! function that root or extrema is computed on
    int functionIndex;
    //! left bound for extremum (want something unimodal)
    double boundStart;
    //! right bound for extremum (want something unimodal)
    double boundEnd;
    //! rootX and Y location
    double rootX,rootY;
    //! show the root (draws coordinate)
    bool rootShow;
    //! show the root (draws coordinate and interval)
    bool minShow;
    //! Last device coordinates when doing mouse dragging
    int lastcx,lastcy;
    //! Whether user is dragging to translate the graph
    bool dragging;
    //! Whether user is dragging to scale the graph
    bool scaling;
    //! pointer to status bar to handle printing notifications
    QStatusBar* status;

public:
    Graph(Functions* functions,QStatusBar* status);
protected:
    //! Window in graph space
    float xmin,xmax,ymin,ymax;
    //! Do a linear fit between two windowing transforms
    float fit(float t,float src0,float src1,float dest0,float dest1);
    //! Draw X-axis
    void drawX(QPainter& painter,int power,bool label=false);
    //! Draw y-axis
    void drawY(QPainter& painter,int power,bool label=false);
    //! Draw graph
    void paintEvent(QPaintEvent */*event*/);
    //! Plot a single function
    void plot(QPainter& painter,int funcId);

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

    //! Transform x and y into device coordinates cx,cy
    void xform(float x,float y,float& cx,float& cy);
    //! Transform cx and cy into graph coordinates x,y
    void xforminv(float cx,float cy,float& x,float& y);
public:
    //! Start the process of computing extremum
    void scheduleRoot(const OperationCode operationCode_in,const int functionIndex_in);
private:
    //! Solve root using Newton's method
    void solveRoot(const int function,double xInitial);
    //! Solve extrema problem using golden section search
    double golden(const int function,double xmin,double xcenter,double xmax,bool solveMax=false,double tolerance=1e-5);
    //! Solve min problem using golden
    void solveMin(const int function,double xmin,double xmax,bool solveMax=false);
    //! Solve max problem using golden
    void solveMax(const int function,double xmin,double xmax);
public slots:
    //! Redraw graph and clear all given roots
    void redraw();
};


#endif
