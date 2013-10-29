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
* @file CECurveListUI.cpp
* @brief Contains the Definition of class CECurveListUI.
*/

/*qt3
#include <qlabel.h>
#include <qlistbox.h>
#include <qlayout.h>
*/

#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>
#include <QtGui/QLayout>

#include "CETool.h"
#include "CECurveListUI.h"

/**
 * Constructor.
 */
CECurveListUI::CECurveListUI(QWidget* parent, CETool* tool) :
    QWidget(parent), _tool(tool),
    _listValid(0), _selValid(0), _updating(0)
{
    setObjectName("CurveList");

    QVBoxLayout* layout = new QVBoxLayout(this);
//qt3    layout->setAutoAdd(true);
    
    QLabel* label = new QLabel("Curves", this);
    _list = new QListWidget(this);
    _list->setSelectionMode(QAbstractItemView::ExtendedSelection);

    layout->addWidget(label);
    layout->addWidget(_list);

    connect(_tool, SIGNAL(curveListChanged()), SLOT(invalidateCurveList()));
    connect(_tool, SIGNAL(selectionChanged()), SLOT(invalidateSelection()));
    connect(_list, SIGNAL(itemSelectionChanged()), SLOT(handleSelectionChanged()));

    doUpdate();
}

/**
 * Destructor.
 */
CECurveListUI::~CECurveListUI()
{
}

    
void
CECurveListUI::invalidateCurveList()
{
    if (_listValid || _selValid) {
        _listValid = 0;
        _selValid = 0;
        update();
    }
}


void
CECurveListUI::invalidateSelection()
{
    if (_updating) return; // prevent circular update!
    if (_selValid) {
        _selValid = 0;
        update();
    }
}


void
CECurveListUI::paintEvent(QPaintEvent* event)
{
    if (!_listValid || !_selValid) doUpdate();
    QWidget::paintEvent(event);
}


void
CECurveListUI::showEvent(QShowEvent* event)
{
    if (!_listValid || !_selValid) doUpdate();
    QWidget::showEvent(event);
}


void
CECurveListUI::doUpdate()
{
    _updating = 1;

    if (!_listValid) {
        // update listbox contents from tool
        std::vector<std::string> names;
        _tool->getCurveNames(names);
        int i;
        for (i = 0; i < (int)names.size(); i++) {
            const char* name = names[i].c_str();
            if (i >= (int) _list->count())
                _list->addItem(name);
//qt3            else if (_list->text(i) != name)
            else if (_list->currentItem() && 
                     _list->currentItem()->text() != name)
            {
//qt3                _list->changeItem(name, i);
                QListWidgetItem* curItem = _list->currentItem();
                curItem->setText(name);
            }
        }
        while ((int) _list->count() > i)
//qt3            _list->removeItem(_list->count()-1);
            _list->takeItem( _list->count()-1 );

    }
    if (!_listValid || !_selValid) {
        // update listbox's selection from tool
        msg::list sel;
        _tool->getSelection(sel);
        _list->clearSelection(); // TODO: will this emit selectionChanged() like the Qt3 version?
        for (int i = 0; i < sel.size(); i+=2)
//qt3            _list->setSelected(sel[i], true);
            _list->setCurrentRow(sel[i]); // TODO: I'm pretty sure sel[i] will be an int
    }
    _selValid = 1;
    _listValid = 1;
    _updating = 0;
}

void
CECurveListUI::handleSelectionChanged()
{
    if (_updating) return; // prevent circular update!
    _updating = 1;

    // first count selections
    int num = 0;
    int curve=0;
    int i;
    for (i = 0; i < (int) _list->count(); i++)
    {
        QListWidgetItem* curItem = _list->item(i);
//qt3        if (_list->isSelected(i)) { num++; curve = i; }
        if ( curItem && curItem->isSelected() ) { num++; curve = i; }
    }

    // update tool's selection list from listbox
    if (num == 0) 
        _tool->clearSelection();
    else if (num == 1)
        _tool->selectCurve(curve);
    else {
        msg::list selections(num);
        int n = 0;
        for (i = 0; i < (int) _list->count(); i++)
        {    
//qt3            if (_list->isSelected(i))
            QListWidgetItem* curItem = _list->item(i);
            if (curItem->isSelected()) selections.set(n++, i);
        }
        _tool->selectCurves(selections);
    }
    _updating = 0;
}
