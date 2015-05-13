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
#ifndef _SeExprEdSlider_h_
#define _SeExprEdSlider_h_
#include <QtGui/QTextBrowser>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QDialog>
#include <QtCore/QTimer>
#include <QtCore/QRegExp>
#include <QtGui/QLineEdit>
#include <QtGui/QCheckBox>
#include <QtGui/QSlider>

#include "SeExprEdCurve.h"
#include "SeExprEdColorCurve.h"

class SeExprEditor;
class QLabel;
class SeExprEdColorCurve;
class QHBoxLayout;
class SeExprEdCSwatchFrame;
class SeExprEdEditable;
class SeExprEdStringEditable;
class SeExprEdVectorEditable;
class SeExprEdNumberEditable;
class SeExprEdAnimCurveEditable;
class SeExprEdColorSwatchEditable;
class SeExprEdColorSwatchWidget;
template<class TVAL> struct SeExprEdGenericCurveEditable;
typedef SeExprEdGenericCurveEditable<SeVec3d> SeExprEdColorCurveEditable;
typedef SeExprEdGenericCurveEditable<double> SeExprEdCurveEditable;

namespace animlib{
class AnimCurve;
}

/// Base class for all controls for SeExpressions
class SeExprEdControl:public QWidget
{
    Q_OBJECT;

protected:
    int _id;
    bool _updating; // whether to send events (i.e. masked when self editing)
    QHBoxLayout* hbox;
    QCheckBox* _colorLinkCB;
    QLabel* _label;

    SeExprEdEditable* _editable;

public:
    SeExprEdControl(int id,SeExprEdEditable* editable,bool showColorLink);
    virtual ~SeExprEdControl(){}

    /// Interface for getting the color (used for linked color picking)
    virtual QColor getColor(){return QColor();}
    /// Interface for setting the color (used for linked color picking)
    virtual void setColor(QColor color){Q_UNUSED(color)};
    
signals:
    // sends that the control has been changed to the control collection
    void controlChanged(int id);
    // sends the new color to the control collection
    void linkColorEdited(int id,QColor color); 
    // sends that a color link is desired to the control collection
    void linkColorLink(int id);
public slots:
    // receives that the link should be changed to the given state (0=off,1=on)
    void linkStateChange(int state);
public:
    // notifies this that the link should be disconnected
    void linkDisconnect(int newId);
};


/// clamp val to the specified range [minval,maxval]
template<class T,class T2,class T3> T clamp(const T val,const T2 minval,const T3 maxval)
{
    if(val<minval) return minval;
    else if(val>maxval) return maxval;
    return val;
}

/// Line Editor Widget(used for numbers)
// TODO: can this now be removed?
class SeExprEdLineEdit : public QLineEdit
{
    Q_OBJECT
    public:
        SeExprEdLineEdit(int id, QWidget* parent);
        virtual void setText(const QString& t)
        {
            if (_signaling) return;
            QLineEdit::setText(t);
        }

    signals:
        void textChanged(int id, const QString& text); 

        private slots:
            void textChangedCB(const QString& text);

    private:
        int _id;
        bool _signaling;
};

/// Generic Slider (used for int and float sliders)
class SeExprEdSlider : public QSlider
{
    Q_OBJECT
    public:
        SeExprEdSlider(QWidget* parent = 0) : QSlider(parent) {}
        SeExprEdSlider(Qt::Orientation orientation, QWidget* parent = 0)
            : QSlider(orientation, parent) {}
        virtual void mousePressEvent(QMouseEvent* e);
        virtual void mouseMoveEvent(QMouseEvent* e);
        virtual void paintEvent(QPaintEvent* e);
        virtual void leaveEvent(QEvent* event ) { Q_UNUSED(event); update(); }
        virtual void enterEvent(QEvent* event ) { Q_UNUSED(event); update(); }
        virtual void wheelEvent(QWheelEvent* e) { e->ignore(); }
};

/// Channel Slider (i.e. for colors)
class SeExprEdChannelSlider : public QWidget
{
    Q_OBJECT
 public:
    SeExprEdChannelSlider(int id, QWidget* parent);
    virtual void paintEvent(QPaintEvent* e);
    virtual void mousePressEvent(QMouseEvent* e);    
    virtual void mouseMoveEvent(QMouseEvent* e);   
    virtual void wheelEvent(QWheelEvent* e) { e->ignore(); } 
    float value() const { return _value; }
    void setDisplayColor( QColor c ) { _col = c; }

