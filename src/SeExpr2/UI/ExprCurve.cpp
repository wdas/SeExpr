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
* @file ExprCurve.cpp
* @brief Contains PyQt4 Ramp Widget to emulate Maya's ramp widget
* @author Arthur Shek
* @version ashek     05/04/09  Initial Version
*/
#include <iostream>
#include <algorithm>

#include <QDialog>
#include <QDoubleValidator>
#include <QGraphicsSceneMouseEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QResizeEvent>
#include <QDialogButtonBox>
#include <QMenu>

#include <SeExpr2/ExprBuiltins.h>
#include <cfloat>

#include "ExprCurve.h"

void CurveScene::removeAll()
{
    _cvs.clear();
}

void CurveGraphicsView::resizeEvent(QResizeEvent* event)
{
    emit resizeSignal(event->size().width(), event->size().height());
}

CurveScene::CurveScene()
    : _curve(new T_CURVE)
    , _width(320)
    , _height(170)
    , _interp(T_CURVE::kMonotoneSpline)
    , _selectedItem(-1)
    , _curvePoly(0)
    , _baseRect(0)
    , _lmb(false)
{
    rebuildCurve();
    resize(_width, _height);
}

CurveScene::~CurveScene()
{
    delete _curve;
}

void CurveScene::resize(const int width, const int height)
{
    // width and height already have the 8 px padding factored in
    _width = width - 16;
    _height = height - 16;
    setSceneRect(-9, -7, width, height);
    drawRect();
    drawPoly();
    drawPoints();
}

void CurveScene::rebuildCurve()
{
    delete _curve;
    _curve = new T_CURVE;
    for (unsigned int i = 0; i < _cvs.size(); i++)
        _curve->addPoint(_cvs[i]._pos, _cvs[i]._val, _cvs[i]._interp);
    _curve->preparePoints();
}

void CurveScene::addPoint(double x, double y, const T_INTERP interp, const bool select)
{
    x = SeExpr2::clamp(x, 0, 1);
    y = SeExpr2::clamp(y, 0, 1);

    _cvs.push_back(T_CURVE::CV(x, y, T_INTERP(interp)));
    int newIndex = _cvs.size() - 1;

    rebuildCurve();

    if (select)
        _selectedItem = newIndex;
    drawPoly();
    drawPoints();
}

void CurveScene::removePoint(const int index)
{
    _cvs.erase(_cvs.begin() + index);
    _selectedItem = -1;
    rebuildCurve();

    drawPoly();
    drawPoints();
    emitCurveChanged();
}

void CurveScene::keyPressEvent(QKeyEvent* event)
{
    if (((event->key() == Qt::Key_Backspace) || (event->key() == Qt::Key_Delete)) && (_selectedItem >= 0)) {
        // user hit delete with cv selected
        removePoint(_selectedItem);
    }
}

void CurveScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    _lmb = true;
    QPointF pos = mouseEvent->scenePos();
    // get items under mouse click
    QList<QGraphicsItem*> itemList = items(pos);
    if (itemList.empty()) {
        _selectedItem = -1;
        emit cvSelected(-1, -1, _interp);
        drawPoints();
    } else if (itemList[0]->zValue() == 2) {
        // getting here means we've selected a current point
        const int numCircle = _circleObjects.size();
        for (int i = 0; i < numCircle; i++) {
            QGraphicsItem* obj = _circleObjects[i];
            if (obj == itemList[0]) {
                _selectedItem = i;
                _interp = _cvs[i]._interp;
                emit cvSelected(_cvs[i]._pos, _cvs[i]._val, _cvs[i]._interp);
            }
        }
        drawPoints();
    } else {
        if (mouseEvent->buttons() == Qt::LeftButton) {
            // getting here means we want to create a new point
            double myx = pos.x() / _width;
            T_INTERP interpFromNearby = _curve->getLowerBoundCV(SeExpr2::clamp(myx, 0, 1))._interp;
            if (interpFromNearby == T_CURVE::kNone)
                interpFromNearby = T_CURVE::kMonotoneSpline;
            addPoint(myx, pos.y() / _height, interpFromNearby);
            emitCurveChanged();
        } else {
            _selectedItem = -1;
            drawPoints();
        }
    }
}

