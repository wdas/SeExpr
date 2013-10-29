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
* @file SeExprEditor.cpp
* @brief This provides an exression editor for SeExpr syntax with auto ui features
* @author  aselle
*/
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtCore/QRegExp>
#include <QtGui/QSplitter>
#include <QtGui/QLabel>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>
#include <QtGui/QScrollArea>
#include <QtGui/QSpacerItem>
#include <QtGui/QSizePolicy>
#include <QtGui/QTextCharFormat>
#include <QtGui/QCompleter>
#include <QtGui/QAbstractItemView>
#include <QtGui/QStandardItemModel>
#include <QtGui/QStringListModel>
#include <QtGui/QScrollBar>
#include <QtGui/QToolTip>
#include <QtGui/QListWidget>
#include <QtGui/QTreeView>
#include <QtGui/QAction>
#include <QtGui/QMenu>

#include <SeExpression.h>
#include <SeExprNode.h>
#include <SeExprFunc.h>
#include <SeExprBuiltins.h>

#include "SeExprEditor.h"
#include "SeExprEdHighlighter.h"
#include "SeExprEdCompletionModel.h"
#include "SeExprEdControlCollection.h"
#include "SeExprEdCurve.h"
#include "SeExprEdColorCurve.h"
#include "SeExprEdControl.h"
#include "SeExprEdPopupDocumentation.h"

SeExprEdLineEdit::SeExprEdLineEdit(int id, QWidget* parent)
: QLineEdit(parent), _id(id), _signaling(0)
{
    connect(this, SIGNAL(textChanged(const QString &)), SLOT(textChangedCB(const QString &)));
}


void SeExprEdLineEdit::textChangedCB(const QString& text)
{
    _signaling = 1;
    emit textChanged(_id, text);
    _signaling = 0;
}

void SeExprEditor::controlChanged(int id)
{
    QString newText=exprTe->toPlainText();
    controls->updateText(id,newText);
    _updatingText=1;
    exprTe->selectAll();
    exprTe->insertPlainText(newText);
    //exprTe->setPlainText(newText);
    _updatingText=0;

    // schedule preview update
    previewTimer->setSingleShot(true);
    previewTimer->start(0);
}

SeExprEditor::~SeExprEditor()
{
    delete controlRebuildTimer;
    delete previewTimer;
}

SeExprEdExpressionTextEdit::~SeExprEdExpressionTextEdit()
{}

SeExprEditor::SeExprEditor(QWidget* parent,SeExprEdControlCollection* controls)
    : QWidget(parent),_updatingText(0),errorHeight(0)
{
    // timers
    controlRebuildTimer=new QTimer();
    previewTimer=new QTimer();

    // title and minimum size
    setWindowTitle("Expression Editor");
    setMinimumHeight(100);

    // expression controls, we need for signal connections
    this->controls = controls;

    // make layout
    QVBoxLayout* exprAndErrors=new QVBoxLayout;
    exprAndErrors->setMargin(0);
    setLayout(exprAndErrors);

    // create text editor widget
    exprTe = new SeExprEdExpressionTextEdit(this);
    exprTe->setMinimumHeight(50);
    
    // calibrate the font size
    int fontsize = 12;
    while (QFontMetrics(QFont("Liberation Sans",fontsize)).width("abcdef")<38 && fontsize<20) 
	fontsize++;
    while (QFontMetrics(QFont("Liberation Sans",fontsize)).width("abcdef")>44 && fontsize>3) 
	fontsize--;
    
    exprTe->setFont(QFont("Liberation Sans",fontsize));
    
    exprAndErrors->addWidget(exprTe);

    // create error widget
    errorWidget=new QListWidget();
    errorWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    errorWidget->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Maximum));
    connect(errorWidget,SIGNAL(itemSelectionChanged()),SLOT(selectError()));
    clearErrors();
    exprAndErrors->addWidget(errorWidget);

    // wire up signals
    connect(exprTe, SIGNAL(applyShortcut()), SLOT(sendApply()));
    connect(exprTe, SIGNAL(nextError()), SLOT(nextError()));
    connect(exprTe, SIGNAL(textChanged()), SLOT(exprChanged()));
    connect(controls,SIGNAL(controlChanged(int)),SLOT(controlChanged(int)));
    connect(controls,SIGNAL(insertString(const std::string&)),SLOT(insertStr(const std::string&)));
    connect(controlRebuildTimer, SIGNAL(timeout()), SLOT(rebuildControls()));
    connect(previewTimer, SIGNAL(timeout()), SLOT(sendPreview()));
}

void SeExprEditor::selectError()
{
    int selected=errorWidget->currentRow();
    QListWidgetItem* item=errorWidget->item(selected);
    int start=item->data(Qt::UserRole).toInt();
    int end=item->data(Qt::UserRole+1).toInt();
    QTextCursor cursor=exprTe->textCursor();
    cursor.movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,start);
    cursor.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,end-start+1);
    exprTe->setTextCursor(cursor);
}

void SeExprEditor::sendApply()
{
    emit apply();
}

void SeExprEditor::sendPreview()
{
    emit preview();
}

