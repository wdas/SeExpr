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
* @file SeExprEdColorCurve.h
* @brief Contains PyQt4 Ramp Widget to emulate Maya's ramp widget
* @author Arthur Shek
* @version ashek     05/04/09  Initial Version
*/
#ifndef _SeExprEdColorCurve_h_
#define _SeExprEdColorCurve_h_


#include <vector>

#include <QtCore/QObject>
#include <QtGui/QComboBox>
#include <QtGui/QGraphicsProxyWidget>
#include <QtGui/QGraphicsView>
#include <QtGui/QLineEdit>

#include <SeVec3d.h>

#include "SeExprEdCurve.h"

/*
  This class overrides QGraphicsScene so we can handle mouse
  press, drag and keyboard events
*/
class CCurveScene : public QGraphicsScene
{
    Q_OBJECT

    typedef SeExpr::SeCurve<SeVec3d> T_CURVE;
    typedef T_CURVE::InterpType T_INTERP ;

public:
    CCurveScene();
    ~CCurveScene();

    void addPoint(double x, const SeVec3d y, const T_INTERP interp, const bool select=true);

    void removePoint(const int index);
    void removeAll();

    virtual void keyPressEvent(QKeyEvent *event);

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    void drawRect();

    void drawPoints();

    QPixmap& getPixmap();
    void emitCurveChanged();

    void rebuildCurve();

    std::vector<T_CURVE::CV> _cvs; // unsorted cvs

    friend class SeExprEdColorCurve;
private:
    T_CURVE* _curve;
public slots:
    void interpChanged(const int interp);
    void selPosChanged(double pos);
    void selValChanged(const SeVec3d& val);
    void resize(const int width, const int height);

signals:
    void cvSelected(double x, const SeVec3d y, const T_INTERP interp);
    void curveChanged();

private:
    QByteArray getCPixmap();

    int _width;
    int _height;
    SeVec3d _color;
    T_INTERP _interp;
    std::vector<QGraphicsEllipseItem *> _circleObjects;
    int _selectedItem;
    QPixmap _pixmap;
    bool _pixmapDirty;
    QWidget *_baseRectW;
    QGraphicsProxyWidget *_baseRect;
    bool _lmb;
};


class SeExprEdCBoxWidget : public QWidget
{
    Q_OBJECT
public:
    SeExprEdCBoxWidget(CCurveScene* curveScene, QWidget* parent = 0) : QWidget(parent), _curveScene(curveScene) {}
    ~SeExprEdCBoxWidget() {}

protected:
    virtual void paintEvent(QPaintEvent* event);

private:
    CCurveScene* _curveScene;
};


class SeExprEdCSwatchFrame : public QFrame
{
    Q_OBJECT
public:
    SeExprEdCSwatchFrame(SeVec3d value, QWidget* parent = 0);
    ~SeExprEdCSwatchFrame() {}

    void setValue(const SeVec3d &value);
    SeVec3d getValue() const;

protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);

signals:
    void selValChangedSignal(SeVec3d value);
    void swatchChanged(QColor color);

private:
    SeVec3d _value;
    QColor _color;
};


class SeExprEdColorCurve : public QWidget
{
    Q_OBJECT

    typedef SeExpr::SeCurve<SeVec3d> T_CURVE;
    typedef T_CURVE::InterpType T_INTERP ;

public:
    SeExprEdColorCurve(QWidget* parent = 0, QString pLabel = "", QString vLabel = "", QString iLabel = "",
        bool expandable=true);
    ~SeExprEdColorCurve() {}

    // Convenience Functions
    void addPoint(const double x, const SeVec3d y, const T_INTERP interp, bool select=false);
    void setSwatchColor(QColor color);
    QColor getSwatchColor();

    CCurveScene *_scene;

public slots:
    void cvSelectedSlot(const double pos, const SeVec3d val, const T_INTERP interp);
    void selPosChanged();
    void openDetail();
private slots:
    void internalSwatchChanged(QColor color);

signals:
    void selPosChangedSignal(double pos);
    void selValChangedSignal(SeVec3d val);
    void swatchChanged(QColor color);


private:
    QLineEdit *_selPosEdit;
    SeExprEdCSwatchFrame *_selValEdit;
    QComboBox *_interpComboBox;
};
#endif