void CurveScene::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    if (_selectedItem >= 0) {
        QMenu* menu = new QMenu(event->widget());
        QAction* deleteAction = menu->addAction("Delete Point");
        // menu->addAction("Cancel");
        QAction* action = menu->exec(event->screenPos());
        if (action == deleteAction)
            removePoint(_selectedItem);
    }
}

void CurveScene::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if (_lmb) {
        QPointF point = mouseEvent->scenePos();
        if (_selectedItem >= 0) {
            // clamp motion to inside curve area
            double pos = SeExpr2::clamp(point.x() / _width, 0, 1);
            double val = SeExpr2::clamp(point.y() / _height, 0, 1);
            _cvs[_selectedItem]._pos = pos;
            _cvs[_selectedItem]._val = val;
            rebuildCurve();
            emit cvSelected(pos, val, _cvs[_selectedItem]._interp);
            drawPoly();
            drawPoints();
            emitCurveChanged();
        }
    }
}

void CurveScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    Q_UNUSED(mouseEvent);
    _lmb = false;
}

// user selected a different interpolation type, redraw
void CurveScene::interpChanged(const int interp)
{
    _interp = T_INTERP(interp);
    if (_selectedItem >= 0) {
        _cvs[_selectedItem]._interp = _interp;
        rebuildCurve();
        drawPoly();
        emitCurveChanged();
    }
}

// user entered a different point position, redraw
void CurveScene::selPosChanged(double posInput)
{
    if (_selectedItem >= 0) {
        double pos = SeExpr2::clamp(posInput, 0, 1);
        _cvs[_selectedItem]._pos = pos;
        rebuildCurve();
        drawPoly();
        drawPoints();
        emitCurveChanged();
    }
}

// user entered a different point value, redraw
void CurveScene::selValChanged(double val)
{
    if (_selectedItem >= 0) {
        val = SeExpr2::clamp(val, 0, 1);
        _cvs[_selectedItem]._val = val;
        rebuildCurve();
        drawPoly();
        drawPoints();
        emitCurveChanged();
    }
}

// return points in reverse order in order to use same parsing in editor
void CurveScene::emitCurveChanged()
{
    emit curveChanged();
}

// draws the base gray outline rectangle
void CurveScene::drawRect()
{
    if (_baseRect == 0) {
        _baseRect = addRect(0, 0, _width, _height, QPen(Qt::black, 1.0), QBrush(Qt::gray));
    }
    _baseRect->setRect(0, 0, _width, _height);
    _baseRect->setZValue(0);
}

// draws the poly curve representation
void CurveScene::drawPoly()
{
    if (_curvePoly == 0) {
        _curvePoly = addPolygon(QPolygonF(), QPen(Qt::black, 1.0), QBrush(Qt::darkGray));
    }

    QPolygonF poly;
    poly.append(QPointF(_width, 0));
    poly.append(QPointF(0, 0));
    for (int i = 0; i < 1000; i++) {
        double x = i / 1000.0;
        poly.append(QPointF(_width * x, _height * _curve->getValue(x)));
    }
    poly.append(QPointF(_width, 0));
    _curvePoly->setPolygon(poly);
    _curvePoly->setZValue(1);
}

// draws the cv points
void CurveScene::drawPoints()
{
    while (_circleObjects.size()) {
        delete _circleObjects[0];
        _circleObjects.erase(_circleObjects.begin());
    }
    const int numCV = _cvs.size();
    for (int i = 0; i < numCV; i++) {
        const T_CURVE::CV& pt = _cvs[i];
        QPen pen;
        if (i == _selectedItem) {
            pen = QPen(Qt::white, 1.0);
        } else {
            pen = QPen(Qt::black, 1.0);
        }
        _circleObjects.push_back(addEllipse(pt._pos * _width - 4, pt._val * _height - 4, 8, 8, pen, QBrush()));
        QGraphicsEllipseItem* circle = _circleObjects.back();
        circle->setFlag(QGraphicsItem::ItemIsMovable, true);
        circle->setZValue(2);
    }
}

