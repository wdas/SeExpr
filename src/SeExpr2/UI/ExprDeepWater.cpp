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
* @file ExprDeepWater.cpp
*/
#include <iostream>
#include <algorithm>

#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QIntValidator>

#include <SeExpr2/ExprBuiltins.h>
#include <cfloat>

#include "ExprDeepWater.h"

void DeepWaterGraphicsView::resizeEvent(QResizeEvent* event)
{
    emit resizeSignal(event->size().width(), event->size().height());
}

DeepWaterScene::DeepWaterScene()
    : _curve(new T_CURVE), _width(320), _height(170), _curvePoly(0), _baseRect(0), _gridRect(0)
{
    resize(_width, _height);
}

DeepWaterScene::~DeepWaterScene()
{
    delete _curve;
}

void DeepWaterScene::resize(const int width, const int height)
{
    // width and height already have the 8 px padding factored in
    _width = width - 16;
    _height = height - 16;
    setSceneRect(-9, -7, width, height);
    drawRect();
    drawPoly();
    drawGrid();
}

void DeepWaterScene::resolutionChanged(int val)
{
    params.resolution = val;
    setParams(params);
}

void DeepWaterScene::tileSizeChanged(double val)
{
    params.tileSize = val;
    setParams(params);
}

void DeepWaterScene::lengthCutoffChanged(double val)
{
    params.lengthCutoff = val;
    setParams(params);
}

void DeepWaterScene::amplitudeChanged(double val)
{
    params.amplitude = val;
    setParams(params);
}

void DeepWaterScene::windAngleChanged(double val)
{
    params.windAngle = val;
    setParams(params);
}

void DeepWaterScene::windSpeedChanged(double val)
{
    params.windSpeed = val;
    setParams(params);
}

void DeepWaterScene::flowDirectionChanged(QString val)
{
    QString flowDirection = val.remove(0, 1);
    flowDirection = flowDirection.remove(flowDirection.size() - 1, 1);
    QStringList components = flowDirection.split(",");
    params.flowDirection = SeExpr2::Vec3d(components[0].toDouble(), components[1].toDouble(), components[2].toDouble());
    setParams(params);
}

void DeepWaterScene::directionalFactorExponentChanged(double val)
{
    params.directionalFactorExponent = val;
    setParams(params);
}

void DeepWaterScene::directionalReflectionDampingChanged(double val)
{
    params.directionalReflectionDamping = val;
    setParams(params);
}

void DeepWaterScene::sharpenChanged(double val)
{
    params.sharpen = val;
    setParams(params);
}

void DeepWaterScene::setParams(const SeDeepWaterParams& paramsIn)
{
    params = paramsIn;
    rebuildDeepWater();
    drawPoly();
    drawGrid();
    emitDeepWaterChanged();
}

void DeepWaterScene::rebuildDeepWater()
{
    delete _curve;
    _curve = new T_CURVE;
    _curve->setParams(params);
    _curve->generateSpectrum();
}

// return points in reverse order in order to use same parsing in editor
void DeepWaterScene::emitDeepWaterChanged()
{
    emit deepWaterChanged();
}

// draws the base gray outline rectangle
void DeepWaterScene::drawRect()
{
    if (_baseRect == 0) {
        _baseRect = addRect(0, 0, _width, _height, QPen(Qt::black, 1.0), QBrush(Qt::gray));
    }
    _baseRect->setRect(0, 0, _width, _height);
    _baseRect->setZValue(0);
}

// draws the poly curve representation
void DeepWaterScene::drawPoly()
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

// draws the base gray outline rectangle
void DeepWaterScene::drawGrid()
{
    if (_gridRect == 0) {
        _gridRect = addRect(0, 0, _width, _height, QPen(Qt::black, 1.0), QBrush(Qt::gray));
    }
    _gridRect->setRect(_width * _curve->getKLow(), 0, _width * _curve->getKHigh() - _width * _curve->getKLow(),
                       _height);
    _gridRect->setBrush(QBrush(_curve->inGrid() ? Qt::green : Qt::cyan));
    _gridRect->setZValue(2);
    _gridRect->setOpacity(0.25);
}

