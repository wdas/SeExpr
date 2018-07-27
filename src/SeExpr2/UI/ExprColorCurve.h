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
* @file ExprColorCurve.h
* @brief Contains PyQt4 Ramp Widget to emulate Maya's ramp widget
* @author Arthur Shek
* @version ashek     05/04/09  Initial Version
*/
#ifndef _ExprColorCurve_h_
#define _ExprColorCurve_h_

#include <vector>

#include <QObject>
#include <QComboBox>
#include <QGraphicsProxyWidget>
#include <QGraphicsView>
#include <QLineEdit>

#include "../Vec.h"
#include "ExprCurve.h"

/*
  This class overrides QGraphicsScene so we can handle mouse
  press, drag and keyboard events
*/
class CCurveScene : public QGraphicsScene {
    Q_OBJECT

    typedef SeExpr2::Curve<SeExpr2::Vec3d> T_CURVE;
    typedef T_CURVE::InterpType T_INTERP;

  public:
    CCurveScene();
    ~CCurveScene();

    void addPoint(double x, const SeExpr2::Vec3d y, const T_INTERP interp, const bool select = true);

    void removePoint(const int index);
    void removeAll();

    virtual void keyPressEvent(QKeyEvent *event);

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    void drawRect();

    void drawPoints();

    QPixmap &getPixmap();
    void emitCurveChanged();

    void rebuildCurve();

    std::vector<T_CURVE::CV> _cvs;  // unsorted cvs

    friend class ExprColorCurve;

  public slots:
    void interpChanged(const int interp);
    void selPosChanged(double pos);
    void selValChanged(const SeExpr2::Vec3d &val);
    void resize(const int width, const int height);

  signals:
    void cvSelected(double x, const SeExpr2::Vec3d y, const T_INTERP interp);
    void curveChanged();

  private:
    T_CURVE *_curve;

    QByteArray getCPixmap();

    int _width;
    int _height;
    SeExpr2::Vec3d _color;
    T_INTERP _interp;
    std::vector<QGraphicsEllipseItem *> _circleObjects;
    int _selectedItem;
    QPixmap _pixmap;
    bool _pixmapDirty;
    QWidget *_baseRectW;
    QGraphicsProxyWidget *_baseRect;
    bool _lmb;
};

class ExprCBoxWidget : public QWidget {
    Q_OBJECT
  public:
    ExprCBoxWidget(CCurveScene *curveScene, QWidget *parent = 0) : QWidget(parent), _curveScene(curveScene) {}
    ~ExprCBoxWidget() {}

  protected:
    virtual void paintEvent(QPaintEvent *event);

  private:
    CCurveScene *_curveScene;
};

class ExprCSwatchFrame : public QFrame {
    Q_OBJECT
  public:
    ExprCSwatchFrame(SeExpr2::Vec3d value, QWidget *parent = 0);
    ~ExprCSwatchFrame() {}

    void setValue(const SeExpr2::Vec3d &value);
    SeExpr2::Vec3d getValue() const;

  protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);

signals:
    void selValChangedSignal(SeExpr2::Vec3d value);
    void swatchChanged(QColor color);

  private:
    SeExpr2::Vec3d _value;
    QColor _color;
};

class ExprColorCurve : public QWidget {
    Q_OBJECT

    typedef SeExpr2::Curve<SeExpr2::Vec3d> T_CURVE;
    typedef T_CURVE::InterpType T_INTERP;

  public:
    ExprColorCurve(QWidget *parent = 0,
                   QString pLabel = "",
                   QString vLabel = "",
                   QString iLabel = "",
                   bool expandable = true);
    ~ExprColorCurve() {}

    // Convenience Functions
    void addPoint(const double x, const SeExpr2::Vec3d y, const T_INTERP interp, bool select = false);
    void setSwatchColor(QColor color);
    QColor getSwatchColor();

    CCurveScene *_scene;

  public slots:
    void cvSelectedSlot(const double pos, const SeExpr2::Vec3d val, const T_INTERP interp);
    void selPosChanged();
    void openDetail();

  signals:
    void selPosChangedSignal(double pos);
    void selValChangedSignal(SeExpr2::Vec3d val);
    void swatchChanged(QColor color);

  private slots:
    void internalSwatchChanged(QColor color);

  private:
    QLineEdit *_selPosEdit;
    ExprCSwatchFrame *_selValEdit;
    QComboBox *_interpComboBox;
};
#endif
