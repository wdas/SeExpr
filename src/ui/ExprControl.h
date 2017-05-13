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
#ifndef _ExprControl_h_
#define _ExprControl_h_
#include <QTextBrowser>
#include <QPlainTextEdit>
#include <QDialog>
#include <QTimer>
#include <QRegExp>
#include <QLineEdit>
#include <QCheckBox>
#include <QSlider>

#include "ExprCurve.h"
#include "ExprColorCurve.h"
#include "ExprDeepWater.h"

class QLabel;
class ExprColorCurve;
class QHBoxLayout;
class ExprCSwatchFrame;
class Editable;
class StringEditable;
class VectorEditable;
class NumberEditable;
class AnimCurveEditable;
class ColorSwatchEditable;
class ExprColorSwatchWidget;
template <class TVAL>
struct GenericCurveEditable;
typedef GenericCurveEditable<SeExpr2::Vec3d> ColorCurveEditable;
typedef GenericCurveEditable<double> CurveEditable;
class DeepWaterEditable;

namespace animlib {
class AnimCurve;
}

/// Base class for all controls for Expressions
class ExprControl : public QWidget {
    Q_OBJECT;

  protected:
    int _id;
    bool _updating;  // whether to send events (i.e. masked when self editing)
    QHBoxLayout* hbox;
    QCheckBox* _colorLinkCB;
    QLabel* _label;

    Editable* _editable;

  public:
    ExprControl(int id, Editable* editable, bool showColorLink);
    virtual ~ExprControl() {}

    /// Interface for getting the color (used for linked color picking)
    virtual QColor getColor() { return QColor(); }
    /// Interface for setting the color (used for linked color picking)
    virtual void setColor(QColor color) {Q_UNUSED(color)};

signals:
    // sends that the control has been changed to the control collection
    void controlChanged(int id);
    // sends the new color to the control collection
    void linkColorEdited(int id, QColor color);
    // sends that a color link is desired to the control collection
    void linkColorLink(int id);
  public
slots:
    // receives that the link should be changed to the given state (0=off,1=on)
    void linkStateChange(int state);

  public:
    // notifies this that the link should be disconnected
    void linkDisconnect(int newId);
};

/// clamp val to the specified range [minval,maxval]
template <class T, class T2, class T3>
T clamp(const T val, const T2 minval, const T3 maxval) {
    if (val < minval)
        return minval;
    else if (val > maxval)
        return maxval;
    return val;
}

/// Line Editor Widget(used for numbers)
// TODO: can this now be removed?
class ExprLineEdit : public QLineEdit {
    Q_OBJECT
  public:
    ExprLineEdit(int id, QWidget* parent);
    virtual void setText(const QString& t) {
        if (_signaling) return;
        QLineEdit::setText(t);
    }

signals:
    void textChanged(int id, const QString& text);

  private
slots:
    void textChangedCB(const QString& text);

  private:
    int _id;
    bool _signaling;
};

/// Generic Slider (used for int and float sliders)
class ExprSlider : public QSlider {
    Q_OBJECT
  public:
    ExprSlider(QWidget* parent = 0) : QSlider(parent) {}
    ExprSlider(Qt::Orientation orientation, QWidget* parent = 0) : QSlider(orientation, parent) {}
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void paintEvent(QPaintEvent* e);
    virtual void leaveEvent(QEvent* event) {
        Q_UNUSED(event);
        update();
    }
    virtual void enterEvent(QEvent* event) {
        Q_UNUSED(event);
        update();
    }
    virtual void wheelEvent(QWheelEvent* e) { e->ignore(); }
};

/// Channel Slider (i.e. for colors)
class ExprChannelSlider : public QWidget {
    Q_OBJECT
  public:
    ExprChannelSlider(int id, QWidget* parent);
    virtual void paintEvent(QPaintEvent* e);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void wheelEvent(QWheelEvent* e) { e->ignore(); }
    float value() const { return _value; }
    void setDisplayColor(QColor c) { _col = c; }

  public
slots:
    void setValue(float value);

signals:
    void valueChanged(int id, float value);

  private:
    int _id;
    float _value;
    QColor _col;
};

/// Number slider for either float or int data
class NumberControl : public ExprControl {
    Q_OBJECT

