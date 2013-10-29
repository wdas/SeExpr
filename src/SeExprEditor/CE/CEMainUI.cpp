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
* @file CEMainUI.C
* @brief Contains the Definition of class CEMainUI.
*/

//qt3 #include <qsplitter.h>
//qt3 #include <qlayout.h>

#include <QtCore/QList>
#include <QtGui/QSplitter>
#include <QtGui/QLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>

#include <QtGui/QPushButton>

#include "CECurveListUI.h"
#include "CEGraphUI.h"
#include "CESegEditUI.h"
#include <animlib/AnimCurve.h>
#include "CEMainUI.h"

/**
 * Constructor.
 */
CEMainUI::CEMainUI(QWidget* parent, CETool* tool)
    : QWidget(parent), _tool(tool)
{
    setObjectName("CurveEditor");

    QVBoxLayout* layout = new QVBoxLayout(this);
//qt3    layout->setAutoAdd(true);
    layout->setContentsMargins(4, 4, 4, 4);   // widget border
    layout->setSpacing(4);                    // space between sub-widgets

    // top area is split horizontally, then the left half is split vertically
    QSplitter* hsplit = new QSplitter(this);
    QSplitter* vsplit = new QSplitter(Qt::Vertical, hsplit);

    hsplit->setContentsMargins(0,0,0,0);
    vsplit->setContentsMargins(0,0,0,0);

    // create the sub-widgets
    _curveListUI = new CECurveListUI(vsplit, tool);
    _graphUI     = new CEGraphUI(hsplit, tool);
    _segEditUI   = new CESegEditUI(this, tool);

    _curveListUI->setContentsMargins(0,0,0,0);
    //_segListUI->setContentsMargins(0,0,0,0);
    _segEditUI->setContentsMargins(0,0,0,0);

    // default splitter sizes 
    QList<int> sizes; 
    sizes.push_back(1); sizes.push_back(1);
    hsplit->setSizes(sizes);

    _graphUI->setSizePolicy(QSizePolicy::MinimumExpanding,
                            QSizePolicy::MinimumExpanding);

    vsplit->addWidget(_curveListUI);
    vsplit->addWidget(_segEditUI);
    hsplit->addWidget(vsplit);
    hsplit->addWidget(_graphUI);

    layout->addWidget(hsplit);
    // make splitters resize nicely
//qt3    hsplit->setResizeMode(vsplit, QSplitter::KeepSize);
    hsplit->setStretchFactor(hsplit->indexOf(vsplit), 0);
    hsplit->setStretchFactor(hsplit->indexOf(_graphUI), 1);

    hsplit->setSizes(sizes);

    this->show();
}

/**
 * Destructor.
 */
CEMainUI::~CEMainUI()
{
    // TODO - stash splitter sizes in tool for remapping
}
