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
* @file ExprDeepWater.h
*/
#pragma once

#include <vector>

#include <QObject>
#include <QGraphicsPolygonItem>
#include <QGraphicsView>
#include <QLineEdit>

#include <cmath>
#include <iostream>

#include "../Vec.h"

struct SeDeepWaterParams {
    SeDeepWaterParams() {}
    SeDeepWaterParams(int resolutionIn,
                      double tileSizeIn,
                      double lengthCutoffIn,
                      double amplitudeIn,
                      double windAngleIn,
                      double windSpeedIn,
                      double directionalFactorExponentIn,
                      double directionalReflectionDampingIn,
                      const SeExpr2::Vec3d &flowDirectionIn,
                      double sharpenIn,
                      double timeIn,
                      double filterWidthIn)
        : resolution(resolutionIn), tileSize(tileSizeIn), lengthCutoff(lengthCutoffIn), amplitude(amplitudeIn),
          windAngle(windAngleIn), windSpeed(windSpeedIn), directionalFactorExponent(directionalFactorExponentIn),
          directionalReflectionDamping(directionalReflectionDampingIn), flowDirection(flowDirectionIn),
          sharpen(sharpenIn), time(timeIn), filterWidth(filterWidthIn) {}

    int resolution;
    double tileSize;
    double lengthCutoff;
    double amplitude;
    double windAngle;
    double windSpeed;
    double directionalFactorExponent;
    double directionalReflectionDamping;
    SeExpr2::Vec3d flowDirection;
    double sharpen;
    double time;
    double filterWidth;
};

template <class T>
struct SeDeepWater {
    SeDeepWater() : gravity(9.8) {}
    virtual ~SeDeepWater() {}

    void setParams(const SeDeepWaterParams &paramsIn) {
        params = paramsIn;
        gridSize = 1 << params.resolution;
    }

    T sqr(T x) { return x * x; }

    inline static T kscale() {
        return 1 / 100.0;
    };

    T toIndex(const T x) { return x / kscale(); }

    T fromIndex(const T index) { return kscale() * index; }

    inline static T bottom_offset() {
        return -5;
    };

    T fromLog(const T x) { return std::log(x) - bottom_offset(); }

    T toLog(const T z) { return std::exp(z + bottom_offset()); }

    T powerLaw(const T x, const SeDeepWaterParams &params) {
        return params.amplitude * exp(-1 / sqr(x)) / pow(x, 4 + params.directionalFactorExponent);  // power law
    }

    T rescale(const T x) { return std::pow(x, .1); }

    void generateSpectrum() {
        const T L = params.windSpeed * params.windSpeed / gravity;
        const T coefficient = 2 * M_PI / params.tileSize;

        klowindex = toIndex(fromLog(L * coefficient));
        khighindex = toIndex(fromLog(L * coefficient * gridSize));

        int sample = 0;
        T k = 0;
        while (k < 3000000) {
            k = toLog(fromIndex(sample++));
            T e = 0;
            if (k != 0) {
                e = powerLaw(k, params);                      // power law
                e *= exp(-sqr(k / L * params.lengthCutoff));  // damps high frequency waves
                e = rescale(e);
            }
            energy.emplace_back(e);
        }

        T x = sqrt(2 / (4 + params.directionalFactorExponent));
        kmaxindex = toIndex(fromLog(x));
        T escale = .95 / rescale(powerLaw(x, params));  // power law
        for (size_t sample = 0; sample < energy.size(); sample++) {
            energy[sample] *= escale;
        }
    }

    //! Evaluates curve and returns full value
    T getValue(double param) const {
        if (energy.empty()) return 0;
        if (param < 0) param = 0;
        if (param > 1) param = 1;
        int index = param * energy.size() - 1;
        return energy[index];
    }

    T getKLow() {
        T klow = (T)klowindex / energy.size();
        return klow < 0 ? 0 : klow;
    }

    T getKHigh() {
        T khigh = (T)khighindex / energy.size();
        return khigh > 1 ? 1 : khigh;
    }

    bool inGrid() { return kmaxindex > klowindex && kmaxindex < khighindex; }

