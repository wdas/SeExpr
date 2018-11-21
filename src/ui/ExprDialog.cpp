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

#include <iostream>
#include <fstream>

#include <QDir>
#include <QLabel>

#include "ExprBrowser.h"
#include "ExprControlCollection.h"
#include "ExprDialog.h"
#include "ExprGrapher2d.h"
#include "ExprHelp.h"
#include "ExprWidgets.h"

#define P3D_CONFIG_ENVVAR "P3D_CONFIG_PATH"

ClickableLabel::ClickableLabel(QWidget* parent) : QLabel(parent)
{
}

ClickableLabel::ClickableLabel(const QString& label) : QLabel(label)
{
}

void ClickableLabel::enterEvent(QEvent* event)
{
    pressed = false;
}

void ClickableLabel::leaveEvent(QEvent* event)
{
    pressed = false;
}

void ClickableLabel::mousePressEvent(QMouseEvent* event)
{
    pressed = true;
}

void ClickableLabel::mouseReleaseEvent(QMouseEvent* event)
{
    if (pressed) {
        emit(clicked());
    }
    pressed = false;
}

ExprDialog::ExprDialog(QWidget* parent, bool graphMode) : QDialog(parent), _currentEditorIdx(0), currhistitem(0)
{
    graph = graphMode;
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
    int widgetIdx = 0;
    grapher = new ExprGrapherWidget(this, 256, 256);
    if(graph){
        previewLayout->addWidget(grapher, widgetIdx);
	widgetIdx += 1;
    }
    leftLayout->addLayout(previewLayout);
    previewLibraryLayout->addWidget(leftWidget, widgetIdx);

    // setup button bar
    // QWidget* buttonBarWidget=new QWidget();
    QHBoxLayout* buttonBarLayout = new QHBoxLayout();
    // buttonBarWidget->setLayout(buttonBarLayout);
    buttonBarLayout->setMargin(1);
    previewButton = new QPushButton("Preview");
    buttonBarLayout->addWidget(previewButton);

    QToolButton* histBack = toolButton(this);
    buttonBarLayout->addWidget(histBack);
    histBack->setIcon(QIcon(SEEXPR_EDITOR_ICON_PATH "back.png"));
    histBack->setToolTip("Previous In History");
    histBack->setEnabled(0);
    histBack->setFixedSize(24, 24);
    histBack->setIconSize(QSize(16, 16));
    histBack->setFocusPolicy(Qt::NoFocus);
    QToolButton* histForw = toolButton(this);
    buttonBarLayout->addWidget(histForw);
    histForw->setIcon(QIcon(SEEXPR_EDITOR_ICON_PATH "forward.png"));
    histForw->setToolTip("Next In History");
    histForw->setEnabled(0);
    histForw->setFixedSize(24, 24);
    histForw->setIconSize(QSize(16, 16));
    histForw->setFocusPolicy(Qt::NoFocus);
    history.push_back("");
    connect(this, SIGNAL(backwardAvailable(bool)), histBack, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(forwardAvailable(bool)), histForw, SLOT(setEnabled(bool)));
    QToolButton* reloadExprPb = toolButton(this);
    reloadExprPb->setIcon(QIcon(SEEXPR_EDITOR_ICON_PATH "reload.png"));
    reloadExprPb->setFixedSize(24, 24);
    reloadExprPb->setToolTip("Reload current expression");
    buttonBarLayout->addWidget(reloadExprPb);
    connect(reloadExprPb, SIGNAL(clicked()), this, SLOT(reloadExpression()));

    buttonBarLayout->addStretch(1);
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
    if(graph)
        connect(grapher, SIGNAL(preview()), SLOT(previewExpression()));
    bottomLayout->addWidget(editor);

    // make expression library browser
    browser = new ExprBrowser(0, editor);
    previewLibraryLayout->addWidget(browser);

    // dialog buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout(0);
    errorCountLabel = new ClickableLabel("0 Errors");
    warningCountLabel = new ClickableLabel("0 Warnings");
    buttonLayout->addWidget(errorCountLabel, 0);
    buttonLayout->addSpacing(10);
    buttonLayout->addWidget(warningCountLabel, 0);
    buttonLayout->addStretch(1);
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

    exprHelp = new ExprHelp();
    topTabWidget->addTab(exprHelp, "Help");

    // connect buttons
    connect(errorCountLabel, SIGNAL(clicked()), editor, SLOT(nextError()));
    connect(warningCountLabel, SIGNAL(clicked()), editor, SLOT(nextError()));
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
    browser->populate();
    browser->expandAll();
    QDialog::show();
}

int ExprDialog::exec()
{
    // populate the expressions
    browser->populate();
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
    if(graph)
        grapher->update();

    int numWarnings = 0;
    int numErrors = 0;
    // set the label widget to mention that functions and variables will not be previewed
    bool empty = true;
    if (grapher->expr.varmap.size() > 0 || grapher->expr.funcmap.size() > 0) {
        if (grapher->expr.varmap.size() > 0) {
            for (BasicExpression::VARMAP::iterator i = grapher->expr.varmap.begin(); i != grapher->expr.varmap.end();
                 ++i) {
                std::stringstream ss;
                ss << "Warning: variable \"" << i->first << "\" not defined, assumed zero";
                editor->addError(-1, -1, ss.str());
                ++numWarnings;
            }
            empty = false;
        }
        if (grapher->expr.funcmap.size() > 0) {
            for (BasicExpression::FUNCMAP::iterator i = grapher->expr.funcmap.begin(); i != grapher->expr.funcmap.end();
                 ++i) {
                std::stringstream ss;
                ss << "Warning: function \"" << i->first << "\" not defined, assumed zero";
                editor->addError(-1, -1, ss.str());
                ++numWarnings;
            }
            empty = false;
        }
    }

    // put errors into editor module
    if (!grapher->exprValid()) {
        const std::vector<SeExpr2::Expression::Error>& errors = grapher->expr.getErrors();
        for (unsigned int i = 0; i < errors.size(); i++) {
            editor->addError(errors[i].startPos, errors[i].endPos, std::string("Error: " + errors[i].error));
            ++numErrors;
        }
    }
    warningCountLabel->setText(QString("%1 Warnings").arg(numWarnings));
    errorCountLabel->setText(QString("%1 Errors").arg(numErrors));
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
