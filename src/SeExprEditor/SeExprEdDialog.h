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
* @file SeExprEdDialog.h
* @brief A basic editor/browser/previewer for expression editing
* @author  jlacewel
*/
#ifndef _MY_EXPR_EDITOR_H
#define _MY_EXPR_EDITOR_H

#include <QtCore/QObject>
#include <QtGui/QMessageBox>
#include <QtOpenGL/QGLWidget>
#include <QtGui/QHBoxLayout>
#include <QtGui/QSplitter>
#include <QtGui/QPalette>
#include <QtGui/QPushButton>
#include <QtGui/QFileDialog>
#include <QtGui/QSpacerItem>
#include <QtGui/QSizePolicy>

//#include <SeExpression.h>

#include <iostream>
#include <fstream>

#include "SeExprEditor.h"


class SeExprEdGrapherWidget;
class SeExprEdBrowser;
class QTabWidget;

class SeExprEdDialog:public QDialog
{
    Q_OBJECT

public:
    SeExprEditor* editor;
    SeExprEdBrowser* browser;
private:
    SeExprEdGrapherWidget* grapher;
    QLabel* previewCommentLabel;
    QPushButton* acceptButton;
    QPushButton* cancelButton;
    SeExprEdControlCollection* controls;

    QPushButton *applyButton,*previewButton,*saveButton,*saveAsButton;
    QPushButton *saveLocalButton,*clearButton;
    QLineEdit* helpFindBox;
    QTimer* showEditorTimer;
    QTextBrowser* helpBrowser;
    QTextCursor cursor;
    QString prevFind;
    int _currentEditorIdx;
public:
    SeExprEdDialog(QWidget* parent);

    std::string getExpressionString()
    {
        return editor->getExpr();
    }

    void setExpressionString(const std::string& str)
    {
        clearExpression();
        editor->setExpr(str, /*apply*/ true);
    }

    void show();
    int exec();

    // Show the Nth editor dialog
    void showEditor(int idx);

private:
    void setupHelp(QTabWidget* tab);

protected:
    void keyPressEvent(QKeyEvent* event);
    void findHelper(QTextDocument::FindFlags flags);
    void closeEvent(QCloseEvent* event);

signals:
    void preview();
    void expressionApplied();
    void dialogClosed();
private slots:
    void previewExpression();
    void verifiedApply();
    void verifiedAccept();
    void findNextInHelp();
    void findPrevInHelp();
    void _showEditor();
public slots:

    void applyExpression();

    void clearExpression();

    void reject();
};

#endif
