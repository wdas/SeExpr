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
* @file ExprEditor.cpp
* @brief This provides an expression editor for SeExpr syntax with auto ui features
* @author  aselle
*/
#include <QRegExp>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QSplitter>
#include <QLabel>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QScrollArea>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QTextCharFormat>
#include <QCompleter>
#include <QAbstractItemView>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QScrollBar>
#include <QToolTip>
#include <QListWidget>
#include <QTreeView>
#include <QAction>
#include <QMenu>
#include <QMimeData>
#include <QAction>

#include <SeExpr2/Expression.h>
#include <SeExpr2/ExprNode.h>
#include <SeExpr2/ExprFunc.h>
#include <SeExpr2/ExprBuiltins.h>

#include "ExprEditor.h"
#include "ExprHighlighter.h"
#include "ExprCompletionModel.h"
#include "ExprControlCollection.h"
#include "ExprCurve.h"
#include "ExprColorCurve.h"
#include "ExprControl.h"
#include "ExprPopupDoc.h"
#include "ExprWidgets.h"

void ExprEditor::controlChanged(int id)
{
    QString newText = exprTe->toPlainText();
    controls->updateText(id, newText);
    _updatingText = 1;
    exprTe->selectAll();
    exprTe->insertPlainText(newText);
    // exprTe->setPlainText(newText);
    _updatingText = 0;

    // schedule preview update
    previewTimer->setSingleShot(true);
    previewTimer->start(0);
}

ExprEditor::~ExprEditor()
{
    delete controlRebuildTimer;
    delete previewTimer;
}

ExprTextEdit::~ExprTextEdit()
{
}

ExprEditor::ExprEditor(QWidget* parent, ExprControlCollection* controls)
    : QWidget(parent), _updatingText(0), errorHeight(0)
{
    // timers
    controlRebuildTimer = new QTimer();
    previewTimer = new QTimer();

    // title and minimum size
    setWindowTitle("Expression Editor");
    setMinimumHeight(100);

    // expression controls, we need for signal connections
    this->controls = controls;

    // make layout
    QVBoxLayout* exprAndErrors = new QVBoxLayout;
    exprAndErrors->setMargin(0);
    setLayout(exprAndErrors);

    searchBar = new QWidget();
    QHBoxLayout* searchBarLayout = new QHBoxLayout();
    searchBarLayout->setMargin(1);
    searchLine = new QLineEdit(this);
    replaceLine = new QLineEdit(this);
    QPushButton* findButton = new QPushButton("Find");
    QPushButton* replace = new QPushButton("Replace");
    QPushButton* replaceAll = new QPushButton("Replace All");
    caseSensitive = toolButton(this, true);
    caseSensitive->setIcon(QIcon(SEEXPR_EDITOR_ICON_PATH "caseSensitive.png"));
    caseSensitive->setFixedSize(20,20);
    caseSensitive->setIconSize(QSize(16,16));
    caseSensitive->setCheckable(true);
    caseSensitive->setToolTip("Match Case");
    wholeWords = toolButton(this, true);
    wholeWords->setIcon(QIcon(SEEXPR_EDITOR_ICON_PATH "wholeWord.png"));
    wholeWords->setFixedSize(20,20);
    wholeWords->setIconSize(QSize(16,16));
    wholeWords->setCheckable(true);
    wholeWords->setToolTip("Whole Word");
    QToolButton* closeSearch = toolButton(this);
    closeSearch->setIcon(QIcon(SEEXPR_EDITOR_ICON_PATH "close.png"));
    closeSearch->setFixedSize(20,20);
    closeSearch->setIconSize(QSize(16,16));

    searchBarLayout->addWidget(findButton);
    searchBarLayout->addWidget(searchLine);
    searchBarLayout->addWidget(replaceLine);
    searchBarLayout->addWidget(replace);
    searchBarLayout->addWidget(replaceAll);
    searchBarLayout->addWidget(caseSensitive);
    searchBarLayout->addWidget(wholeWords);
    searchBarLayout->addWidget(closeSearch);
    searchBar->setLayout(searchBarLayout);
    
    
    searchBar->setVisible(false);
    connect(findButton, SIGNAL(clicked()), this, SLOT(find()));
    connect(replace, SIGNAL(clicked()), this, SLOT(replace()));
    connect(replaceAll, SIGNAL(clicked()), this, SLOT(replaceAll()));
    connect(closeSearch, SIGNAL(clicked()), this, SLOT(closeFind()));
    connect(caseSensitive, SIGNAL(toggled(bool)), this, SLOT(findAll()));
    connect(wholeWords, SIGNAL(toggled(bool)), this, SLOT(findAll()));
    connect(searchLine, SIGNAL(textEdited(QString)), this, SLOT(findAll()));

    // create text editor widget
    exprTe = new ExprTextEdit(this);
    exprTe->setMinimumHeight(50);

    // calibrate the font size
    int fontsize = 12;
    while (QFontMetrics(QFont("Liberation Sans", fontsize)).width("abcdef") < 38 && fontsize < 20)
        fontsize++;
    while (QFontMetrics(QFont("Liberation Sans", fontsize)).width("abcdef") > 44 && fontsize > 3)
        fontsize--;

    exprTe->setFont(QFont("Liberation Sans", fontsize));

    exprAndErrors->addWidget(exprTe);

    // create error widget
    errorWidget = new QListWidget();
    errorWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    errorWidget->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum));
    connect(errorWidget, SIGNAL(itemSelectionChanged()), SLOT(selectError()));
    clearErrors();
    exprAndErrors->addWidget(errorWidget);
    exprAndErrors->addWidget(searchBar);

    // wire up signals
    connect(exprTe, SIGNAL(applyShortcut()), SLOT(sendApply()));
    connect(exprTe, SIGNAL(nextError()), SLOT(nextError()));
    connect(exprTe, SIGNAL(textChanged()), SLOT(exprChanged()));
    connect(controls, SIGNAL(controlChanged(int)), SLOT(controlChanged(int)));
    connect(controls, SIGNAL(insertString(const std::string&)), SLOT(insertStr(const std::string&)));
    connect(controlRebuildTimer, SIGNAL(timeout()), SLOT(rebuildControls()));
    connect(previewTimer, SIGNAL(timeout()), SLOT(sendPreview()));
    
    QAction *find = new QAction(this);
    find->setShortcut(Qt::Key_F | Qt::CTRL);    
    connect(find, SIGNAL(triggered()), this, SLOT(showFind()));
    this->addAction(find);
    
    QAction *closeFind = new QAction(this);
    closeFind->setShortcut(Qt::Key_Escape);    
    connect(closeFind, SIGNAL(triggered()), this, SLOT(closeFind()));
    this->addAction(closeFind);
}

