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
* @file ExprGrapher2d.cpp
* @brief A 2d image graph view for expression editing previewing
* @author  jlacewel
*/

#include <QDoubleValidator>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

#include "ExprGrapher2d.h"
#include "../utils/concurrent-for.h"

ExprGrapherWidget::ExprGrapherWidget(QWidget* parent, int width, int height)
    : expr("", SeExpr2::ExprType().FP(1)), view(new ExprGrapherView(*this, this, width, height)), _pixelLabel(nullptr)
{
    static ExprGrapherSymbols symbols;
    expr.setVarBlockCreator(&symbols);

    Q_UNUSED(parent);
    setMinimumSize(width, height + 30);
    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->setMargin(0);
    vbox->setMargin(2);
    setLayout(vbox);
    vbox->addWidget(view, 0, Qt::AlignCenter | Qt::AlignTop);
    QHBoxLayout* hbox = new QHBoxLayout;
    vbox->addLayout(hbox);
    hbox->setMargin(0);

    float xmin, xmax, ymin, ymax, z;
    view->getWindow(xmin, xmax, ymin, ymax, z);
    scale = new QLineEdit();
    scale->setToolTip("sets the preview scale");
    QDoubleValidator* valValidator = new QDoubleValidator(0.0, 10000000.0, 6, scale);
    scale->setValidator(valValidator);
    scaleValueManipulated();

    connect(scale, SIGNAL(returnPressed()), this, SLOT(scaleValueEdited()));
    connect(view, SIGNAL(scaleValueManipulated()), this, SLOT(scaleValueManipulated()));
    connect(view, SIGNAL(clicked()), this, SLOT(forwardPreview()));
    connect(view, SIGNAL(pixelHovered(int, int)), this, SLOT(updatePixelLabel(int, int)));

    hbox->addWidget(new QLabel("scale"), 0);
    hbox->addWidget(scale, 0);

    hbox->addStretch(1);

    _pixelLabel = new QLabel();
    _pixelLabel->show();
    hbox->addWidget(_pixelLabel, 0);
    updatePixelLabel(0, 0);
}

void ExprGrapherWidget::scaleValueEdited()
{
    float xmin, xmax, ymin, ymax, z;
    view->getWindow(xmin, xmax, ymin, ymax, z);
    float xdiff = xmax - xmin, ydiff = ymax - ymin;
    float xcenter = .5 * (xmax + xmin), ycenter = .5 * (ymin + ymax);
    float newScale = atof(scale->text().toStdString().c_str());

    float aspect = ydiff / xdiff;

    xmin = xcenter - newScale;
    xmax = xcenter + newScale;
    ymin = ycenter - aspect * newScale;
    ymax = ycenter + aspect * newScale;
    view->setWindow(xmin, xmax, ymin, ymax, z);
}

void ExprGrapherWidget::scaleValueManipulated()
{
    float xmin, xmax, ymin, ymax, z;
    view->getWindow(xmin, xmax, ymin, ymax, z);
    scale->setText(QString("%1").arg(.5 * (xmax - xmin)));
}

void ExprGrapherWidget::updatePixelLabel(int x, int y)
{
    float* pixel = view->pixel(x, y);
    QString rgbstr = QString("[ %1, %2, %3 ]")
                         .arg(QString::number(pixel[0], 'f', 3), QString::number(pixel[1], 'f', 3),
                              QString::number(pixel[2], 'f', 3));
    _pixelLabel->setText(rgbstr);
}

void ExprGrapherWidget::update()
{
    view->update();
}

void ExprGrapherWidget::forwardPreview()
{
    emit preview();
}

ExprGrapherView::ExprGrapherView(ExprGrapherWidget& widget, QWidget* parent, int width, int height)
    : QGLWidget(parent)
    , widget(widget)
    , _image(NULL)
    , _width(width)
    , _height(height)
    , scaling(false)
    , translating(false)
{
    this->setMinimumSize(width, height);

    _image = new float[3 * _width * _height];
    setWindow(-1, 1, -1, 1, 0);
    update();

    setCursor(Qt::OpenHandCursor);

    setMouseTracking(true);
}

ExprGrapherView::~ExprGrapherView()
{
    delete[] _image;
}