ExprDeepWater::ExprDeepWater(QWidget* parent)
    : QWidget(parent)
    , _scene(0)
    , _resolutionEdit(0)
    , _tileSizeEdit(0)
    , _lengthCutoffEdit(0)
    , _amplitudeEdit(0)
    , _windAngleEdit(0)
    , _windSpeedEdit(0)
    , _flowDirectionEdit(0)
    , _directionalFactorExponentEdit(0)
    , _directionalReflectionDampingEdit(0)
    , _sharpenEdit(0)
{
    QHBoxLayout* mainLayout = new QHBoxLayout();
    mainLayout->setSpacing(2);
    mainLayout->setMargin(4);

    QWidget* edits = new QWidget;
    QVBoxLayout* editsLayout = new QVBoxLayout;
    editsLayout->setAlignment(Qt::AlignTop);
    editsLayout->setSpacing(0);
    editsLayout->setMargin(0);
    edits->setLayout(editsLayout);

    int editWidth = QFontMetrics(font()).width("[0,0,0]") + 8;

    QWidget* resolution = new QWidget;
    QHBoxLayout* resolutionLayout = new QHBoxLayout;
    resolutionLayout->setSpacing(1);
    resolutionLayout->setMargin(1);
    resolution->setLayout(resolutionLayout);
    _resolutionEdit = new DeepWaterLineEdit;
    _resolutionEdit->setFixedWidth(editWidth);
    QIntValidator* resolutionValidator = new QIntValidator(_resolutionEdit);
    resolutionValidator->setBottom(1);
    _resolutionEdit->setValidator(resolutionValidator);
    _resolutionEdit->setFixedHeight(20);
    resolutionLayout->addStretch(50);
    QLabel* resolutionLabel = new QLabel("R");
    resolutionLabel->setToolTip("Resolution");
    resolutionLayout->addWidget(resolutionLabel);
    resolutionLayout->addWidget(_resolutionEdit);

    QWidget* tileSize = new QWidget;
    QHBoxLayout* tileSizeLayout = new QHBoxLayout;
    tileSizeLayout->setSpacing(1);
    tileSizeLayout->setMargin(1);
    tileSize->setLayout(tileSizeLayout);
    _tileSizeEdit = new DeepWaterLineEdit;
    _tileSizeEdit->setFixedWidth(editWidth);
    _tileSizeEdit->setFixedHeight(20);
    tileSizeLayout->addStretch(50);
    QLabel* tileSizeLabel = new QLabel("TS");
    tileSizeLabel->setToolTip("Tile Size");
    tileSizeLayout->addWidget(tileSizeLabel);
    tileSizeLayout->addWidget(_tileSizeEdit);

    QWidget* lengthCutoff = new QWidget;
    QHBoxLayout* lengthCutoffLayout = new QHBoxLayout;
    lengthCutoffLayout->setSpacing(1);
    lengthCutoffLayout->setMargin(1);
    lengthCutoff->setLayout(lengthCutoffLayout);
    _lengthCutoffEdit = new DeepWaterLineEdit;
    _lengthCutoffEdit->setFixedWidth(editWidth);
    _lengthCutoffEdit->setFixedHeight(20);
    lengthCutoffLayout->addStretch(50);
    QLabel* lengthCutoffLabel = new QLabel("LC");
    lengthCutoffLabel->setToolTip("Length Cutoff");
    lengthCutoffLayout->addWidget(lengthCutoffLabel);
    lengthCutoffLayout->addWidget(_lengthCutoffEdit);

    QWidget* amplitude = new QWidget;
    QHBoxLayout* amplitudeLayout = new QHBoxLayout;
    amplitudeLayout->setSpacing(1);
    amplitudeLayout->setMargin(1);
    amplitude->setLayout(amplitudeLayout);
    _amplitudeEdit = new DeepWaterLineEdit;
    _amplitudeEdit->setFixedWidth(editWidth);
    _amplitudeEdit->setFixedHeight(20);
    amplitudeLayout->addStretch(50);
    QLabel* amplitudeLabel = new QLabel("A");
    amplitudeLabel->setToolTip("Amplitude");
    amplitudeLayout->addWidget(amplitudeLabel);
    amplitudeLayout->addWidget(_amplitudeEdit);

    QWidget* windAngle = new QWidget;
    QHBoxLayout* windAngleLayout = new QHBoxLayout;
    windAngleLayout->setSpacing(1);
    windAngleLayout->setMargin(1);
    windAngle->setLayout(windAngleLayout);
    _windAngleEdit = new DeepWaterLineEdit;
    _windAngleEdit->setFixedWidth(editWidth);
    _windAngleEdit->setFixedHeight(20);
    windAngleLayout->addStretch(50);
    QLabel* windAngleLabel = new QLabel("WA");
    windAngleLabel->setToolTip("Wind Angle");
    windAngleLayout->addWidget(windAngleLabel);
    windAngleLayout->addWidget(_windAngleEdit);

    QWidget* windSpeed = new QWidget;
    QHBoxLayout* windSpeedLayout = new QHBoxLayout;
    windSpeedLayout->setSpacing(1);
    windSpeedLayout->setMargin(1);
    windSpeed->setLayout(windSpeedLayout);
    _windSpeedEdit = new DeepWaterLineEdit;
    _windSpeedEdit->setFixedWidth(editWidth);
    _windSpeedEdit->setFixedHeight(20);
    windSpeedLayout->addStretch(50);
    QLabel* windSpeedLabel = new QLabel("WS");
    windSpeedLabel->setToolTip("Wind Speed");
    windSpeedLayout->addWidget(windSpeedLabel);
    windSpeedLayout->addWidget(_windSpeedEdit);

    QWidget* directionalFactorExponent = new QWidget;
    QHBoxLayout* directionalFactorExponentLayout = new QHBoxLayout;
    directionalFactorExponentLayout->setSpacing(1);
    directionalFactorExponentLayout->setMargin(1);
    directionalFactorExponent->setLayout(directionalFactorExponentLayout);
    _directionalFactorExponentEdit = new DeepWaterLineEdit;
    _directionalFactorExponentEdit->setFixedWidth(editWidth);
    _directionalFactorExponentEdit->setFixedHeight(20);
    directionalFactorExponentLayout->addStretch(50);
    QLabel* directionalFactorExponentLabel = new QLabel("DFE");
    directionalFactorExponentLabel->setToolTip("Directional Factor Exponent");
    directionalFactorExponentLayout->addWidget(directionalFactorExponentLabel);
    directionalFactorExponentLayout->addWidget(_directionalFactorExponentEdit);

    QWidget* directionalReflectionDamping = new QWidget;
    QHBoxLayout* directionalReflectionDampingLayout = new QHBoxLayout;
    directionalReflectionDampingLayout->setSpacing(1);
    directionalReflectionDampingLayout->setMargin(1);
    directionalReflectionDamping->setLayout(directionalReflectionDampingLayout);
    _directionalReflectionDampingEdit = new DeepWaterLineEdit;
    _directionalReflectionDampingEdit->setFixedWidth(editWidth);
    _directionalReflectionDampingEdit->setFixedHeight(20);
    directionalReflectionDampingLayout->addStretch(50);
    QLabel* directionalReflectionDampingLabel = new QLabel("DRD");
    directionalReflectionDampingLabel->setToolTip("Directional Reflection Damping");
    directionalReflectionDampingLayout->addWidget(directionalReflectionDampingLabel);
    directionalReflectionDampingLayout->addWidget(_directionalReflectionDampingEdit);

    QWidget* flowDirection = new QWidget;
    QHBoxLayout* flowDirectionLayout = new QHBoxLayout;
    flowDirectionLayout->setSpacing(1);
    flowDirectionLayout->setMargin(1);
    flowDirection->setLayout(flowDirectionLayout);
    _flowDirectionEdit = new DeepWaterLineEdit;
    _flowDirectionEdit->setFixedWidth(editWidth);
    _flowDirectionEdit->setFixedHeight(20);
    flowDirectionLayout->addStretch(50);
    QLabel* flowDirectionLabel = new QLabel("FD");
    flowDirectionLabel->setToolTip("Flow Direction");
    flowDirectionLayout->addWidget(flowDirectionLabel);
    flowDirectionLayout->addWidget(_flowDirectionEdit);

    QWidget* sharpen = new QWidget;
    QHBoxLayout* sharpenLayout = new QHBoxLayout;
    sharpenLayout->setSpacing(1);
    sharpenLayout->setMargin(1);
    sharpen->setLayout(sharpenLayout);
    _sharpenEdit = new DeepWaterLineEdit;
    _sharpenEdit->setFixedWidth(editWidth);
    _sharpenEdit->setFixedHeight(20);
    sharpenLayout->addStretch(50);
    QLabel* sharpenLabel = new QLabel("S");
    sharpenLabel->setToolTip("Sharpen");
    sharpenLayout->addWidget(sharpenLabel);
    sharpenLayout->addWidget(_sharpenEdit);

    QFrame* curveFrame = new QFrame;
    curveFrame->setFrameShape(QFrame::Panel);
    curveFrame->setFrameShadow(QFrame::Sunken);
    curveFrame->setLineWidth(1);
    QHBoxLayout* curveFrameLayout = new QHBoxLayout;
    curveFrameLayout->setMargin(0);
    DeepWaterGraphicsView* curveView = new DeepWaterGraphicsView;
    curveView->setFrameShape(QFrame::Panel);
    curveView->setFrameShadow(QFrame::Sunken);
    curveView->setLineWidth(1);
    curveView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    curveView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _scene = new DeepWaterScene;
    curveView->setScene(_scene);
    curveView->setTransform(QTransform().scale(1, -1));
    curveView->setRenderHints(QPainter::Antialiasing);
    curveFrameLayout->addWidget(curveView);
    curveFrame->setLayout(curveFrameLayout);

    editsLayout->addWidget(resolution);
    editsLayout->addWidget(tileSize);
    editsLayout->addWidget(lengthCutoff);
    editsLayout->addWidget(amplitude);
    editsLayout->addWidget(windSpeed);
    editsLayout->addWidget(directionalFactorExponent);
    QFrame* line = new QFrame();
    line->setToolTip("Parameters below this do not affect spectrum");
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    editsLayout->addWidget(line);
    editsLayout->addWidget(windAngle);
    editsLayout->addWidget(flowDirection);
    editsLayout->addWidget(directionalReflectionDamping);
    editsLayout->addWidget(sharpen);

    mainLayout->addWidget(edits);
    mainLayout->addWidget(curveFrame);
    mainLayout->setStretchFactor(curveFrame, 100);
    setLayout(mainLayout);

    // SIGNALS

    connect(_resolutionEdit, SIGNAL(returnPressed()), this, SLOT(resolutionChanged()));
    connect(_resolutionEdit, SIGNAL(focusOut()), this, SLOT(resolutionChanged()));
    connect(this, SIGNAL(resolutionChangedSignal(int)), _scene, SLOT(resolutionChanged(int)));
    connect(_tileSizeEdit, SIGNAL(returnPressed()), this, SLOT(tileSizeChanged()));
    connect(_tileSizeEdit, SIGNAL(focusOut()), this, SLOT(tileSizeChanged()));
    connect(this, SIGNAL(tileSizeChangedSignal(double)), _scene, SLOT(tileSizeChanged(double)));
    connect(_lengthCutoffEdit, SIGNAL(returnPressed()), this, SLOT(lengthCutoffChanged()));
    connect(_lengthCutoffEdit, SIGNAL(focusOut()), this, SLOT(lengthCutoffChanged()));
    connect(this, SIGNAL(lengthCutoffChangedSignal(double)), _scene, SLOT(lengthCutoffChanged(double)));
    connect(_amplitudeEdit, SIGNAL(returnPressed()), this, SLOT(amplitudeChanged()));
    connect(_amplitudeEdit, SIGNAL(focusOut()), this, SLOT(amplitudeChanged()));
    connect(this, SIGNAL(amplitudeChangedSignal(double)), _scene, SLOT(amplitudeChanged(double)));
    connect(_windAngleEdit, SIGNAL(returnPressed()), this, SLOT(windAngleChanged()));
    connect(_windAngleEdit, SIGNAL(focusOut()), this, SLOT(windAngleChanged()));
    connect(this, SIGNAL(windAngleChangedSignal(double)), _scene, SLOT(windAngleChanged(double)));
    connect(_windSpeedEdit, SIGNAL(returnPressed()), this, SLOT(windSpeedChanged()));
    connect(_windSpeedEdit, SIGNAL(focusOut()), this, SLOT(windSpeedChanged()));
    connect(this, SIGNAL(windSpeedChangedSignal(double)), _scene, SLOT(windSpeedChanged(double)));
    connect(_flowDirectionEdit, SIGNAL(returnPressed()), this, SLOT(flowDirectionChanged()));
    connect(_flowDirectionEdit, SIGNAL(focusOut()), this, SLOT(flowDirectionChanged()));
    connect(this, SIGNAL(flowDirectionChangedSignal(QString)), _scene, SLOT(flowDirectionChanged(QString)));
    connect(_directionalFactorExponentEdit, SIGNAL(returnPressed()), this, SLOT(directionalFactorExponentChanged()));
    connect(_directionalFactorExponentEdit, SIGNAL(focusOut()), this, SLOT(directionalFactorExponentChanged()));
    connect(this, SIGNAL(directionalFactorExponentChangedSignal(double)), _scene,
            SLOT(directionalFactorExponentChanged(double)));
    connect(_directionalReflectionDampingEdit, SIGNAL(returnPressed()), this,
            SLOT(directionalReflectionDampingChanged()));
    connect(_directionalReflectionDampingEdit, SIGNAL(focusOut()), this, SLOT(directionalReflectionDampingChanged()));
    connect(this, SIGNAL(directionalReflectionDampingChangedSignal(double)), _scene,
            SLOT(directionalReflectionDampingChanged(double)));
    connect(_sharpenEdit, SIGNAL(returnPressed()), this, SLOT(sharpenChanged()));
    connect(_sharpenEdit, SIGNAL(focusOut()), this, SLOT(sharpenChanged()));
    connect(this, SIGNAL(sharpenChangedSignal(double)), _scene, SLOT(sharpenChanged(double)));

    // when the widget is resized, resize the curve widget
    connect(curveView, SIGNAL(resizeSignal(int, int)), _scene, SLOT(resize(int, int)));
}

