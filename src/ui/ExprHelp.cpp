#include "ExprHelp.h"

#include <QApplication>
#include <QDir>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

using namespace SeExpr2;

ExprHelp::ExprHelp(QWidget* parent) : QWidget(parent)
{
    helpBrowser = new QTextBrowser(this);

    // Locate help docs relative to location of the app itself
    QFile* helpDoc = new QFile(QCoreApplication::applicationDirPath() + "/../share/doc/SeExpr2/SeExpressions.html");
    if (helpDoc->exists()) {
        QString sheet =
            "body {background-color: #eeeeee; color: #000000;} \na {color: #3333ff; text-decoration: none;}\n";
        helpBrowser->document()->setDefaultStyleSheet(sheet);
        helpBrowser->setSource(helpDoc->fileName());
    }

    QPushButton* backPb = new QPushButton("Back");
    backPb->setEnabled(false);
    QPushButton* forwardPb = new QPushButton("Forward");
    forwardPb->setEnabled(false);

    QVBoxLayout* helpLayout = new QVBoxLayout(this);
    QHBoxLayout* helpPbLayout = new QHBoxLayout;
    helpLayout->addLayout(helpPbLayout);
    helpPbLayout->addWidget(backPb);
    helpPbLayout->addWidget(forwardPb);
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

void ExprHelp::findHelper(QTextDocument::FindFlags flags)
{
    QTextDocument* doc = helpBrowser->document();
    if (prevFind != helpFindBox->text()) {
        prevFind = helpFindBox->text();
        helpBrowser->setTextCursor(QTextCursor(doc));
    }
    QTextCursor blah = doc->find(helpFindBox->text(), helpBrowser->textCursor(), flags);
    helpBrowser->setTextCursor(blah);
}

void ExprHelp::findNextInHelp()
{
    findHelper(0);
}

void ExprHelp::findPrevInHelp()
{
    findHelper(QTextDocument::FindBackward);
}