void SeExprEditor::exprChanged()
{
    if (_updatingText) return;

    // schedule control rebuild
    controlRebuildTimer->setSingleShot(true);
    controlRebuildTimer->start(0);
}

void SeExprEditor::rebuildControls()
{
    bool wasShown=!exprTe->completer->popup()->isHidden();
    bool newVariables=controls->rebuildControls(exprTe->toPlainText(),exprTe->completionModel->local_variables);
    if(newVariables) exprTe->completer->setModel(exprTe->completionModel);
    if(wasShown) exprTe->completer->popup()->show();
}

void SeExprEdExpressionTextEdit::updateStyle()
{
    lastStyleForHighlighter=0;
    highlighter->fixStyle(palette());
    highlighter->rehighlight();
    repaint();
}

SeExprEdExpressionTextEdit::SeExprEdExpressionTextEdit(QWidget* parent)
    :QTextEdit(parent),lastStyleForHighlighter(0),_tip(0)
{
    highlighter=new SeExprEdHighlighter(document());

    // setup auto completion
    completer=new QCompleter();
    completionModel=new SeExprEdCompletionModel(this);
    completer->setModel(completionModel);
    QTreeView* treePopup=new QTreeView;
    completer->setPopup(treePopup);
    treePopup->setRootIsDecorated(false);
    treePopup->setMinimumWidth(300);
    treePopup->setMinimumHeight(50);
    treePopup->setItemsExpandable(true);

    completer->setWidget(this);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    QObject::connect(completer,SIGNAL(activated(const QString&)),this,
        SLOT(insertCompletion(const QString&)));
	
    _popupEnabledAction = new QAction("Pop-up Help", this);
    _popupEnabledAction->setCheckable(true);
    _popupEnabledAction->setChecked(true);
}

void SeExprEdExpressionTextEdit::focusInEvent(QFocusEvent* e)
{
    if(completer) completer->setWidget(this);
    QTextEdit::focusInEvent(e);
}

void SeExprEdExpressionTextEdit::focusOutEvent(QFocusEvent* e)
{
    hideTip();
    QTextEdit::focusInEvent(e);
}

void SeExprEdExpressionTextEdit::mousePressEvent(QMouseEvent* event)
{
    hideTip();
    QTextEdit::mousePressEvent(event);
}

void SeExprEdExpressionTextEdit::mouseDoubleClickEvent(QMouseEvent* event)
{
    hideTip();
    QTextEdit::mouseDoubleClickEvent(event);
}

void SeExprEdExpressionTextEdit::paintEvent(QPaintEvent* event)
{
    if(lastStyleForHighlighter!=style()){
        lastStyleForHighlighter=style();
        highlighter->fixStyle(palette());
        highlighter->rehighlight();
    }
    QTextEdit::paintEvent(event);
}

void SeExprEdExpressionTextEdit::wheelEvent(QWheelEvent* event)
{
    if(event->modifiers() == Qt::ControlModifier){
        if(event->delta()>0) zoomIn();
        else if(event->delta()<0) zoomOut();
    }
    return QTextEdit::wheelEvent(event);
}

void SeExprEdExpressionTextEdit::keyPressEvent( QKeyEvent* e )
{
    // Accept expression
    if (e->key()==Qt::Key_Return && e->modifiers()==Qt::ControlModifier){
        emit applyShortcut();
        return ;
    }else if(e->key()==Qt::Key_F4){
        emit nextError();
        return;
    }

    // If the completer is active pass keys it needs down
    if(completer && completer->popup()->isVisible()){
        switch(e->key()){
            case Qt::Key_Enter:case Qt::Key_Return:case Qt::Key_Escape:
            case Qt::Key_Tab:case Qt::Key_Backtab:
                e->ignore();return;
            default:
                break;
        }
    }

    // use the values here as long as we are not using the shortcut to bring up the editor
    bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
    if (!isShortcut) // dont process the shortcut when we have a completer
      QTextEdit::keyPressEvent(e);
    
    const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (!completer || (ctrlOrShift && e->text().isEmpty()))
        return;

    bool hasModifier=(e->modifiers()!=Qt::NoModifier) && !ctrlOrShift;

    // grab the line we're on 
    QTextCursor tc=textCursor();
    tc.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
    QString line=tc.selectedText();

    // matches the last prefix of a completable variable or function and extract as completionPrefix
    static QRegExp completion("^(?:.*[^A-Za-z0-9_$])?((?:\\$[A-Za-z0-9_]*)|[A-Za-z]+[A-Za-z0-9_]*)$");
    int index=completion.indexIn(line);
    QString completionPrefix;
    if(index != -1 && !line.contains('#')){
        completionPrefix=completion.cap(1);
        //std::cout<<"we have completer prefix '"<<completionPrefix.toStdString()<<"'"<<std::endl;
    }

    // hide the completer if we have too few characters, we are at end of word
    if(!isShortcut && (hasModifier || e->text().isEmpty() || completionPrefix.length()<1 || index==-1)){
        completer->popup()->hide();
    }else if (_popupEnabledAction->isChecked()) {
        
        // copy the completion prefix in if we don't already have it in the completer
        if(completionPrefix!=completer->completionPrefix()){
            completer->setCompletionPrefix(completionPrefix);
            completer->popup()->setCurrentIndex(completer->completionModel()->index(0,0));
        }
        
        // display the completer
       QRect cr=cursorRect();
       cr.setWidth(completer->popup()->sizeHintForColumn(0)+completer->popup()->sizeHintForColumn(1)
                + completer->popup()->verticalScrollBar()->sizeHint().width());
        cr.translate(0,6);
        completer->complete(cr);
        hideTip();
        return;
    }

    // documentation completion
    static QRegExp inFunction("^(?:.*[^A-Za-z0-9_$])?([A-Za-z0-9_]+)\\([^()]*$");
    int index2=inFunction.indexIn(line);
    if(index2!=-1){
        QString functionName=inFunction.cap(1);
        QStringList tips=completionModel->getDocString(functionName).split("\n");
        QString tip="<b>"+tips[0]+"</b>";
        for(int i=1;i<tips.size();i++){
            tip+="<br>"+tips[i];
        }
        if (_popupEnabledAction->isChecked()) showTip(tip);
        //QToolTip::showText(mapToGlobal(cr.bottomLeft()),tip,this,cr);
    }else{
        hideTip();
    }
}