 public slots:    
    void setValue(float value);

 signals:
    void valueChanged(int id, float value);
 private:
    int _id;
    float _value;
    QColor _col;
};

/// Number slider for either float or int data
class SeExprEdNumberControl:public SeExprEdControl
{
    Q_OBJECT

    /// Pointer to the number control model
    SeExprEdNumberEditable* _numberEditable;     
    /// Slider for the number
    SeExprEdSlider* _slider; 
    /// Text box for the number
    SeExprEdLineEdit* _edit;
public:
    SeExprEdNumberControl(int id,SeExprEdNumberEditable* number);
private:
    /// Update the model with the value and notify the collection
    void setValue(float value);
    /// Update values in slider and textbox  given what the model contains
    void updateControl();
private slots:
    void sliderChanged(int val);
    void editChanged(int id,const QString& text);
};

/// A vector or color control (named vector because it edits a SeVec3d literal)
class SeExprEdVectorControl:public SeExprEdControl
{
    Q_OBJECT

    /// Number model
    SeExprEdVectorEditable* _numberEditable;
    /// All three line edit widgets (for each component)
    SeExprEdLineEdit* _edits[3];
    SeExprEdCSwatchFrame* _swatch;;
    /// All three channel sliders (for each component)
    SeExprEdChannelSlider* _sliders[3];
public:
    SeExprEdVectorControl(int id,SeExprEdVectorEditable* number);

    QColor getColor();
    void setColor(QColor color);
private:
    /// set the value in the model (in response to editing from controls)
    void setValue(int id,float value);
    /// update the individual slider and eidt box controls
    void updateControl();
private slots:
    void sliderChanged(int id,float val);
    void editChanged(int id,const QString& text);
    void swatchChanged(QColor color);
};

/// A control for editing strings, filenames, and directories
class SeExprEdStringControl:public SeExprEdControl
{
    Q_OBJECT

    /// model for the string control
    SeExprEdStringEditable* _stringEditable;
    /// Edit box for the string
    QLineEdit* _edit;
public:
    SeExprEdStringControl(int id,SeExprEdStringEditable* stringEditable);
private:
    void updateControl();
private slots:
    void textChanged(const QString& newText);
    void fileBrowse();
    void directoryBrowse();
};

/// Control for editing a normal curve ramp
class SeExprEdCurveControl:public SeExprEdControl
{
    Q_OBJECT
    
    /// curve model
    SeExprEdCurveEditable* _curveEditable;
    /// curve edit widget
    SeExprEdCurve* _curve;
public:
    SeExprEdCurveControl(int id,SeExprEdCurveEditable* stringEditable);
private slots:
    void curveChanged();
};

/// Control for editing a color ramp curve
class SeExprEdCCurveControl:public SeExprEdControl
{
    Q_OBJECT

    /// color curve model
    SeExprEdColorCurveEditable* _curveEditable;
    /// color curve widget
    SeExprEdColorCurve* _curve;
public:
    SeExprEdCCurveControl(int id,SeExprEdColorCurveEditable* stringEditable);
    QColor getColor();
    void setColor(QColor color);
private slots:
    void curveChanged();
};

/// Anim curve control
class SeExprEdGraphPreview;
class SeExprEdAnimCurveControl:public SeExprEdControl
{
    Q_OBJECT;

    SeExprEdAnimCurveEditable* _editable;
    SeExprEdGraphPreview* _preview;
public:
    SeExprEdAnimCurveControl(int id,SeExprEdAnimCurveEditable* curveEditable);
    typedef void (*AnimCurveCallback)(const std::string&,animlib::AnimCurve& curve);
    static void setAnimCurveCallback(AnimCurveCallback callback);

public slots:
    void editGraphClicked();

private slots:
    void refreshClicked();

private:
    static AnimCurveCallback callback;
};

/// A control for editing color swatches
class SeExprEdColorSwatchControl:public SeExprEdControl
{
    Q_OBJECT

    /// model for the color swatches control
    SeExprEdColorSwatchEditable* _swatchEditable;
    /// Edit box for the color swatches
    SeExprEdColorSwatchWidget *_swatch;
public:
    SeExprEdColorSwatchControl(int id,SeExprEdColorSwatchEditable* swatchEditable);
private slots:
    void buildSwatchWidget();
    void colorChanged(int index, SeVec3d value);
    void colorAdded(int index, SeVec3d value);
    void colorRemoved(int index);
private:
    bool _indexLabel;
};

#endif
