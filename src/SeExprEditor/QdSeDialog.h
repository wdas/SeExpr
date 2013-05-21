/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
*
* @file QdSeDialog.h
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

#include "QdSeEditor.h"


class QdSeGrapherWidget;
class QdSeBrowser;
class QTabWidget;

class QdSeDialog:public QDialog
{
    Q_OBJECT

public:
    QdSeEditor* editor;
    QdSeBrowser* browser;
private:
    QdSeGrapherWidget* grapher;
    QLabel* previewCommentLabel;
    QPushButton* acceptButton;
    QPushButton* cancelButton;
    QdSeControlCollection* controls;

    QPushButton *applyButton,*previewButton,*saveButton,*saveAsButton;
    QPushButton *saveLocalButton,*clearButton;
    QLineEdit* helpFindBox;
    QTimer* showEditorTimer;
    QTextBrowser* helpBrowser;
    QTextCursor cursor;
    QString prevFind;
    int _currentEditorIdx;
public:
    QdSeDialog(QWidget* parent);

    std::string getExpressionString()
    {
        return editor->getExpr();
    }

    void setExpressionString(const std::string& str)
    {
        clearExpression();
        editor->setExpr(str, /*apply*/ true);
    }

    int exec();

    // Show the Nth editor dialog
    void showEditor(int idx);

private:
    void setupHelp(QTabWidget* tab);

protected:
    void keyPressEvent(QKeyEvent* event);
    void findHelper(QTextDocument::FindFlags flags);

signals:
    void expressionApplied();
private slots:
    void verifiedApply();
    void verifiedAccept();
    void findNextInHelp();
    void findPrevInHelp();
    void _showEditor();
public slots:

    void applyExpression();

    void clearExpression();

};

#endif