void SeExprEdExpressionTextEdit::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createStandardContextMenu();
    
    if (!menu->actions().empty())
    {
	QAction* f = menu->actions().first();
	menu->insertAction(f, _popupEnabledAction);
	menu->insertSeparator(f);
    }
    
    menu->exec(event->globalPos());
    delete menu;
}

void SeExprEdExpressionTextEdit::showTip(const QString& string)
{
    // skip empty strings
    if(string=="") return;
    // skip already shown stuff
    if(_tip && !_tip->isHidden() && _tip->label->text() == string) return;

    QRect cr=cursorRect();
    cr.setX(0);
    cr.setWidth(cr.width()*3);
    if(_tip){delete _tip;_tip=0;}
    _tip=new SeExprEdPopupDocumentation(this,mapToGlobal(cr.bottomLeft())+QPoint(0,6),string);
}

void SeExprEdExpressionTextEdit::hideTip()
{
    if(_tip) _tip->hide();
}

void SeExprEdExpressionTextEdit::insertCompletion(const QString &completion)
{
    if (completer->widget() != this) return;
    QTextCursor tc = textCursor();
    int extra = completion.length() - completer->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    if(completion[0]!='$') tc.insertText("(");
    setTextCursor(tc);
}

std::string SeExprEditor::getExpr()
{
    return exprTe->toPlainText().toStdString();
}

void SeExprEditor::setExpr(const std::string& expression,const bool doApply)
{
    //exprTe->clear();
    exprTe->selectAll();
    exprTe->insertPlainText(QString::fromStdString(expression));
    clearErrors();
    exprTe->moveCursor(QTextCursor::Start);
    if(doApply) emit apply();
}

void SeExprEditor::insertStr(const std::string& str)
{
    exprTe->insertPlainText(QString::fromStdString(str));
}

void SeExprEditor::appendStr(const std::string& str)
{
    exprTe->append(QString::fromStdString(str));
}

void SeExprEditor::addError(const int startPos,const int endPos,const std::string& error)
{
    QListWidgetItem* item=new QListWidgetItem(("Error: "+error).c_str(),errorWidget);
    item->setData(Qt::UserRole,startPos);
    item->setData(Qt::UserRole+1,endPos);
    errorWidget->setHidden(false);
    // TODO: fix to not use count lines and compute heuristic of 25 pixels per line!
    const char* c=error.c_str();
    int lines=1;
    while(*c != '\0'){
        if(*c == '\n') lines++;
        c++;
    }
    errorHeight+=25*lines;
    // widget should not need to be bigger than this
    errorWidget->setMaximumHeight(errorHeight);
}

void SeExprEditor::nextError()
{
    int newRow=errorWidget->currentRow()+1;
    if(newRow>=errorWidget->count()) newRow=0;
    errorWidget->setCurrentRow(newRow);
}

void SeExprEditor::clearErrors()
{
    errorWidget->clear();
    errorWidget->setHidden(true);
    errorHeight=0;
}

void SeExprEditor::clearExtraCompleters()
{
    exprTe->completionModel->clearFunctions();
    exprTe->completionModel->clearVariables();
}

void SeExprEditor::registerExtraFunction(const std::string& name,const std::string& docString)
{
    exprTe->completionModel->addFunction(name.c_str(),docString.c_str());
}

void SeExprEditor::registerExtraVariable(const std::string& name,const std::string& docString)
{
    exprTe->completionModel->addVariable(name.c_str(),docString.c_str());
}

void SeExprEditor::replaceExtras(const SeExprEdCompletionModel& completer)
{
    exprTe->completionModel->syncExtras(completer);
}

void SeExprEditor::updateCompleter()
{
    exprTe->completer->setModel(exprTe->completionModel);
}

void SeExprEditor::updateStyle()
{
    exprTe->updateStyle();
}

