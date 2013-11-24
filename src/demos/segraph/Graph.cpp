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
#include "Graph.h"
#include "SeExprMacros.h"
#ifndef SEEXPR_WIN32
#  include <fenv.h>
#endif
#include <cmath>
Graph::Graph(QStatusBar* status,std::vector<GrapherExpr*>& exprs)
    :exprs(exprs),operationCode(NONE),rootShow(false),minShow(false),
    dragging(false),scaling(false),status(status)
{
    // use standard window...
    xmin=-10;xmax=10;
    ymin=-10;ymax=10;
    // use base 2 log for hash marks and grid
    logBase=2.;
}

float Graph::
fit(float t,float src0,float src1,float dest0,float dest1){
    return (t-src0)/(src1-src0)*(dest1-dest0)+dest0;
}

void Graph::
drawX(QPainter& painter,int power,bool label)
{
    float delta=pow(logBase,power-1)/divs;
    float xline=ceil(xmin/delta)*delta;
    QString format("%1");
    for(;xline<xmax;xline+=delta){
        float x1,x2,y1,y2;
        xform(xline,ymin,x1,y1);
        xform(xline,ymax,x2,y2);
        if(label){
            painter.drawText(QPoint(x1+3,y1-5),format.arg(xline));
        }else{
            painter.drawLine(x1,y1,x2,y2);
        }
    }
}

void Graph::
drawY(QPainter& painter,int power,bool label)
{
    QString format("%1");
    float delta=pow(logBase,power-1)/divs;
    float yline=ceil(ymin/delta)*delta;
    for(;yline<ymax;yline+=delta){
        float x1,x2,y1,y2;
        xform(xmin,yline,x1,y1);
        xform(xmax,yline,x2,y2);
        if(label){
            painter.drawText(QPoint(x1+3,y1-4),format.arg(yline));
        }else{
            painter.drawLine(x1,y1,x2,y2);
        }
    }
}

void Graph::
paintEvent(QPaintEvent */*event*/)
{
    QPainter painter(this);

    QBrush brush;brush.setColor(QColor(250,50,50));
    painter.fillRect(0,0,width(),height(),brush);
    
    QPen pen;
    pen.setWidth(1);
    pen.setColor(QColor(210,210,255));
    painter.setPen(pen);

    float powerx=(int)floor(log(xmax-xmin)/log(logBase));
    float powery=(int)floor(log(ymax-ymin)/log(logBase));

    int powerOffsetSmall=4;
    int powerOffsetBig=1;
    drawX(painter,powerx-powerOffsetSmall);
    drawY(painter,powery-powerOffsetSmall);
    
    pen.setWidth(2);
    pen.setColor(QColor(200,200,255));
    painter.setPen(pen);

    drawX(painter,powerx-powerOffsetBig,false);
    drawY(painter,powery-powerOffsetBig,false);

    pen.setColor(Qt::black);
    painter.setPen(pen);
    drawX(painter,powerx-powerOffsetBig,true);
    drawY(painter,powery-powerOffsetBig,true);


    pen.setWidth(2);
    pen.setColor(QColor(50,100,200));
    painter.setPen(pen);
    float x1,y1,x2,y2;
    xform(0,ymin,x1,y1);
    xform(0,ymax,x2,y2);
    painter.drawLine(x1,y1,x2,y2);
    xform(xmin,0,x1,y1);
    xform(xmax,0,x2,y2);
    painter.drawLine(x1,y1,x2,y2);

   



    painter.setRenderHints(QPainter::Antialiasing);
    //for(int i=0;i<funcs.rowCount(QModelIndex());i++){
    //    plot(painter,i);
    //}

    for(unsigned int i=0;i<exprs.size();i++){
        plotNew(painter,i);
    }

    if(rootShow || minShow){
        pen.setColor(Qt::black);
        painter.setPen(pen);
        //std::cerr<<"drawing"<<std::endl;
        float dx,dy;
        xform(rootX,rootY,dx,dy);
        painter.drawEllipse(QPoint(dx,dy),3,3);
        QString text=QString("(%1,%2)").arg(rootX).arg(rootY);
        painter.drawText(QPoint(dx,dy),text);
    }


    if(minShow || operationCode==FIND_MIN || operationCode==FIND_MAX){
        QBrush brush(Qt::yellow);
        painter.setBrush(brush);
        //std::cerr<<"draw boundstart "<<boundStart<<" "<<boundEnd<<std::endl;
        xform(boundStart,ymin,x1,y1);
        xform(boundStart,ymax,x1,y2);
        xform(boundEnd,ymin,x2,y1);
        xform(boundEnd,ymax,x2,y2);
        //painter.drawLine(x1,y1,x1,y2);
        //painter.drawLine(x2,y1,x2,y2);
        painter.setOpacity(.2);
        painter.drawRect(x1,y1,x2-x1,y2-y1);
        painter.setOpacity(1);
    }

}


