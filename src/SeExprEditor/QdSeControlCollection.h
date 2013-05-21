/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
*
* @file QdSeControlCollection.h
* @brief Manages/creates a bunch of QdSeControls by using expression text
* @author  aselle
*/
#ifndef _QdSeControlCollection_
#define _QdSeControlCollection_
#include <vector>

#include <QtGui/QTextBrowser>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QDialog>
#include <QtCore/QTimer>
#include <QtCore/QRegExp>
#include <QtGui/QLineEdit>
#include <QtGui/QCheckBox>
#include <QtGui/QSlider>
#include "QdSeControl.h"

class QVBoxLayout;
class QdSeEditableExpression;

/// This class is the UI for adding widgets
class QdSeAddDialog:public QDialog
{
    Q_OBJECT;
public:
    QLineEdit *variableName;
    QTabWidget *tabWidget;
    QLineEdit *intDefault;
    QLineEdit *intMin;
    QLineEdit *intMax;
    QLineEdit *floatDefault;
    QLineEdit *floatMin;
    QLineEdit *floatMax;
    QLineEdit *vectorDefault0;
    QLineEdit *vectorDefault1;
    QLineEdit *vectorDefault2;
    QLineEdit *vectorMin;
    QLineEdit *vectorMax;
    QLineEdit *curveLookup;
    QLineEdit *colorCurveLookup;
    QLineEdit *animCurveLookup;
    QLineEdit *animCurveLink;
    QColor color;
    QPushButton* colorWidget;
    QComboBox* stringTypeWidget;
    QLineEdit* stringDefaultWidget;
    QLineEdit* stringNameWidget;

    QdSeAddDialog(int& count,QWidget* parent=0);
private slots:
    void colorChooseClicked();

};

/// Widget that holds and manages controls for a SeExpression
///   This is typically used by a QdSeEditor or a QdSeShortEdit widget
///   This widget also is responsible for finding all user local variables (for use in autocomplete)
class QdSeControlCollection:public QWidget
{
    Q_OBJECT;

    int _linkedId;
    int count;
    bool showAddButton;

    // holds a representation factored into the controls
    QdSeEditableExpression* editableExpression;
public:
    QdSeControlCollection(QWidget* parent=0,bool showAddButton=true);
    ~QdSeControlCollection();

private:
    // TODO: put back
    std::vector<QdSeControl*> _controls;
    QVBoxLayout* controlLayout;
public:
    /// Request new text, given taking into account control id's new values
    void updateText(const int id,QString& text);
    /// Rebuild the controls given the new expressionText. Return any local variables found
    bool rebuildControls(const QString& expressionText,std::vector<QString>& variables);

    /// Number of controls
    int numControls() { return _controls.size(); }

    void showEditor(int idx);

    /// Anim curve callback
    static void setAnimCurveCallback(QdSeAnimCurveControl::AnimCurveCallback callback){
        QdSeAnimCurveControl::setAnimCurveCallback(callback);
    }
    
private slots:
    /// When a user clicks "Add Widget" button
    void addControlDialog();
    /// Notification when by a control whenever it is edited
    void singleControlChanged(int id);
    /// Notification by a control that a new color link is desired
    void linkColorLink(int id);
    /// Notification by a control that a color is edited (when it is linked)
    void linkColorEdited(int id,QColor color);
signals:
    /// Notification that a specific control was changed
    void controlChanged(int id);
    /// Gives information about when a link color was changed
    void linkColorOutput(QColor color);
    /// Emitted to request that a new widget string should be added to the expression
    /// i.e. after "Add Widget" was used
    void insertString(const std::string& controlString);
public slots:
    /// Notification from outside that a linked color widget was changed
    /// and should be forwarded to any linked controls
    void linkColorInput(QColor color);
    
};

#endif