void ExprEditor::selectError()
{
    int selected = errorWidget->currentRow();
    QListWidgetItem* item = errorWidget->item(selected);
    int start = item->data(Qt::UserRole).toInt();
    int end = item->data(Qt::UserRole + 1).toInt();
    QTextCursor cursor = exprTe->textCursor();
    cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, start);
    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, end - start + 1);
    exprTe->setTextCursor(cursor);
}

void ExprEditor::sendApply()
{
    emit apply();
}

void ExprEditor::sendPreview()
{
    emit preview();
}

void ExprEditor::exprChanged()
{
    if (_updatingText)
        return;
    if (searchBar->isVisible())
        findAll();

    // schedule control rebuild
    controlRebuildTimer->setSingleShot(true);
    controlRebuildTimer->start(0);
}

void ExprEditor::rebuildControls()
{
    bool wasShown = !exprTe->completer->popup()->isHidden();
    bool newVariables = controls->rebuildControls(exprTe->toPlainText(), exprTe->completionModel->local_variables);
    if (newVariables)
        exprTe->completer->setModel(exprTe->completionModel);
    if (wasShown)
        exprTe->completer->popup()->show();
}

void ExprEditor::showFind()
{
    QTextCursor tc = exprTe->textCursor();
    if (tc.hasSelection()){
        searchLine->setText(tc.selectedText());
    }
    searchLine->setFocus();
    searchBar->setVisible(true);
    findAll();
}

bool ExprEditor::find() 
{
    findAll();
    QTextDocument::FindFlags flags = 0;
    if (caseSensitive->isChecked())
        flags |= QTextDocument::FindCaseSensitively;
    if (wholeWords->isChecked())
        flags |= QTextDocument::FindWholeWords;
    
    bool found = exprTe->find(searchLine->text(), flags);
    if (!found){
      QTextCursor tc = exprTe->textCursor();
      tc.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor,1);
      exprTe->setTextCursor(tc);
      found = exprTe->find(searchLine->text(), flags);
    }
    
    return found;
}