void Graph::
plotNew(QPainter& painter,int funcId)
{
    GrapherExpr& expr=*exprs[funcId];
    if(!expr.isValid()) return;

    QPen curvepen;
    QColor color=QColor(255,0,0); // funcs.getColor(funcId);
    curvepen.setColor(color);
#if 0
    if(funcs.isSelected(funcId))
        curvepen.setWidth(4);
    else
#endif
        curvepen.setWidth(2);
    
    float xold=xmin;
    QPainterPath path;
    bool first=false;

    expr.setX(xold);
    float yold=expr.evaluate()[0];
    float xd,yd;

    const bool error=false;//isnan(yold);
    xform(xold,yold,xd,yd);
    if(error) first=true;
    else{
        path.moveTo(xd,yd);
        first=false;
    }

    float x_per_pixel=(xmax-xmin)/width(),y_per_pixel=(ymax-ymin)/height();
    int plotted=0;
    float xrate=x_per_pixel*1;
    //fedisableexcept(FE_ALL_EXCEPT);
    while(xold<xmax){
        bool disjoint=false;
        float x=xold+xrate;
        expr.setX(x);
        float y=expr.evaluate()[0];
        if(!error){
            if(fabs(y-yold)>y_per_pixel*100 && xrate>.1*x_per_pixel){xrate/=2;continue;}
            if(fabs(y-yold)<y_per_pixel*5 && xrate<x_per_pixel){xrate*=2;}
                
            float xd,yd;
            xform(x,y,xd,yd);
            if(first || y>ymax || y<ymin){path.moveTo(xd,yd);disjoint=true;}
            else path.lineTo(xd,yd);
        }
        first=false;
        xold=x;
        yold=y;
        plotted++;
    }
    painter.strokePath(path,curvepen);
    //std::cerr<<"Plottted "<<plotted<<std::endl;
        
}

void Graph::
mousePressEvent(QMouseEvent* event)
{
    lastcx=event->x();lastcy=event->y();
    if(event->button()==Qt::MidButton)
        dragging=true;
    else if(event->button()==Qt::RightButton){
        scaling=true;
    }

    if(operationCode && Qt::LeftButton){
        float x,y;
        xforminv(lastcx,lastcy,x,y);
        boundStart=x;
    }
}

void Graph::
mouseMoveEvent(QMouseEvent* event)
{
    if(dragging||scaling||operationCode){
        int newx=event->x(),newy=event->y();
        //std::cerr<<"new "<<newx<<" "<<newy<<std::endl;
        float newx0,newy0,oldx0,oldy0,dx,dy;
        xforminv(newx,newy,newx0,newy0);
        xforminv(lastcx,lastcy,oldx0,oldy0);
        dx=newx0-oldx0;dy=newy0-oldy0;
        if(dragging){
            //std::cerr<<"dx "<<dx<<" "<<dy<<std::endl;
            xmin-=dx;xmax-=dx;
            ymin-=dy;ymax-=dy;
        }else if(scaling){
            float width=(xmax-xmin)/2.,height=(ymax-ymin)/2.;
            float xcenter=(xmax+xmin)/2.,ycenter=(ymax+ymin)/2.;

            width*=pow(10.f,-dx/(xmax-xmin));
            height*=pow(10.f,-dy/(ymax-ymin));
            xmin=xcenter-width;
            ymin=ycenter-height;
            xmax=xcenter+width;
            ymax=ycenter+height;
        }else if(operationCode){
            boundEnd=newx0;
        }
        lastcx=newx;lastcy=newy;
        repaint();
    }

}

