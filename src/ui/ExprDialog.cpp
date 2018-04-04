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
* @file ExprDialog.cpp
* @brief A basic editor/browser/previewer for expression editing
* @author  jlacewel
*/

#include "ExprBrowser.h"
#include "ExprGrapher2d.h"
#include "ExprDialog.h"
#include "ExprControlCollection.h"

#include <QDir>
#include <QApplication>
#include <QLabel>
#include <iostream>
#include <fstream>

#define P3D_CONFIG_ENVVAR "P3D_CONFIG_PATH"
static const char* arrow_left_xpm[] = {"16 16 9 1",        "g c #808080",      "b c #c0c000",      "e c #808080",
                                       "# c #000000",      "c c #ffff00",      ". c None",         "a c #585858",
                                       "f c #606060",      "d c #a0a0a0",      "................", "................",
                                       "......##........", ".....#d#........", "....#de#........", "...#dee#######..",
                                       "..#deeeeddddd#..", ".#deeeeeeeeef#..", ".#eeeeeeeeeef#..", "..#eeefffffff#..",
                                       "...#eef#######..", "....#ef#........", ".....#f#........", "......##........",
                                       "................", "................"};
static const char* arrow_right_xpm[] = {"16 16 9 1",        "g c #808080",      "b c #c0c000",      "e c #808080",
                                        "# c #000000",      "c c #ffff00",      ". c None",         "a c #585858",
                                        "f c #606060",      "d c #a0a0a0",      "................", "................",
                                        "........##......", "........#d#.....", "........#de#....", "..#######eee#...",
                                        "..#dddddeeeee#..", "..#deeeeeeeeee#.", "..#deeeeeeeeef#.", "..#dfffffeeef#..",
                                        "..#######def#...", "........#df#....", "........#d#.....", "........##......",
                                        "................", "................"};

