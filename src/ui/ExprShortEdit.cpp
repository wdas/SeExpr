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
* @file ExprShortEdit.cpp
* @brief This provides an expression editor for SeExpr syntax with auto ui features
* @author  aselle
*/
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QHBoxLayout>
#include <QIcon>
#include <QCompleter>
#include <QTreeView>
#include <QScrollBar>
#include <QToolTip>
#include <QLabel>

#include "ExprShortEdit.h"
#include "ExprDialog.h"
#include "ExprBrowser.h"
#include "ExprHighlighter.h"
#include "ExprCompletionModel.h"
#include "ExprControlCollection.h"
#include "ExprPopupDoc.h"
#include "BasicExpression.h"

/* XPM */
static const char* sum_xpm[] = {"16 16 6 1",        "# c None",         ". c None",         "b c #808080",
                                "d c #010000",      "c c #aaaaaa",      "a c #303030",      "................",
                                ".#aaaaaaaaaa##..", ".abbbbbbcbbba...", ".#abbaaaaabba...", "..#aabba..aba...",
                                "....#abba..a#...", ".....#abba......", ".....#abba......", "...##abba...#...",
                                "...#abba...aa...", "..#abba...aca...", ".#abbaaaaabba...", ".abbbbbbbbbba...",
                                ".aaaaaaaaaaa#...", "................", "................"};

/* XPM */
static const char* stop_xpm[] = {"16 16 4 1",        "       c None",    ".      c #FF0000", "+      c #FF8080",
                                 "@      c #FFFFFF", "                ", "                ", "     ......     ",
                                 "    ...+++..    ", "   ....@@@...   ", "  .....@@@....  ", "  .....@@@....  ",
                                 "  .....@@@....  ", "  .....@@@....  ", "  ............  ", "   ....@@@....  ",
                                 "    ...@@@...   ", "     .......    ", "      .....     ", "                ",
                                 "                "};

ExprShortEdit::ExprShortEdit(QWidget* parent, bool expanded, bool applyOnSelect)
    : QWidget(parent)
    , controlRebuildTimer(new QTimer(this))
    , editDetail(new QToolButton())
    , controls(nullptr)
    , _expanded(nullptr)
    , _dialog(nullptr)
    , _context("")
    , _searchPath("")
    , _applyOnSelect(applyOnSelect)
{
    vboxlayout = new QVBoxLayout();
    vboxlayout->setSpacing(2);
    vboxlayout->setContentsMargins(0, 0, 0, 0);
    hboxlayout = new QHBoxLayout();
    hboxlayout->setSpacing(2);
    hboxlayout->setContentsMargins(0, 0, 0, 0);
    edit = new ExprShortTextEdit(parent);

    error = new QLabel();
    error->setPixmap(QPixmap(stop_xpm));
    error->setHidden(true);

    expandButton = new QToolButton();
    expandButton->setFixedSize(20, 20);
    expandButton->setFocusPolicy(Qt::NoFocus);
    expandButton->setArrowType(Qt::RightArrow);
    connect(expandButton, SIGNAL(clicked()), SLOT(expandPressed()));

    editDetail->setIcon(QIcon(QPixmap(sum_xpm)));
    hboxlayout->addWidget(expandButton);
    hboxlayout->addWidget(edit);
    hboxlayout->addWidget(error);
    hboxlayout->addWidget(editDetail);

    editDetail->setFixedSize(20, 20);
    connect(editDetail, SIGNAL(clicked()), SLOT(detailPressed()));
    connect(edit, SIGNAL(editingFinished()), SLOT(textFinished()));

    vboxlayout->addLayout(hboxlayout);
    setLayout(vboxlayout);
    connect(controlRebuildTimer, SIGNAL(timeout()), SLOT(rebuildControls()));
    checkErrors();
    rebuildControls();
    if (expanded)
        expandPressed();
}

ExprShortEdit::~ExprShortEdit()
{
}