void ExprDeepWater::resolutionChanged()
{
    int val = QString(_resolutionEdit->text()).toInt();
    emit resolutionChangedSignal(val);
}

void ExprDeepWater::tileSizeChanged()
{
    double val = QString(_tileSizeEdit->text()).toDouble();
    emit tileSizeChangedSignal(val);
}

void ExprDeepWater::lengthCutoffChanged()
{
    double val = QString(_lengthCutoffEdit->text()).toDouble();
    emit lengthCutoffChangedSignal(val);
}

void ExprDeepWater::amplitudeChanged()
{
    double val = QString(_amplitudeEdit->text()).toDouble();
    emit amplitudeChangedSignal(val);
}

void ExprDeepWater::windAngleChanged()
{
    double val = QString(_windAngleEdit->text()).toDouble();
    emit windAngleChangedSignal(val);
}

void ExprDeepWater::windSpeedChanged()
{
    double val = QString(_windSpeedEdit->text()).toDouble();
    emit windSpeedChangedSignal(val);
}

void ExprDeepWater::flowDirectionChanged()
{
    emit flowDirectionChangedSignal(_flowDirectionEdit->text());
}

void ExprDeepWater::directionalFactorExponentChanged()
{
    double val = QString(_directionalFactorExponentEdit->text()).toDouble();
    emit directionalFactorExponentChangedSignal(val);
}