ExprDialog::ExprDialog(QWidget* parent) : QDialog(parent), _currentEditorIdx(0), currhistitem(0)
{
    this->setMinimumWidth(600);
    QVBoxLayout* rootLayout = new QVBoxLayout(0);
    rootLayout->setMargin(2);
    rootLayout->setSpacing(2);
    this->setLayout(rootLayout);

    showEditorTimer = new QTimer();
    connect(showEditorTimer, SIGNAL(timeout()), SLOT(_showEditor()));

    QSplitter* vsplitter = new QSplitter(Qt::Vertical, this);
    rootLayout->addWidget(vsplitter);

    QTabWidget* topTabWidget = new QTabWidget();
    vsplitter->addWidget(topTabWidget);

    QWidget* previewLibraryWidget = new QWidget();
    QHBoxLayout* previewLibraryLayout = new QHBoxLayout();
    previewLibraryLayout->setSpacing(2);
    previewLibraryLayout->setMargin(2);
    previewLibraryWidget->setLayout(previewLibraryLayout);
    topTabWidget->addTab(previewLibraryWidget, "Preview / Library");

    QWidget* bottomWidget = new QWidget();
    vsplitter->addWidget(bottomWidget);
    QVBoxLayout* bottomLayout = new QVBoxLayout();
    bottomLayout->setMargin(1);
    bottomWidget->setLayout(bottomLayout);

    // setup preview
    QWidget* leftWidget = new QWidget();
    leftWidget->setFixedWidth(450);
    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->setMargin(0);
    leftLayout->setSpacing(2);
    leftWidget->setLayout(leftLayout);
    QHBoxLayout* previewLayout = new QHBoxLayout();
    grapher = new ExprGrapherWidget(this, 256, 256);
    previewLayout->addWidget(grapher, 0);
    previewCommentLabel = new QLabel();
    previewCommentLabel->setWordWrap(true);
    previewLayout->addWidget(previewCommentLabel, 1, Qt::AlignLeft | Qt::AlignTop);
    leftLayout->addLayout(previewLayout);
    previewLibraryLayout->addWidget(leftWidget);

    // setup button bar
    // QWidget* buttonBarWidget=new QWidget();
    QHBoxLayout* buttonBarLayout = new QHBoxLayout();
    // buttonBarWidget->setLayout(buttonBarLayout);
    buttonBarLayout->setMargin(1);
    previewButton = new QPushButton("Preview");
    buttonBarLayout->addWidget(previewButton);

    QPushButton* histBack = new QPushButton(this);
    buttonBarLayout->addWidget(histBack);
    histBack->setIcon(QPixmap(arrow_left_xpm));
    histBack->setToolTip("Previous In History");
    histBack->setEnabled(0);
    histBack->setFixedSize(24, 24);
    histBack->setFocusPolicy(Qt::NoFocus);
    QPushButton* histForw = new QPushButton(this);
    buttonBarLayout->addWidget(histForw);
    histForw->setIcon(QPixmap(arrow_right_xpm));
    histForw->setToolTip("Next In History");
    histForw->setEnabled(0);
    histForw->setFixedSize(24, 24);
    histForw->setFocusPolicy(Qt::NoFocus);
    history.push_back("");
    connect(this, SIGNAL(backwardAvailable(bool)), histBack, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(forwardAvailable(bool)), histForw, SLOT(setEnabled(bool)));
    QPushButton* reloadExprPb = new QPushButton("Reload");
    reloadExprPb->setFixedHeight(24);
    reloadExprPb->setToolTip("Reload current expression");
    buttonBarLayout->addWidget(reloadExprPb);
    connect(reloadExprPb, SIGNAL(clicked()), this, SLOT(reloadExpression()));

    saveButton = new QPushButton("Save");
    buttonBarLayout->addWidget(saveButton);
    saveAsButton = new QPushButton("Save As");
    buttonBarLayout->addWidget(saveAsButton);
    saveLocalButton = new QPushButton("Save Local");
    saveLocalButton->setEnabled(false);
    buttonBarLayout->addWidget(saveLocalButton);
    clearButton = new QPushButton("Clear");
    buttonBarLayout->addWidget(clearButton);
    bottomLayout->addLayout(buttonBarLayout);

    controls = new ExprControlCollection();

    // controls
    // leftLayout->addWidget(controls, 1);
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(controls);
    scrollArea->setFocusPolicy(Qt::NoFocus);
    scrollArea->setMinimumHeight(100);
    scrollArea->setFixedWidth(450);
    scrollArea->setWidgetResizable(true);
    leftLayout->addWidget(scrollArea, 1);

    // make button bar
    editor = new ExprEditor(this, controls);
    connect(editor, SIGNAL(apply()), SLOT(verifiedApply()));
    connect(editor, SIGNAL(preview()), SLOT(previewExpression()));
    connect(grapher, SIGNAL(preview()), SLOT(previewExpression()));
    bottomLayout->addWidget(editor);

    // make expression library browser
    browser = new ExprBrowser(0, editor);
    previewLibraryLayout->addWidget(browser);

    // dialog buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout(0);
    buttonLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    applyButton = new QPushButton("Apply");
    buttonLayout->addWidget(applyButton);
    acceptButton = new QPushButton("Accept");
    buttonLayout->addWidget(acceptButton);
    cancelButton = new QPushButton("Cancel");
    buttonLayout->addWidget(cancelButton);
    connect(applyButton, SIGNAL(clicked()), this, SLOT(verifiedApply()));
    connect(acceptButton, SIGNAL(clicked()), this, SLOT(verifiedAccept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    rootLayout->addLayout(buttonLayout);

    setupHelp(topTabWidget);

    // connect buttons
    connect(previewButton, SIGNAL(clicked()), SLOT(previewExpression()));
    connect(clearButton, SIGNAL(clicked()), SLOT(clearExpression()));
    connect(saveButton, SIGNAL(clicked()), browser, SLOT(saveExpression()));
    connect(saveAsButton, SIGNAL(clicked()), browser, SLOT(saveExpressionAs()));
    connect(saveLocalButton, SIGNAL(clicked()), browser, SLOT(saveLocalExpressionAs()));
    connect(histBack, SIGNAL(clicked()), SLOT(histBackward()));
    connect(histForw, SIGNAL(clicked()), SLOT(histForward()));
    connect(browser, SIGNAL(selectionChanged(const QString&)), SLOT(selectionChanged(const QString&)));
}

void ExprDialog::showEditor(int idx)
{
    _currentEditorIdx = idx;
    showEditorTimer->setSingleShot(true);
    showEditorTimer->start();
}

void ExprDialog::_showEditor()
{
    controls->showEditor(_currentEditorIdx);
}

void ExprDialog::show()
{
    // populate the expressions
    browser->getExpressionDirs();
    browser->expandAll();
    QDialog::show();
}

int ExprDialog::exec()
{
    // populate the expressions
    browser->getExpressionDirs();
    browser->expandAll();
    return QDialog::exec();
}

void ExprDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
        return;
    return QDialog::keyPressEvent(event);
}

void ExprDialog::closeEvent(QCloseEvent* event)
{
    emit dialogClosed();
    QDialog::closeEvent(event);
}

void ExprDialog::reject()
{
    emit dialogClosed();
    QDialog::reject();
}

void ExprDialog::verifiedApply()
{
    applyExpression();
    if (grapher->expr.isValid()) {
        emit expressionApplied();
    } else {
        QMessageBox msgBox;
        msgBox.setText("Your expression had possible errors.");
        msgBox.setInformativeText("Do you want to accept your expression anyways?");
        QPushButton* okButton = msgBox.addButton("OK", QMessageBox::RejectRole);
        msgBox.addButton("Cancel", QMessageBox::AcceptRole);
        int ret = msgBox.exec();
        Q_UNUSED(ret);
        if (msgBox.clickedButton() == okButton)
            emit expressionApplied();
    }
    histAdd();
}

void ExprDialog::verifiedAccept()
{
    applyExpression();
    if (grapher->expr.isValid()) {
        emit expressionApplied();
        emit dialogClosed();
        accept();
    } else {
        QMessageBox msgBox;
        msgBox.setText("Your expression had possible errors.");
        msgBox.setInformativeText("Do you want to accept your expression anyways?");
        QPushButton* okButton = msgBox.addButton("OK", QMessageBox::RejectRole);
        msgBox.addButton("Cancel", QMessageBox::AcceptRole);
        int ret = msgBox.exec();
        Q_UNUSED(ret);
        if (msgBox.clickedButton() == okButton) {
            emit expressionApplied();
            emit dialogClosed();
            accept();
        }
    }
}

void ExprDialog::reloadExpression()
{
    if (currentexprfile == "")
        return;

    std::ifstream file(currentexprfile.toStdString().c_str());
    std::string fileContents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    editor->setExpr(fileContents, false);
    histAdd();
}

void ExprDialog::setupHelp(QTabWidget* tab)
{
    QWidget* browserspace = new QWidget(tab);
    helpBrowser = new QTextBrowser(browserspace);
    tab->addTab(browserspace, "Help");

    // Locate help docs relative to location of the app itself
    QFile* helpDoc = new QFile(QCoreApplication::applicationDirPath() + "/../share/doc/SeExpr2/SeExpressions.html");
    if (helpDoc->exists()) {
        QString sheet =
            "body {background-color: #eeeeee; color: #000000;} \na {color: #3333ff; text-decoration: none;}\n";
        helpBrowser->document()->setDefaultStyleSheet(sheet);
        helpBrowser->setSource(helpDoc->fileName());
    }

    QPushButton* backPb = new QPushButton("Back");
    // backPb->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowLeft));
    backPb->setEnabled(false);
    QPushButton* forwardPb = new QPushButton("Forward");
    //    forwardPb->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowRight));
    forwardPb->setEnabled(false);

    QVBoxLayout* helpLayout = new QVBoxLayout(browserspace);
    QHBoxLayout* helpPbLayout = new QHBoxLayout;
    helpLayout->addLayout(helpPbLayout);
    helpPbLayout->addWidget(backPb);
    helpPbLayout->addWidget(forwardPb);
    // helpPbLayout->addItem(new QSpacerItem(0,0, QSizePolicy::MinimumExpanding,
    //                            QSizePolicy::Minimum));
    QHBoxLayout* findBar = new QHBoxLayout();
    helpPbLayout->addWidget(new QLabel("Find"), /*stretch*/ false);
    helpFindBox = new QLineEdit;
    helpPbLayout->addWidget(helpFindBox, /*stretch*/ false);
    connect(helpFindBox, SIGNAL(returnPressed()), this, SLOT(findNextInHelp()));
    QPushButton* nextButton = new QPushButton("Find Next");
    QPushButton* prevButton = new QPushButton("Find Prev");
    helpPbLayout->addWidget(nextButton, /*stretch*/ false);
    helpPbLayout->addWidget(prevButton, /*stretch*/ false);
    connect(nextButton, SIGNAL(clicked()), this, SLOT(findNextInHelp()));
    connect(prevButton, SIGNAL(clicked()), this, SLOT(findPrevInHelp()));
    helpPbLayout->addLayout(findBar, /*stretch*/ false);
    helpLayout->addWidget(helpBrowser, /*stretch*/ true);
    helpBrowser->setMinimumHeight(120);

    // wire up help browser forward/back buttons
    connect(backPb, SIGNAL(clicked()), helpBrowser, SLOT(backward()));
    connect(forwardPb, SIGNAL(clicked()), helpBrowser, SLOT(forward()));
    connect(helpBrowser, SIGNAL(backwardAvailable(bool)), backPb, SLOT(setEnabled(bool)));
    connect(helpBrowser, SIGNAL(forwardAvailable(bool)), forwardPb, SLOT(setEnabled(bool)));
}

void ExprDialog::findHelper(QTextDocument::FindFlags flags)
{
    QTextDocument* doc = helpBrowser->document();
    if (prevFind != helpFindBox->text()) {
        prevFind = helpFindBox->text();
        helpBrowser->setTextCursor(QTextCursor(doc));
    }
    QTextCursor blah = doc->find(helpFindBox->text(), helpBrowser->textCursor(), flags);
    helpBrowser->setTextCursor(blah);
}

void ExprDialog::findNextInHelp()
{
    findHelper(0);
}

void ExprDialog::findPrevInHelp()
{
    findHelper(QTextDocument::FindBackward);
}

void ExprDialog::previewExpression()
{
    applyExpression();
    emit preview();
}

void ExprDialog::applyExpression()
{
    editor->clearErrors();
    // set new expression
    grapher->expr.setExpr(editor->getExpr());
    grapher->expr.setDesiredReturnType(SeExpr2::ExprType().FP(3));
    grapher->update();

    // set the label widget to mention that functions and variables will not be previewed
    bool empty = true;
    std::stringstream s;
    if (grapher->expr.varmap.size() > 0 || grapher->expr.funcmap.size() > 0) {
        s << "<b>Variables/Functions not supported in preview (assumed zero):</b><br>";
        if (grapher->expr.varmap.size() > 0) {
            for (BasicExpression::VARMAP::iterator i = grapher->expr.varmap.begin(); i != grapher->expr.varmap.end();
                 ++i) {
                s << "$" << i->first << " ";
            }
            empty = false;
        }
        if (grapher->expr.funcmap.size() > 0) {
            for (BasicExpression::FUNCMAP::iterator i = grapher->expr.funcmap.begin(); i != grapher->expr.funcmap.end();
                 ++i) {
                s << "" << i->first << "() ";
            }
            empty = false;
        }
    }
    if (empty) {
        previewCommentLabel->setText("");
    } else {
        previewCommentLabel->setText(s.str().c_str());
    }

    // put errors into editor module
    bool valid = grapher->expr.isValid();
    if (!valid) {
        const std::vector<SeExpr2::Expression::Error>& errors = grapher->expr.getErrors();
        for (unsigned int i = 0; i < errors.size(); i++) {
            editor->addError(errors[i].startPos, errors[i].endPos, errors[i].error);
        }
        editor->nextError();
    }
}

void ExprDialog::clearExpression()
{
    browser->clearSelection();
    editor->setExpr("", false);
    applyExpression();
}

void removeDuplicates(QStringList& strlist)
{
    if (strlist.size() < 2)
        return;
    QStringList::Iterator it = strlist.begin();
    QString last = *it;
    std::vector<QStringList::Iterator> deletelist;
    for (++it; it != strlist.end(); ++it) {
        if (*it == last)
            deletelist.push_back(it);
        last = *it;
    }
    for (int i = 0; i < deletelist.size(); i++)
        strlist.erase(deletelist[i]);
}

void ExprDialog::enableBackForwards()
{
    // std::cout << currhistitem << " " << history.size() << "\n";
    if (currhistitem <= 0 || history.size() < 2)
        emit backwardAvailable(false);
    if (currhistitem > 0 && history.size() >= 2)
        emit backwardAvailable(true);

    if (currhistitem >= history.size() - 1 || history.size() < 2)
        emit forwardAvailable(false);
    if (currhistitem < history.size() - 1 && history.size() >= 2)
        emit forwardAvailable(true);
}
void ExprDialog::selectionChanged(const QString& str)
{
    currentexprfile = str;
    histAdd();
    previewExpression();
}

void ExprDialog::histBackward()
{
    if (history.isEmpty() || currhistitem <= 0)
        return;
    QString oldtext = editor->getExpr().c_str();
    if (currhistitem == history.size() - 1)
        history[currhistitem] = editor->getExpr().c_str();
    currhistitem--;
    editor->setExpr(history[currhistitem].toStdString());
    emit forwardAvailable(true);
    removeDuplicates(history);
    currhistitem = std::min(currhistitem, (int)history.size() - 1);
    enableBackForwards();
    if (oldtext == editor->getExpr().c_str() && currhistitem > 0)
        histBackward();
}

void ExprDialog::histForward()
{
    if (history.isEmpty() || currhistitem >= history.size() - 1)
        return;
    currhistitem++;
    editor->setExpr(history[currhistitem].toStdString());
    enableBackForwards();
}

void ExprDialog::histAdd()
{
    if (history.isEmpty() || editor->getExpr() == "")
        return;

    history.last() = editor->getExpr().c_str();
    currhistitem = history.size();
    history.push_back("");
    removeDuplicates(history);
    currhistitem = std::min(currhistitem, (int)history.size() - 1);
    enableBackForwards();
}