    T gravity;
    size_t gridSize;
    SeDeepWaterParams params;
    std::vector<T> energy;
    T klowindex;
    T khighindex;
    T kmaxindex;
};

/*
  This class overrides QGraphicsView so we can get resize events
*/
class DeepWaterGraphicsView : public QGraphicsView {
    Q_OBJECT
  public:
    DeepWaterGraphicsView() {
        setTransformationAnchor(QGraphicsView::NoAnchor);
        setResizeAnchor(QGraphicsView::NoAnchor);
    }
    ~DeepWaterGraphicsView() {}

    virtual void resizeEvent(QResizeEvent *event);

signals:
    void resizeSignal(int width, int height);
};

class DeepWaterLineEdit : public QLineEdit {
    Q_OBJECT

  public:
    DeepWaterLineEdit(QWidget *parent = 0) {}
    ~DeepWaterLineEdit() {}

signals:
    void focusOut();

  protected:
    virtual void focusOutEvent(QFocusEvent *e) {
        QLineEdit::focusOutEvent(e);
        emit(focusOut());
    }
};

/*
  This class overrides QGraphicsScene so we can handle mouse
  press, drag and keyboard events
*/
class DeepWaterScene : public QGraphicsScene {
    Q_OBJECT

    typedef SeDeepWater<double> T_CURVE;

  public:
    DeepWaterScene();
    ~DeepWaterScene();

    void setParams(const SeDeepWaterParams &paramsIn);

    void drawRect();
    void drawPoly();
    void drawGrid();

    void emitDeepWaterChanged();

    void rebuildDeepWater();

    friend class ExprDeepWater;

    SeDeepWaterParams params;

  private:
    T_CURVE *_curve;

  public
slots:
    void resolutionChanged(int val);
    void tileSizeChanged(double val);
    void lengthCutoffChanged(double val);
    void amplitudeChanged(double val);
    void windAngleChanged(double val);
    void windSpeedChanged(double val);
    void flowDirectionChanged(QString val);
    void directionalFactorExponentChanged(double val);
    void directionalReflectionDampingChanged(double val);
    void sharpenChanged(double val);
    void resize(const int width, const int height);

signals:
    void deepWaterChanged();

  private:
    int _width;
    int _height;
    QGraphicsPolygonItem *_curvePoly;
    QGraphicsRectItem *_baseRect;
    QGraphicsRectItem *_gridRect;
};

class ExprDeepWater : public QWidget {
    Q_OBJECT
  public:
    ExprDeepWater(QWidget *parent = 0);
    ~ExprDeepWater() {}

    void setParams(const SeDeepWaterParams &params);
    DeepWaterScene *_scene;

  public
slots:
    void resolutionChanged();
    void tileSizeChanged();
    void lengthCutoffChanged();
    void amplitudeChanged();
    void windAngleChanged();
    void windSpeedChanged();
    void flowDirectionChanged();
    void directionalFactorExponentChanged();
    void directionalReflectionDampingChanged();
    void sharpenChanged();

signals:
    void resolutionChangedSignal(int val);
    void tileSizeChangedSignal(double val);
    void lengthCutoffChangedSignal(double val);
    void amplitudeChangedSignal(double val);
    void windAngleChangedSignal(double val);
    void windSpeedChangedSignal(double val);
    void flowDirectionChangedSignal(QString val);
    void directionalFactorExponentChangedSignal(double val);
    void directionalReflectionDampingChangedSignal(double val);
    void sharpenChangedSignal(double val);

  private:
    DeepWaterLineEdit *_resolutionEdit;
    DeepWaterLineEdit *_tileSizeEdit;
    DeepWaterLineEdit *_lengthCutoffEdit;
    DeepWaterLineEdit *_amplitudeEdit;
    DeepWaterLineEdit *_windAngleEdit;
    DeepWaterLineEdit *_windSpeedEdit;
    DeepWaterLineEdit *_flowDirectionEdit;
    DeepWaterLineEdit *_directionalFactorExponentEdit;
    DeepWaterLineEdit *_directionalReflectionDampingEdit;
    DeepWaterLineEdit *_sharpenEdit;
};
