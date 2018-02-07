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
*/
#if 0
header

    QTextBrowser* helpBrowser;

constructor

    QWidget* browserspace = new QWidget(tab);
    helpBrowser = new QTextBrowser(browserspace);
    tab->addTab(browserspace, "Help");
    char * path = getenv("RP_SeExpr2");
    if (path)
        helpBrowser->setSource(QString("%1/share/doc/SeExpr2/SeExpressions.html").arg(path));

    QPushButton* backPb = new QPushButton("Back");
    backPb->setEnabled(false);
    QPushButton* forwardPb = new QPushButton("Forward");
    forwardPb->setEnabled(false);

    QVBoxLayout * helpLayout = new QVBoxLayout(browserspace);
    QHBoxLayout * helpPbLayout = new QHBoxLayout;
    helpLayout->addLayout(helpPbLayout);
    helpPbLayout->addWidget(backPb);
    helpPbLayout->addWidget(forwardPb);
    helpPbLayout->addItem(new QSpacerItem(0,0, QSizePolicy::MinimumExpanding,
                QSizePolicy::Minimum));
    helpLayout->addWidget(helpBrowser, /*stretch*/ true);
    helpBrowser->setMinimumHeight(120);

    // wire up help browser forward/back buttons
    connect(backPb, SIGNAL(clicked()), helpBrowser, SLOT(backward()));
    connect(forwardPb, SIGNAL(clicked()), helpBrowser, SLOT(forward()));
    connect(helpBrowser, SIGNAL(backwardAvailable(bool)), backPb, SLOT(setEnabled(bool)));
    connect(helpBrowser, SIGNAL(forwardAvailable(bool)), forwardPb, SLOT(setEnabled(bool)));
#endif
