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
* @file CEGraphUI.h
* @brief Contains the declaration of class CEGraphUI.
*/

#ifndef CEGraphUI_h
#define CEGraphUI_h

#include <QtOpenGL/QGLWidget>

#include <GL/gl.h>
#include <vector>
//qt3 #include <qgl.h>
#include "CEGraphView.h"

class CETool;
class CEGraphCurve;
class CEGraphSeg;
class CEGraphKey;
class CEDragHandler;

class QFont;
class QColor;
class QEvent;
class QResizeEvent;
class QMouseEvent;


//****************************************************************************
/**
 * @class CEGraphUI
 * @brief Insert one-line brief description of class CEGraphUI here.
 *
 * Insert detailed description of class CEGraphUI definition here.
 *
 * @author  brentb
 *
 * @version <B>1.0 brentb 2001.11.20:</B> Initial version of class CEGraphUI.
 * @version <b>2.0 bsilva 2001.10.11:</b> Ported to Qt4.
 *
 */  

class CEGraphUI :
    public QGLWidget
{   
    Q_OBJECT

public:
    /// Constructor
    CEGraphUI(QWidget* parent, CETool* tool);
    /// Destructor
    virtual ~CEGraphUI();

    // view position in (x:frames, y:curve units)
    const CEGraphView& getView();

    CETool* tool() { return _tool; }
    void moveSelBox(int x1, int y1, int x2, int y2);
    void hideSelBox();
    enum ColorType {
        BgColor, ScaleBgColor, ScaleColor, GridColor, TimebarColor,
        OverlayColor, SelectedColor, HiSelectedColor, 
        CurveColor, HiCurveColor, NumColors };
    void setColor(ColorType color);
    const QColor& getColor(ColorType color);
    void getActivePart(int& curve, int& seg, int& part)
      { curve = _activeCurve; seg = _activeSeg; part = _activePart; }
    CEGraphCurve* getCurve(int curve);
    CEGraphSeg* getSeg(int curve, int seg);
    CEGraphKey* getKey(int curve, int seg);
    int doSelectModeRender(int x1, int x2, int y1, int y2,
			   GLuint* buffer, int bufsize);

    virtual QSize minimumSizeHint() const { return QSize(200, 200); }

public slots:

    /// Notification methods (from CETool)
    void invalidateCurve(int index);
    void invalidateCurveList();
    void invalidateSelection();
    void invalidateView();
    void invalidateTimeMode();

//    void show();

protected:
    /// overrides of QGLWidget methods
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void resizeOverlayGL(int w, int h);
    virtual void paintGL();
    virtual void paintOverlayGL();

    virtual void resizeEvent(QResizeEvent *e);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseDoubleClickEvent(QMouseEvent* e);
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent* e);
    virtual void enterEvent(QEvent* e);
    virtual void leaveEvent(QEvent* e);
    virtual void keyPressEvent(QKeyEvent *event);


    void printStringAt(double x, double y, const char* str);

private:
    /// No definition by design, so accidental copying is prevented.
    CEGraphUI ( const CEGraphUI& );     
    /// No definition by design, so accidental assignment is prevented.
    CEGraphUI& operator=( const CEGraphUI& );  

    void getNextIncrement(double min, int timeMode,
			  double& increment, int& numDecimalPlaces);
    void getNextTickIncrement(double increment, int timeMode,
			      double& tickIncrement);
    void paintScales();
    void paintCurves();
    void paintOverlay(bool erase=0);
    void paintCrossHairs();
    void paintSelBox();
    void paintTimeBar(double frame);

    void moveCrossHairs(int x, int y);
    void hideCrossHairs();

    void pickActive(int x, int y);

    void updateView();
    void updateCurves();
    void clearCurves();

    CETool* _tool;
    QColor _colors[NumColors];
    
    QFont _font;
    int _fontListBase;
    int _fontListCount;

    // size of scales area (bottom and left of widget), based on font
    int _scalesWidth;
    int _scalesHeight;

    // current view
    CEGraphView _view; 
    bool _viewValid;

    // crosshair data
    bool _showCrossHairs;
    int _crossX, _crossY;

    // selection box data
    bool _showSelBox;
    int _selBoxX1, _selBoxY1, _selBoxX2, _selBoxY2;

    // curve list data
    bool _curveListValid;
    std::vector<CEGraphCurve*> _curves;

    // active object data
    int _activeCurve;
    int _activeSeg;
    int _activePart;

    // active drag handler
    CEDragHandler* _dragHandler;
};

#endif //CEGraphUI_h