ExprCurve::ExprCurve(QWidget* parent, QString pLabel, QString vLabel, QString iLabel, bool expandable)
    : QWidget(parent), _scene(0), _selPosEdit(0), _selValEdit(0), _interpComboBox(0)
{
    Q_UNUSED(iLabel);
    QHBoxLayout* mainLayout = new QHBoxLayout();
    mainLayout->setSpacing(2);
    mainLayout->setMargin(4);

    QWidget* edits = new QWidget;
    QVBoxLayout* editsLayout = new QVBoxLayout;
    editsLayout->setAlignment(Qt::AlignTop);
    editsLayout->setSpacing(0);
    editsLayout->setMargin(0);
    edits->setLayout(editsLayout);

    QWidget* selPos = new QWidget;
    QHBoxLayout* selPosLayout = new QHBoxLayout;
    selPosLayout->setSpacing(1);
    selPosLayout->setMargin(1);
    selPos->setLayout(selPosLayout);
    _selPosEdit = new QLineEdit;
    QDoubleValidator* posValidator = new QDoubleValidator(0.0, 1.0, 6, _selPosEdit);
    _selPosEdit->setValidator(posValidator);
    int editwidth = QFontMetrics(font()).width("9.999") + 8;
    _selPosEdit->setFixedWidth(editwidth);
    _selPosEdit->setFixedHeight(20);
    selPosLayout->addStretch(50);
    QLabel* posLabel;
    if (pLabel.isEmpty()) {
        posLabel = new QLabel("Selected Position:  ");
    } else {
        posLabel = new QLabel(pLabel);
    }
    selPosLayout->addWidget(posLabel);
    selPosLayout->addWidget(_selPosEdit);

    QWidget* selVal = new QWidget;
    QBoxLayout* selValLayout = new QHBoxLayout;
    selValLayout->setSpacing(1);
    selValLayout->setMargin(1);
    selVal->setLayout(selValLayout);
    _selValEdit = new QLineEdit;
    QDoubleValidator* valValidator = new QDoubleValidator(0.0, 1.0, 6, _selValEdit);
    _selValEdit->setValidator(valValidator);
    _selValEdit->setFixedWidth(editwidth);
    _selValEdit->setFixedHeight(20);
    selValLayout->addStretch(50);
    QLabel* valLabel;
    if (vLabel.isEmpty()) {
        valLabel = new QLabel("Selected Value:  ");
    } else {
        valLabel = new QLabel(vLabel);
    }
    selValLayout->addWidget(valLabel);
    selValLayout->addWidget(_selValEdit);

    _interpComboBox = new QComboBox;
    _interpComboBox->addItem("None");
    _interpComboBox->addItem("Linear");
    _interpComboBox->addItem("Smooth");
    _interpComboBox->addItem("Spline");
    _interpComboBox->addItem("MSpline");
    _interpComboBox->setCurrentIndex(4);
    _interpComboBox->setFixedWidth(70);
    _interpComboBox->setFixedHeight(20);

    editsLayout->addWidget(selPos);
    editsLayout->addWidget(selVal);
    editsLayout->addWidget(_interpComboBox);

    QFrame* curveFrame = new QFrame;
    curveFrame->setFrameShape(QFrame::Panel);
    curveFrame->setFrameShadow(QFrame::Sunken);
    curveFrame->setLineWidth(1);
    QHBoxLayout* curveFrameLayout = new QHBoxLayout;
    curveFrameLayout->setMargin(0);
    CurveGraphicsView* curveView = new CurveGraphicsView;
    curveView->setFrameShape(QFrame::Panel);
    curveView->setFrameShadow(QFrame::Sunken);
    curveView->setLineWidth(1);
    curveView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    curveView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _scene = new CurveScene;
    curveView->setScene(_scene);
    curveView->setTransform(QTransform().scale(1, -1));
    curveView->setRenderHints(QPainter::Antialiasing);
    curveFrameLayout->addWidget(curveView);
    curveFrame->setLayout(curveFrameLayout);

    mainLayout->addWidget(edits);
    mainLayout->addWidget(curveFrame);
    if (expandable) {
        QPushButton* expandButton = new QPushButton(">");
        expandButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        expandButton->setFixedWidth(15);
        mainLayout->addWidget(expandButton);
        // open a the detail widget when clicked
        connect(expandButton, SIGNAL(clicked()), this, SLOT(openDetail()));
    }
    mainLayout->setStretchFactor(curveFrame, 100);
    setLayout(mainLayout);

    // SIGNALS

    // when a user selects a cv, update the fields on left
    connect(_scene, SIGNAL(cvSelected(double, double, T_INTERP)), this, SLOT(cvSelectedSlot(double, double, T_INTERP)));
    // when a user selects a different interp, the curve has to redraw
    connect(_interpComboBox, SIGNAL(activated(int)), _scene, SLOT(interpChanged(int)));
    // when a user types a different position, the curve has to redraw
    connect(_selPosEdit, SIGNAL(returnPressed()), this, SLOT(selPosChanged()));
    connect(this, SIGNAL(selPosChangedSignal(double)), _scene, SLOT(selPosChanged(double)));
    // when a user types a different value, the curve has to redraw
    connect(_selValEdit, SIGNAL(returnPressed()), this, SLOT(selValChanged()));
    connect(this, SIGNAL(selValChangedSignal(double)), _scene, SLOT(selValChanged(double)));
    // when the widget is resized, resize the curve widget
    connect(curveView, SIGNAL(resizeSignal(int, int)), _scene, SLOT(resize(int, int)));
}