void ExprShortEdit::setSearchPath(const QString& context, const QString& path)
{
    _context = context.toStdString();
    _searchPath = path.toStdString();
}

void ExprShortEdit::detailPressed()
{
    showDetails(-1);
}

void ExprShortEdit::showDetails(int idx)
{
    _dialog = new ExprDialog(this);
    _dialog->editor->replaceExtras(*edit->completionModel);

    _dialog->browser->setApplyOnSelect(_applyOnSelect);
    _dialog->browser->setSearchPath(_context.c_str(), _searchPath.c_str());
    _dialog->browser->expandAll();
    _dialog->setExpressionString(getExpressionString());
    if (idx >= 0) {
        _dialog->showEditor(idx);
    }

    connect(_dialog, SIGNAL(expressionApplied()), SLOT(expressionApplied()));
    connect(_dialog, SIGNAL(dialogClosed()), SLOT(dialogClosed()));
    _dialog->show();
    setEnabled(false);
    _dialog->setEnabled(true);
}

void ExprShortEdit::expressionApplied()
{
    setExpressionString(_dialog->getExpressionString());
}

void ExprShortEdit::dialogClosed()
{
    setEnabled(true);
}

void ExprShortEdit::rebuildControls()
{
    if (!controls) {
        controls = new ExprControlCollection(0, false);
        connect(controls, SIGNAL(controlChanged(int)), SLOT(controlChanged(int)));
    }
    bool wasShown = !edit->completer->popup()->isHidden();
    bool newVariables = controls->rebuildControls(getExpression(), edit->completionModel->local_variables);
    if (controls->numControls() == 0) {
        controls->deleteLater();
        controls = nullptr;
        expandButton->setArrowType(Qt::RightArrow);
        expandButton->setVisible(false);
        _expanded = false;
    } else {
        vboxlayout->addWidget(controls);
        expandButton->setVisible(true);
        if (_expanded) {
            controls->setVisible(true);
            expandButton->setArrowType(Qt::DownArrow);
        } else {
            controls->setVisible(false);
            expandButton->setArrowType(Qt::RightArrow);
        }
    }
    if (newVariables)
        edit->completer->setModel(edit->completionModel);
    if (wasShown)
        edit->completer->popup()->show();
}

void ExprShortEdit::expandPressed()
{
    if (_expanded) {
        _expanded = false;
        expandButton->setArrowType(Qt::RightArrow);
        if (controls) {
            controls->setVisible(false);
        }
    } else {
        if (controls) {
            controls->setVisible(true);
            expandButton->setArrowType(Qt::DownArrow);
            _expanded = true;
        } else {
            _expanded = false;
            expandButton->setArrowType(Qt::RightArrow);
        }
    }
}

void ExprShortEdit::handleTextEdited()
{
}

void ExprShortEdit::textFinished()
{
    controlRebuildTimer->setSingleShot(true);
    controlRebuildTimer->start(0);
    checkErrors();
    emit exprChanged();
}

void ExprShortEdit::setExpressionString(const std::string& expression)
{
    edit->setText(QString(expression.c_str()));
    controlRebuildTimer->setSingleShot(true);
    controlRebuildTimer->start(0);
    checkErrors();
    emit exprChanged();
}

QString ExprShortEdit::getExpression() const
{
    return edit->toPlainText();
}

std::string ExprShortEdit::getExpressionString() const
{
    return getExpression().toStdString();
}

void ExprShortEdit::controlChanged(int id)
{
    if (controls) {
        QString newText = getExpression();
        controls->updateText(id, newText);
        edit->setText(newText);
        checkErrors();
        emit exprChanged();
    }
}

void ExprShortEdit::clearExtraCompleters()
{
    edit->completionModel->clearFunctions();
    edit->completionModel->clearVariables();
}

void ExprShortEdit::registerExtraFunction(const std::string& name, const std::string& docString)
{
    edit->completionModel->addFunction(name.c_str(), docString.c_str());
}