void ExprGrapherView::setWindow(float xmin, float xmax, float ymin, float ymax, float z)
{
    this->z = z;
    this->xmin = xmin;
    this->xmax = xmax;
    this->ymin = ymin;
    this->ymax = ymax;

    dx = (xmax - xmin) / _width;
    dy = (ymax - ymin) / _height;
}

void ExprGrapherView::getWindow(float& xmin, float& xmax, float& ymin, float& ymax, float& z)
{
    z = this->z;
    xmin = this->xmin;
    xmax = this->xmax;
    ymin = this->ymin;
    ymax = this->ymax;
}

void ExprGrapherView::clear()
{
    for (int row = 0; row < _height; ++row) {
        for (int col = 0; col < _width; ++col) {
            int index = 3 * row * _width + 3 * col;
            _image[index] = 1.0f;
            _image[index + 1] = 0.0f;
            _image[index + 2] = 0.0f;
        }
    }
}

void ExprGrapherView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MidButton) {
        setCursor(Qt::ClosedHandCursor);
        translating = true;
    }
    if (event->button() == Qt::RightButton) {
        setCursor(Qt::SizeAllCursor);
        scaling = true;
    }
    event_oldx = event->x();
    event_oldy = event->y();
}
void ExprGrapherView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        emit clicked();
    scaling = translating = false;
    setCursor(Qt::OpenHandCursor);
}
void ExprGrapherView::mouseMoveEvent(QMouseEvent* event)
{
    int x = event->x(), y = event->y();
    float offsetx = dx * (x - event_oldx);
    float offsety = -dy * (y - event_oldy);

    if (translating) {
        xmin -= offsetx;
        xmax -= offsetx;
        ymin -= offsety;
        ymax -= offsety;
        update();
        repaint();
    } else if (scaling) {
        float offset = (fabs(offsetx) > fabs(offsety)) ? offsetx : offsety;

        float width = .5 * (xmax - xmin), height = .5 * (ymax - ymin);
        float xcenter = .5 * (xmin + xmax), ycenter = .5 * (ymin + ymax);
        // Use float args for pow() to fix Windows compile error
        float scale_factor = pow(10.f, -offset / (xmax - xmin));
        width *= scale_factor;
        height *= scale_factor;
        setWindow(xcenter - width, xcenter + width, ycenter - height, ycenter + height, z);
        emit scaleValueManipulated();
        update();
        repaint();
    } else {
        // flip y because Qt places origin at top left, but we store image data with origin at bottom left
        y = _height - y;
        emit pixelHovered(x, y);
    }
    event_oldx = x;
    event_oldy = y;
}

void ExprGrapherView::update()
{
    if (!widget.exprValid()) {
        clear();
        updateGL();
        return;
    }

    float dv = 1.0f / _height;
    float du = 1.0f / _width;
    CONCURRENT_FOR(0, _height, 1, [&dv, &du, this](int row) {
        static ExprGrapherSymbols symbols;
        thread_local SeExpr2::SymbolTable symtab(symbols.create());

        float y = .5 * dy + ymin + dy * row;
        float v = .5 * dv + dv * row;

        float x = .5 * dx + xmin;
        float u = .5 * du;

        double v_ = (double)v;
        symtab.Pointer(symbols.v) = &v_;
        for (int col = 0; col < _width; col++, x += dy, u += du) {
            int index = (row * _width + col) * 3;

            double u_ = (double)u;
            SeExpr2::Vec3d P(x, y, z);
            symtab.Pointer(symbols.u) = &u_;
            symtab.Pointer(symbols.P) = &P[0];

            double value[3] = {0.0};
            widget.expr.evalFP(&value[0], 3, &symtab);

            _image[index] = value[0];
            _image[index + 1] = value[1];
            _image[index + 2] = value[2];
            index += 3;
        }
    });

    updateGL();
}

void ExprGrapherView::paintGL()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, (GLfloat)_width, 0.0, (GLfloat)_height, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDepthFunc(0);
    glClearColor(1, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glRasterPos2i(0, 0);
    glDrawPixels(_width, _height, GL_RGB, GL_FLOAT, _image);
}
