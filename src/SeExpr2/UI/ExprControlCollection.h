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
* @file ExprControlCollection.h
* @brief Manages/creates a bunch of ExprControls by using expression text
* @author  aselle
*/
#ifndef _ExprControlCollection_h
#define _ExprControlCollection_h
#include <vector>

#include <QTimer>
#include <QRegExp>
#include <QTextBrowser>
#include <QPlainTextEdit>
#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QSlider>
#include "ExprControl.h"

class QVBoxLayout;
class QRadioButton;
class EditableExpression;

/// This class is the UI for adding widgets
class ExprAddDialog : public QDialog {
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
    QLineEdit *swatchLookup;
    QLineEdit *deepWaterLookup;
    QRadioButton *rainbowPaletteBtn;
    QRadioButton *grayPaletteBtn;
    QColor color;
    QPushButton *colorWidget;
    QComboBox *stringTypeWidget;
    QLineEdit *stringDefaultWidget;
    QLineEdit *stringNameWidget;

    ExprAddDialog(int &count, QWidget *parent = 0);
    const char *initSwatch();
  private
slots:
    void colorChooseClicked();
};

/// Widget that holds and manages controls for an Expression
///   This is typically used by an ExprEditor or a ExprShortEdit widget
///   This widget also is responsible for finding all user local variables (for use in autocomplete)
class ExprControlCollection : public QWidget {
    Q_OBJECT;

    int _linkedId;
    int count;
    bool showAddButton;

    // holds a representation factored into the controls
    EditableExpression *editableExpression;

  public:
    ExprControlCollection(QWidget *parent = 0, bool showAddButton = true);
    ~ExprControlCollection();

  private:
    // TODO: put back
    std::vector<ExprControl *> _controls;
    QVBoxLayout *controlLayout;

  public:
    /// Request new text, given taking into account control id's new values
    void updateText(const int id, QString &text);
    /// Rebuild the controls given the new expressionText. Return any local variables found
    bool rebuildControls(const QString &expressionText, std::vector<QString> &variables);

    /// Number of controls
    int numControls() { return _controls.size(); }

    void showEditor(int idx);

    /// Anim curve callback
    static void setAnimCurveCallback(AnimCurveControl::AnimCurveCallback callback) {
        AnimCurveControl::setAnimCurveCallback(callback);
    }

  private
slots:
    /// When a user clicks "Add Widget" button
    void addControlDialog();
    /// Notification when by a control whenever it is edited
    void singleControlChanged(int id);
    /// Notification by a control that a new color link is desired
    void linkColorLink(int id);
    /// Notification by a control that a color is edited (when it is linked)
    void linkColorEdited(int id, QColor color);
signals:
    /// Notification that a specific control was changed
    void controlChanged(int id);
    /// Gives information about when a link color was changed
    void linkColorOutput(QColor color);
    /// Emitted to request that a new widget string should be added to the expression
    /// i.e. after "Add Widget" was used
    void insertString(const std::string &controlString);
  public
slots:
    /// Notification from outside that a linked color widget was changed
    /// and should be forwarded to any linked controls
    void linkColorInput(QColor color);
};

#endif