// CV selected, update the user interface fields.
void ExprCurve::cvSelectedSlot(double pos, double val, T_INTERP interp)
{
    QString posStr;
    if (pos >= 0.0)
        posStr.setNum(pos, 'f', 3);
    _selPosEdit->setText(posStr);
    QString valStr;
    if (val >= 0.0)
        valStr.setNum(val, 'f', 3);
    _selValEdit->setText(valStr);
    _interpComboBox->setCurrentIndex(interp);
}

// User entered new position, round and send signal to redraw curve.
void ExprCurve::selPosChanged()
{
    double pos = QString(_selPosEdit->text()).toDouble();
    _selPosEdit->setText(QString("%1").arg(pos, 0, 'f', 3));
    emit selPosChangedSignal(pos);
}

// User entered new value, round and send signal to redraw curve.
void ExprCurve::selValChanged()
{
    double val = QString(_selValEdit->text()).toDouble();
    val = SeExpr2::clamp(val, 0, 1);
    _selValEdit->setText(QString("%1").arg(val, 0, 'f', 3));
    emit selValChangedSignal(val);
}

void ExprCurve::openDetail()
{
    QDialog* dialog = new QDialog();
    dialog->setMinimumWidth(1024);
    dialog->setMinimumHeight(400);
    ExprCurve* curve = new ExprCurve(0, "", "", "", false);

    // copy points into new data
    const std::vector<T_CURVE::CV>& data = _scene->_cvs;
    typedef std::vector<T_CURVE::CV>::const_iterator ITERATOR;
    for (ITERATOR i = data.begin(); i != data.end(); ++i)
        curve->addPoint(i->_pos, i->_val, i->_interp);

    QVBoxLayout* layout = new QVBoxLayout();
    dialog->setLayout(layout);
    layout->addWidget(curve);
    QDialogButtonBox* buttonbar = new QDialogButtonBox();
    buttonbar->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
    connect(buttonbar, SIGNAL(accepted()), dialog, SLOT(accept()));
    connect(buttonbar, SIGNAL(rejected()), dialog, SLOT(reject()));
    layout->addWidget(buttonbar);

    if (dialog->exec() == QDialog::Accepted) {
        // copy points back from child
        _scene->removeAll();
        const std::vector<T_CURVE::CV>& dataNew = curve->_scene->_cvs;
        typedef std::vector<T_CURVE::CV>::const_iterator ITERATOR;
        for (ITERATOR i = dataNew.begin(); i != dataNew.end(); ++i)
            addPoint(i->_pos, i->_val, i->_interp);
        _scene->emitCurveChanged();
    }
}

void ExprCurve::addPoint(const double x, const double y, T_INTERP interp, const bool select)
{
    _scene->addPoint(x, y, interp, select);
}