void ExprEditor::findAll()
{
    QTextDocument::FindFlags flags = 0;
    if (caseSensitive->isChecked())
        flags |= QTextDocument::FindCaseSensitively;
    if (wholeWords->isChecked())
        flags |= QTextDocument::FindWholeWords;
    
    QTextDocument* doc = exprTe->document();
    QTextCursor* tc = new QTextCursor(doc);
    
    tc->movePosition(QTextCursor::Start, QTextCursor::MoveAnchor,1);
    
    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection sel;
    
    bool found = true;
    while (found) {
        QTextCursor blah = doc->find(searchLine->text(), *tc, flags);
        if (!blah.hasSelection()){
            found = false;
        } else {
            QTextEdit::ExtraSelection sel;
            QColor bg(80, 80, 110, 189);
            sel.format.setBackground(bg);
            sel.cursor = blah;
            extraSelections.append(sel);
            tc = &blah;
        }
    }
    exprTe->setExtraSelections(extraSelections);
}

void ExprEditor::replace() 
{
    QTextCursor tc = exprTe->textCursor();
    if (!tc.hasSelection())
      return;
    QString replaceText = replaceLine->text();

    tc.beginEditBlock();
    tc.removeSelectedText();
    tc.insertText(replaceText);
    tc.endEditBlock();
    find();
}

void ExprEditor::replaceAll() 
{    
    int pos = exprTe->verticalScrollBar()->value();
    QTextCursor tc = exprTe->textCursor();
    tc.beginEditBlock();
    while (find()) {
        replace();
    }
    tc.endEditBlock();
     exprTe->verticalScrollBar()->setValue(pos);
}

void ExprEditor::closeFind()
{
    searchBar->setVisible(false);
    QList<QTextEdit::ExtraSelection> extraSelections;
    exprTe->setExtraSelections(extraSelections);
    exprTe->setFocus();
}

void ExprTextEdit::updateStyle()
{
    lastStyleForHighlighter = 0;
    highlighter->fixStyle(palette());
    highlighter->rehighlight();
    repaint();
}

ExprTextEdit::ExprTextEdit(QWidget* parent) : QTextEdit(parent), lastStyleForHighlighter(0), _tip(0)
{
    highlighter = new ExprHighlighter(document());
    setWordWrapMode(QTextOption::NoWrap);

    QFont font;
    font.setFamily("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(10);
    setFont(font);
    const int tabStop = 4;  // 4 characters

    QFontMetrics metrics(font);
    setTabStopWidth(tabStop * metrics.width(' '));

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

    _popupEnabledAction = new QAction("Pop-up Help", this);
    _popupEnabledAction->setCheckable(true);
    _popupEnabledAction->setChecked(true);

    _commentAction = new QAction("Toggle Comments", this);
    _commentAction->setShortcut(Qt::Key_Slash | Qt::CTRL);
    connect(_commentAction, SIGNAL(triggered()), this, SLOT(commentLines()));
    this->addAction(_commentAction);
}

void ExprTextEdit::insertFromMimeData(const QMimeData* source)
{
    QTextEdit::insertPlainText(source->text());
}

void ExprTextEdit::focusInEvent(QFocusEvent* e)
{
    if (completer)
        completer->setWidget(this);
    QTextEdit::focusInEvent(e);
}

void ExprTextEdit::focusOutEvent(QFocusEvent* e)
{
    hideTip();
    QTextEdit::focusInEvent(e);
}

void ExprTextEdit::mousePressEvent(QMouseEvent* event)
{
    hideTip();
    QTextEdit::mousePressEvent(event);
}

void ExprTextEdit::mouseDoubleClickEvent(QMouseEvent* event)
{
    hideTip();
    QTextEdit::mouseDoubleClickEvent(event);
}

void ExprTextEdit::paintEvent(QPaintEvent* event)
{
    if (lastStyleForHighlighter != style()) {
        lastStyleForHighlighter = style();
        highlighter->fixStyle(palette());
        highlighter->rehighlight();
    }
    QTextEdit::paintEvent(event);
}

void ExprTextEdit::wheelEvent(QWheelEvent* event)
{
    if (event->modifiers() == Qt::ControlModifier) {
        if (event->delta() > 0)
            zoomIn();
        else if (event->delta() < 0)
            zoomOut();
        return;
    }
    return QTextEdit::wheelEvent(event);
}

void ExprTextEdit::keyPressEvent(QKeyEvent* e)
{
    // Accept expression
    if (e->key() == Qt::Key_Return && e->modifiers() == Qt::ControlModifier) {
        emit applyShortcut();
        return;
    } else if (e->key() == Qt::Key_F4) {
        emit nextError();
        return;
    }

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
    if (e->key() == Qt::Key_Tab) {
        tabLines(true);
        return;
    }
    if (e->key() == Qt::Key_Backtab) {
        tabLines(false);
        return;
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
    } else if (_popupEnabledAction->isChecked()) {
        // copy the completion prefix in if we don't already have it in the completer
        if (completionPrefix != completer->completionPrefix()) {
            completer->setCompletionPrefix(completionPrefix);
            completer->popup()->setCurrentIndex(completer->completionModel()->index(0, 0));
        }

        // display the completer
        QRect cr = cursorRect();
        cr.setWidth(completer->popup()->sizeHintForColumn(0) + completer->popup()->sizeHintForColumn(1) +
                    completer->popup()->verticalScrollBar()->sizeHint().width());
        cr.translate(0, 6);
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
        if (_popupEnabledAction->isChecked())
            showTip(tip);
        // QToolTip::showText(mapToGlobal(cr.bottomLeft()),tip,this,cr);
    } else {
        hideTip();
    }
}

void ExprTextEdit::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu* menu = createStandardContextMenu();

    if (!menu->actions().empty()) {
        QAction* f = menu->actions().first();
        menu->insertAction(f, _popupEnabledAction);
        menu->insertSeparator(f);
        menu->addAction(_commentAction);
    }

    menu->exec(event->globalPos());
    delete menu;
}