void ExprDeepWater::directionalReflectionDampingChanged()
{
    double val = QString(_directionalReflectionDampingEdit->text()).toDouble();
    emit directionalReflectionDampingChangedSignal(val);
}

void ExprDeepWater::sharpenChanged()
{
    double val = QString(_sharpenEdit->text()).toDouble();
    emit sharpenChangedSignal(val);
}

void ExprDeepWater::setParams(const SeDeepWaterParams& params)
{
    _scene->setParams(params);
    _resolutionEdit->setText(QString::number(params.resolution));
    _tileSizeEdit->setText(QString::number(params.tileSize));
    _lengthCutoffEdit->setText(QString::number(params.lengthCutoff));
    _amplitudeEdit->setText(QString::number(params.amplitude));
    _windAngleEdit->setText(QString::number(params.windAngle));
    _windSpeedEdit->setText(QString::number(params.windSpeed));
    QString flowDirection = "[";
    flowDirection += QString::number(params.flowDirection[0]) + "," + QString::number(params.flowDirection[1]) + "," +
                     QString::number(params.flowDirection[2]) + "]";
    _flowDirectionEdit->setText(flowDirection);
    _directionalFactorExponentEdit->setText(QString::number(params.directionalFactorExponent));
    _directionalReflectionDampingEdit->setText(QString::number(params.directionalReflectionDamping));
    _sharpenEdit->setText(QString::number(params.sharpen));
}
