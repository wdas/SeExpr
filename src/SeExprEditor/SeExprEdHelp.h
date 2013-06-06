/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
*/
//#include "SeExprEdHelp.h"
#if 0
header

    QTextBrowser* helpBrowser;

constructor

    QWidget* browserspace = new QWidget(tab);
    helpBrowser = new QTextBrowser(browserspace);
    tab->addTab(browserspace, "Help");
    char * path = getenv("RP_SeExpr");
    if (path)
        helpBrowser->setSource(QString("%1/share/doc/SeExpr/SeExpressions.html").arg(path));

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
