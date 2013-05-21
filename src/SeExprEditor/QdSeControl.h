/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
*/
#ifndef _QdSeSlider_h_
#define _QdSeSlider_h_
#include <QtGui/QTextBrowser>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QDialog>
#include <QtCore/QTimer>
#include <QtCore/QRegExp>
#include <QtGui/QLineEdit>
#include <QtGui/QCheckBox>
#include <QtGui/QSlider>

#include "QdSeCurve.h"
#include "QdSeCCurve.h"

class QdSeEditor;
class QLabel;
class QdSeCCurve;
class QHBoxLayout;
class QdCSwatch;
class QdSeEditable;
class QdSeStringEditable;
class QdSeVectorEditable;
class QdSeNumberEditable;
class QdSeAnimCurveEditable;
template<class TVAL> struct QdSeGenericCurveEditable;
typedef QdSeGenericCurveEditable<SeVec3d> QdSeCCurveEditable;
typedef QdSeGenericCurveEditable<double> QdSeCurveEditable;

namespace animlib{
class AnimCurve;
}

/// Base class for all controls for SeExpressions
class QdSeControl:public QWidget
{
    Q_OBJECT;

protected:
    int _id;
    bool _updating; // whether to send events (i.e. masked when self editing)
    QHBoxLayout* hbox;
    QCheckBox* _colorLinkCB;
    QLabel* _label;

    QdSeEditable* _editable;

public:
    QdSeControl(int id,QdSeEditable* editable,bool showColorLink);
    virtual ~QdSeControl(){}

    /// Interface for getting the color (used for linked color picking)
    virtual QColor getColor(){return QColor();}
    /// Interface for setting the color (used for linked color picking)
    virtual void setColor(QColor color){};
    
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
class QdSeLineEdit : public QLineEdit
{
    Q_OBJECT
    public:
        QdSeLineEdit(int id, QWidget* parent);
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
class QdSeSlider : public QSlider
{
    Q_OBJECT
    public:
        QdSeSlider(QWidget* parent = 0) : QSlider(parent) {}
        QdSeSlider(Qt::Orientation orientation, QWidget* parent = 0)
            : QSlider(orientation, parent) {}
        virtual void mousePressEvent(QMouseEvent* e);
        virtual void mouseMoveEvent(QMouseEvent* e);
        virtual void paintEvent(QPaintEvent* e);
        virtual void leaveEvent(QEvent* event ) { update(); }
        virtual void enterEvent(QEvent* event ) { update(); }
        virtual void wheelEvent(QWheelEvent* e) { e->ignore(); }
};

/// Channel Slider (i.e. for colors)
class QdSeChannelSlider : public QWidget
{
    Q_OBJECT
 public:
    QdSeChannelSlider(int id, QWidget* parent);
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
class QdSeNumberControl:public QdSeControl
{
    Q_OBJECT

    /// Pointer to the number control model
    QdSeNumberEditable* _numberEditable;     
    /// Slider for the number
    QdSeSlider* _slider; 
    /// Text box for the number
    QdSeLineEdit* _edit;
public:
    QdSeNumberControl(int id,QdSeNumberEditable* number);
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
class QdSeVectorControl:public QdSeControl
{
    Q_OBJECT

    /// Number model
    QdSeVectorEditable* _numberEditable;
    /// All three line edit widgets (for each component)
    QdSeLineEdit* _edits[3];
    QdCSwatch* _swatch;;
    /// All three channel sliders (for each component)
    QdSeChannelSlider* _sliders[3];
public:
    QdSeVectorControl(int id,QdSeVectorEditable* number);

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
class QdSeStringControl:public QdSeControl
{
    Q_OBJECT

    /// model for the string control
    QdSeStringEditable* _stringEditable;
    /// Edit box for the string
    QLineEdit* _edit;
public:
    QdSeStringControl(int id,QdSeStringEditable* stringEditable);
private:
    void updateControl();
private slots:
    void textChanged(const QString& newText);
    void fileBrowse();
    void directoryBrowse();
};

/// Control for editing a normal curve ramp
class QdSeCurveControl:public QdSeControl
{
    Q_OBJECT
    
    /// curve model
    QdSeCurveEditable* _curveEditable;
    /// curve edit widget
    QdSeCurve* _curve;
public:
    QdSeCurveControl(int id,QdSeCurveEditable* stringEditable);
private slots:
    void curveChanged();
};

/// Control for editing a color ramp curve
class QdSeCCurveControl:public QdSeControl
{
    Q_OBJECT

    /// color curve model
    QdSeCCurveEditable* _curveEditable;
    /// color curve widget
    QdSeCCurve* _curve;
public:
    QdSeCCurveControl(int id,QdSeCCurveEditable* stringEditable);
    QColor getColor();
    void setColor(QColor color);
private slots:
    void curveChanged();
};

/// Anim curve control
class QdSeGraphPreview;
class QdSeAnimCurveControl:public QdSeControl
{
    Q_OBJECT;

    QdSeAnimCurveEditable* _editable;
    QdSeGraphPreview* _preview;
public:
    QdSeAnimCurveControl(int id,QdSeAnimCurveEditable* curveEditable);
    typedef void (*AnimCurveCallback)(const std::string&,animlib::AnimCurve& curve);
    static void setAnimCurveCallback(AnimCurveCallback callback);

public slots:
    void editGraphClicked();

private slots:
    void refreshClicked();

private:
    static AnimCurveCallback callback;
};

#endif