void ExprShortEdit::registerExtraVariable(const std::string& name, const std::string& docString)
{
    edit->completionModel->addVariable(name.c_str(), docString.c_str());
}

void ExprShortEdit::updateCompleter()
{
    edit->completer->setModel(edit->completionModel);
}

void ExprShortEdit::checkErrors()
{
    BasicExpression expr(getExpressionString());
    bool valid = expr.isValid();
    std::string err;
    if (!valid)
        err = expr.parseError();

    hideErrors(valid, err);
}

void ExprShortEdit::hideErrors(bool hidden, const std::string& err)
{
    error->setHidden(hidden);
    if (!hidden) {
        error->setToolTip(QString::fromStdString(err));
    }
}

int ExprShortEdit::numControls() const
{
    return controls ? controls->numControls() : 0;
}

void ExprShortEdit::setSimple(bool enabled)
{
    edit->setHidden(enabled);
    editDetail->setHidden(enabled);
    expandButton->setHidden(enabled);
}

void ExprShortEdit::setDetailsMenu(QMenu* menu)
{
    editDetail->setMenu(menu);
}

void ExprShortEdit::setVerticalScrollBarPolicy(Qt::ScrollBarPolicy policy)
{
    edit->setVerticalScrollBarPolicy(policy);
}

void ExprShortEdit::setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy policy)
{
    edit->setHorizontalScrollBarPolicy(policy);
}

void ExprShortEdit::setLineWrapMode(QTextEdit::LineWrapMode mode)
{
    edit->setLineWrapMode(mode);
}

ExprShortTextEdit::ExprShortTextEdit(QWidget* parent) : QTextEdit(parent), editing(false), _tip(0)
{
    lastStyleForHighlighter = 0;
    setMaximumHeight(25);
    highlighter = new ExprHighlighter(document());
    highlighter->fixStyle(palette());
    highlighter->rehighlight();
    repaint();

    // setup auto completion
    completer = new QCompleter();
    completionModel = new ExprCompletionModel(this);
    completer->setModel(completionModel);
    QTreeView* treePopup = new QTreeView;
    completer->setPopup(treePopup);
    treePopup->setRootIsDecorated(false);
    treePopup->setMinimumWidth(300);
    treePopup->setMinimumHeight(50);
    treePopup->setItemsExpandable(true);

    completer->setWidget(this);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    QObject::connect(completer, SIGNAL(activated(const QString&)), this, SLOT(insertCompletion(const QString&)));
}

void ExprShortTextEdit::focusInEvent(QFocusEvent* e)
{
    // setTextCursor(QTextCursor(document()));
    if (completer)
        completer->setWidget(this);
    QTextEdit::focusInEvent(e);
}

void ExprShortTextEdit::focusOutEvent(QFocusEvent* e)
{
    // setTextCursor(QTextCursor());
    finishEdit();
    QTextCursor newCursor = textCursor();
    newCursor.clearSelection();
    setTextCursor(newCursor);
    setColor(false);
    hideTip();
    QTextEdit::focusOutEvent(e);
}

void ExprShortTextEdit::mousePressEvent(QMouseEvent* event)
{
    hideTip();
    QTextEdit::mousePressEvent(event);
}

void ExprShortTextEdit::mouseDoubleClickEvent(QMouseEvent* event)
{
    hideTip();
    QTextEdit::mouseDoubleClickEvent(event);
}

void ExprShortTextEdit::paintEvent(QPaintEvent* e)
{
    if (lastStyleForHighlighter != style()) {
        lastStyleForHighlighter = style();
        highlighter->fixStyle(palette());
        highlighter->rehighlight();
    }
    QTextEdit::paintEvent(e);
}