    /// Pointer to the number control model
    NumberEditable* _numberEditable;
    /// Slider for the number
    ExprSlider* _slider;
    /// Text box for the number
    ExprLineEdit* _edit;

  public:
    NumberControl(int id, NumberEditable* number);

  private:
    /// Update the model with the value and notify the collection
    void setValue(float value);
    /// Update values in slider and textbox  given what the model contains
    void updateControl();
  private
slots:
    void sliderChanged(int val);
    void editChanged(int id, const QString& text);
};

/// A vector or color control (named vector because it edits a SeExpr2::Vec3d literal)
class VectorControl : public ExprControl {
    Q_OBJECT

    /// Number model
    VectorEditable* _numberEditable;
    /// All three line edit widgets (for each component)
    ExprLineEdit* _edits[3];
    ExprCSwatchFrame* _swatch;
    ;
    /// All three channel sliders (for each component)
    ExprChannelSlider* _sliders[3];

  public:
    VectorControl(int id, VectorEditable* number);

    QColor getColor();
    void setColor(QColor color);

  private:
    /// set the value in the model (in response to editing from controls)
    void setValue(int id, float value);
    /// update the individual slider and eidt box controls
    void updateControl();
  private
slots:
    void sliderChanged(int id, float val);
    void editChanged(int id, const QString& text);
    void swatchChanged(QColor color);
};

/// A control for editing strings, filenames, and directories
class StringControl : public ExprControl {
    Q_OBJECT

    /// model for the string control
    StringEditable* _stringEditable;
    /// Edit box for the string
    QLineEdit* _edit;

  public:
    StringControl(int id, StringEditable* stringEditable);

  private:
    void updateControl();
  private
slots:
    void textChanged(const QString& newText);
    void fileBrowse();
    void directoryBrowse();
};

/// Control for editing a normal curve ramp
class CurveControl : public ExprControl {
    Q_OBJECT

    /// curve model
    CurveEditable* _curveEditable;
    /// curve edit widget
    ExprCurve* _curve;

  public:
    CurveControl(int id, CurveEditable* stringEditable);
  private
slots:
    void curveChanged();
};

/// Control for editing a color ramp curve
class CCurveControl : public ExprControl {
    Q_OBJECT

    /// color curve model
    ColorCurveEditable* _curveEditable;
    /// color curve widget
    ExprColorCurve* _curve;

  public:
    CCurveControl(int id, ColorCurveEditable* stringEditable);
    QColor getColor();
    void setColor(QColor color);
  private
slots:
    void curveChanged();
};

/// Anim curve control
class ExprGraphPreview;
class AnimCurveControl : public ExprControl {
    Q_OBJECT;

    AnimCurveEditable* _editable;
    ExprGraphPreview* _preview;

  public:
    AnimCurveControl(int id, AnimCurveEditable* curveEditable);
    typedef void (*AnimCurveCallback)(const std::string&, animlib::AnimCurve& curve);
    static void setAnimCurveCallback(AnimCurveCallback callback);

  public
slots:
    void editGraphClicked();

  private
slots:
    void refreshClicked();

  private:
    static AnimCurveCallback callback;
};

/// A control for editing color swatches
class ColorSwatchControl : public ExprControl {
    Q_OBJECT

    /// model for the color swatches control
    ColorSwatchEditable* _swatchEditable;
    /// Edit box for the color swatches
    ExprColorSwatchWidget* _swatch;

  public:
    ColorSwatchControl(int id, ColorSwatchEditable* swatchEditable);
  private
slots:
    void buildSwatchWidget();
    void colorChanged(int index, SeExpr2::Vec3d value);
    void colorAdded(int index, SeExpr2::Vec3d value);
    void colorRemoved(int index);

  private:
    bool _indexLabel;
};

/// Control for displaying a deep water spectrum
class DeepWaterControl : public ExprControl {
    Q_OBJECT

    /// curve model
    DeepWaterEditable* _deepWaterEditable;
    /// deep water widget
    ExprDeepWater* _deepWater;

  public:
    DeepWaterControl(int id, DeepWaterEditable* stringEditable);
  private
slots:
    void deepWaterChanged();
};

#endif