void Graph::
mouseReleaseEvent(QMouseEvent* event)
{dragging=false;scaling=false;
    if(operationCode){
        float x,y;
        xforminv(event->x(),event->y(),x,y);
        switch(operationCode){
            case NONE:
                break;
            case FIND_ROOT:
                solveRoot(functionIndex,x);
                break;
            case FIND_MIN:
                solveMin(functionIndex,boundStart,x);
                break;
            case FIND_MAX:
                solveMax(functionIndex,boundStart,x);
                break;
        }
        status->showMessage("");
        operationCode=NONE;
    }
}

void Graph::
xform(float x,float y,float& cx,float& cy)
{
    cx=(x-xmin)/(xmax-xmin)*width();
    cy=height()-(y-ymin)/(ymax-ymin)*height()-1;
}

void Graph::
xforminv(float cx,float cy,float& x,float& y)
{
    x=cx/width()*(xmax-xmin)+xmin;
    y=(height()-cy-1)/height()*(ymax-ymin)+ymin;
}


void Graph::
scheduleRoot(const OperationCode operationCode_in,const int functionIndex_in)
{
    operationCode=operationCode_in;
    functionIndex=functionIndex_in;
    boundStart=xmin-5;
    boundEnd=xmin-3;
    minShow=rootShow=false;
    repaint();
}


void Graph::
solveRoot(const int function,double xInitial)
{
#if 0
    double x0=xInitial;
    double y_x0=funcs.eval(function,x0);
    double x1=xInitial+1e-5;
    double y_x1=funcs.eval(function,x1);
    for(int i=0;i<100 && fabs(x1-x0)>1e-6 && fabs(y_x0-y_x1)>1e-6;i++){
        double x2=x1-(x1-x0)/(y_x1-y_x0)*y_x1;
        x0=x1;
        x1=x2;
        y_x0=y_x1;
        y_x1=funcs.eval(function,x1);
    }
    rootX=x1;
    rootY=y_x1;
    minShow=false;
    rootShow=true;
    repaint();
#else
    UNUSED(function);
    UNUSED(xInitial);
#endif
}


double Graph::
golden(const int function,double xmin,double xcenter,double xmax,bool solveMax,double tolerance)
{
    static const double phi=.5*(1.+sqrt(5.)),resPhi=2-phi;

    //double xcenter=.5*(xmin+xmax);
    double xnew=xcenter+resPhi*(xmax-xcenter);
        
    if(fabs(xmax-xmin)<1e-5*(fabs(xnew)+fabs(xcenter))) return (xmax+xmin)/2;

#if 0
    double f_xcenter=funcs.eval(function,xcenter);
    double f_xnew=funcs.eval(function,xnew);
    if(solveMax){
        f_xcenter*=-1;
        f_xnew*=-1;
    }

    if(f_xnew<f_xcenter) return golden(function,xcenter,xnew,xmax,solveMax,tolerance);
    else return golden(function,xnew,xcenter,xmin,solveMax,tolerance);
#else
    UNUSED(function);
    UNUSED(solveMax);
    UNUSED(tolerance);
#endif
    return 0;
}

void Graph::
solveMin(const int function,double xmin,double xmax,bool solveMax)
{
#if 0
    if(xmax<xmin) std::swap(xmin,xmax);
    double xsolve=golden(function,xmin,.5*(xmin+xmax),xmax,solveMax,1e-5);
    rootX=xsolve;
    rootY=funcs.eval(function,xsolve);
    rootShow=false;
    minShow=true;
    repaint();
#else
    UNUSED(function);
    UNUSED(xmin);
    UNUSED(xmax);
    UNUSED(solveMax);
#endif
}

void Graph::
solveMax(const int function,double xmin,double xmax)
{
    solveMin(function,xmin,xmax,true);
}

void Graph::
redraw()
{
    rootShow=false;
    minShow=false;
    repaint();
}