void ExprTextEdit::showTip(const QString& string)
{
    // skip empty strings
    if (string == "")
        return;
    // skip already shown stuff
    if (_tip && !_tip->isHidden() && _tip->label->text() == string)
        return;

    QRect cr = cursorRect();
    cr.setX(0);
    cr.setWidth(cr.width() * 3);
    if (_tip) {
        delete _tip;
        _tip = 0;
    }
    _tip = new ExprPopupDoc(this, mapToGlobal(cr.bottomLeft()) + QPoint(0, 6), string);
}

void ExprTextEdit::hideTip()
{
    if (_tip)
        _tip->hide();
}

void ExprTextEdit::insertCompletion(const QString& completion)
{
    if (completer->widget() != this)
        return;
    QTextCursor tc = textCursor();
    int extra = completion.length() - completer->completionPrefix().length();
    tc.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, completer->completionPrefix().length());
    tc.removeSelectedText();
    tc.insertText(completion);
    if (completion[0] != '$')
        tc.insertText("(");
    setTextCursor(tc);
}

void ExprTextEdit::tabLines(bool indent)
{
    QTextCursor tc = textCursor();

    tc.beginEditBlock();

    int relativePos = tc.position() - tc.block().position();
    bool hasSelection = tc.hasSelection();

    int start = tc.anchor();
    int end = tc.position();

    if (start > end)
        std::swap(start, end);

    tc.setPosition(start, QTextCursor::MoveAnchor);
    int startBlock = tc.block().blockNumber();

    tc.setPosition(end, QTextCursor::MoveAnchor);
    int endBlock = tc.block().blockNumber();

    tc.setPosition(start, QTextCursor::MoveAnchor);
    int range = endBlock - startBlock;

    QString text;
    for (int i = 0; i <= range; i++) {
        tc.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);

        if (indent) {
            tc.insertText("\t");
        } else {
            tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            text = tc.selectedText();
            QString trimmedText = text.trimmed();
            bool found = false;
            if (text.startsWith("\t")) {
                tc.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
                tc.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 1);
                tc.removeSelectedText();
                int index = text.indexOf("\t");
            }
        }

        tc.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor);
    }
    tc = textCursor();
    if (relativePos == 0 && indent)
        tc.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, 1);
    if (!hasSelection)
        tc.clearSelection();

    setTextCursor(tc);

    tc.endEditBlock();
}