void ExprShortTextEdit::keyPressEvent(QKeyEvent* e)
{
    // If the completer is active pass keys it needs down
    if (completer && completer->popup()->isVisible()) {
        switch (e->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return;
        default:
            break;
        }
    }

    // Accept expression
    if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
        selectAll();
        finishEdit();
        return;
    } else if (e->key() == Qt::Key_Escape) {
        setText(savedText);
        selectAll();
        finishEdit();
        return;
    } else if (e->key() == Qt::Key_Tab) {
        QWidget::keyPressEvent(e);
        return;
    } else if (!editing) {
        editing = true;
        setColor(true);
        savedText = toPlainText();
    }

    // use the values here as long as we are not using the shortcut to bring up the editor
    bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E);  // CTRL+E
    if (!isShortcut)  // dont process the shortcut when we have a completer
        QTextEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (!completer || (ctrlOrShift && e->text().isEmpty()))
        return;

    bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;

    // grab the line we're on
    QTextCursor tc = textCursor();
    tc.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
    QString line = tc.selectedText();

    // matches the last prefix of a completable variable or function and extract as completionPrefix
    static QRegExp completion("^(?:.*[^A-Za-z0-9_$])?((?:\\$[A-Za-z0-9_]*)|[A-Za-z]+[A-Za-z0-9_]*)$");
    int index = completion.indexIn(line);
    QString completionPrefix;
    if (index != -1 && !line.contains('#')) {
        completionPrefix = completion.cap(1);
        // std::cout<<"we have completer prefix '"<<completionPrefix.toStdString()<<"'"<<std::endl;
    }

    // hide the completer if we have too few characters, we are at end of word
    if (!isShortcut && (hasModifier || e->text().isEmpty() || completionPrefix.length() < 1 || index == -1)) {
        completer->popup()->hide();
    } else {
        // copy the completion prefix in if we don't already have it in the completer
        if (completionPrefix != completer->completionPrefix()) {
            completer->setCompletionPrefix(completionPrefix);
            completer->popup()->setCurrentIndex(completer->completionModel()->index(0, 0));
        }

        // display the completer
        QRect cr = cursorRect();
        cr.setWidth(2 * (completer->popup()->sizeHintForColumn(0) + completer->popup()->sizeHintForColumn(1) +
                         completer->popup()->verticalScrollBar()->sizeHint().width()));
        completer->complete(cr);
        hideTip();
        return;
    }

    // documentation completion
    static QRegExp inFunction("^(?:.*[^A-Za-z0-9_$])?([A-Za-z0-9_]+)\\([^()]*$");
    int index2 = inFunction.indexIn(line);
    if (index2 != -1) {
        QString functionName = inFunction.cap(1);
        QStringList tips = completionModel->getDocString(functionName).split("\n");
        QString tip = "<b>" + tips[0] + "</b>";
        for (int i = 1; i < tips.size(); i++) {
            tip += "<br>" + tips[i];
        }
        showTip(tip);
    } else {
        hideTip();
    }
}

void ExprShortTextEdit::showTip(const QString& string)
{
    // skip empty strings
    if (string == "")
        return;
    // skip already shwon stuff
    // if(_tip && !_tip->isHidden() && _tip->label->text() == string) return;

    QRect cr = cursorRect();
    cr.setX(0);
    cr.setWidth(cr.width() * 3);
    if (_tip) {
        delete _tip;
        _tip = 0;
    }
    _tip = new ExprPopupDoc(this, mapToGlobal(cr.bottomLeft() + QPoint(0, 6)), string);
}

void ExprShortTextEdit::hideTip()
{
    if (_tip)
        _tip->hide();
}

void ExprShortTextEdit::insertCompletion(const QString& completion)
{
    if (completer->widget() != this)
        return;
    QTextCursor tc = textCursor();
    int extra = completion.length() - completer->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    if (completion[0] != '$')
        tc.insertText("(");
    setTextCursor(tc);
}

void ExprShortTextEdit::finishEdit()
{
    editing = false;
    setColor(false);
    emit editingFinished();
}

void ExprShortTextEdit::setColor(bool editing)
{
    Q_UNUSED(editing);
    // todo: decorate when editing
}