void ExprTextEdit::commentLines()
{
    QTextCursor tc = textCursor();

    tc.beginEditBlock();

    int relativePos = tc.position() - tc.block().position();
    bool hasSelection = tc.hasSelection();

    int start = tc.anchor();
    int end = tc.position();

    if (start > end)
        std::swap(start, end);

    tc.setPosition(start, QTextCursor::MoveAnchor);
    int startBlock = tc.block().blockNumber();

    tc.setPosition(end, QTextCursor::MoveAnchor);
    int endBlock = tc.block().blockNumber();

    tc.setPosition(start, QTextCursor::MoveAnchor);
    int range = endBlock - startBlock;

    bool addComments = true;
    std::vector<bool> modes;

    QString text;
    for (int i = 0; i <= range; i++) {
        tc.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
        tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        text = tc.selectedText();
        text = text.trimmed();
        if (text.startsWith('#'))
            modes.push_back(false);
        else
            modes.push_back(true);

        tc.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor);
    }

    if (std::adjacent_find(modes.begin(), modes.end(), std::not_equal_to<int>()) == modes.end())
        addComments = modes[0];

    tc.setPosition(start, QTextCursor::MoveAnchor);
    for (int i = 0; i <= range; i++) {
        tc.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);

        if (addComments) {
            tc.insertText("# ");
        } else {
            tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            text = tc.selectedText();
            QString trimmedText = text.trimmed();
            bool found = false;
            if (trimmedText.startsWith('#')) {
                tc.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
                int index = text.indexOf("# ");
                if (index != -1) {
                    tc.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, index);
                    tc.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 2);
                    tc.removeSelectedText();
                } else {
                    index = text.indexOf('#');
                    if (index != -1) {
                        tc.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, index);
                        tc.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 1);
                        tc.removeSelectedText();
                    }
                }
            }
        }

        tc.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor);
    }
    tc = textCursor();
    if (relativePos == 0 && addComments) {
        tc.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, 2);
    }
    if (!hasSelection)
        tc.clearSelection();

    setTextCursor(tc);

    tc.endEditBlock();
}

std::string ExprEditor::getExpr()
{
    return exprTe->toPlainText().toStdString();
}

void ExprEditor::setExpr(const std::string& expression, const bool doApply)
{
    // exprTe->clear();
    exprTe->selectAll();
    exprTe->insertPlainText(QString::fromStdString(expression));
    clearErrors();
    exprTe->moveCursor(QTextCursor::Start);
    if (doApply)
        emit apply();
}

void ExprEditor::insertStr(const std::string& str)
{
    exprTe->insertPlainText(QString::fromStdString(str));
}

void ExprEditor::appendStr(const std::string& str)
{
    exprTe->append(QString::fromStdString(str));
}

void ExprEditor::addError(const int startPos, const int endPos, const std::string& error)
{
    QListWidgetItem* item = new QListWidgetItem(("Error: " + error).c_str(), errorWidget);
    item->setData(Qt::UserRole, startPos);
    item->setData(Qt::UserRole + 1, endPos);
    errorWidget->setHidden(false);
    // TODO: fix to not use count lines and compute heuristic of 25 pixels per line!
    const char* c = error.c_str();
    int lines = 1;
    while (*c != '\0') {
        if (*c == '\n')
            lines++;
        c++;
    }
    errorHeight += 25 * lines;
    // widget should not need to be bigger than this
    errorWidget->setMaximumHeight(errorHeight);
}

void ExprEditor::nextError()
{
    int newRow = errorWidget->currentRow() + 1;
    if (newRow >= errorWidget->count())
        newRow = 0;
    errorWidget->setCurrentRow(newRow);
}

void ExprEditor::clearErrors()
{
    errorWidget->clear();
    errorWidget->setHidden(true);
    errorHeight = 0;
}

void ExprEditor::clearExtraCompleters()
{
    exprTe->completionModel->clearFunctions();
    exprTe->completionModel->clearVariables();
}

void ExprEditor::registerExtraFunction(const std::string& name, const std::string& docString)
{
    exprTe->completionModel->addFunction(name.c_str(), docString.c_str());
}

void ExprEditor::registerExtraVariable(const std::string& name, const std::string& docString)
{
    exprTe->completionModel->addVariable(name.c_str(), docString.c_str());
}

void ExprEditor::replaceExtras(const ExprCompletionModel& completer)
{
    exprTe->completionModel->syncExtras(completer);
}

void ExprEditor::updateCompleter()
{
    exprTe->completer->setModel(exprTe->completionModel);
}

void ExprEditor::updateStyle()
{
    exprTe->updateStyle();
}
