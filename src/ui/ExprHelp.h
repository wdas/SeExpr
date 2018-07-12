/*
Copyright Disney Enterprises, Inc.  All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License
and the following modification to it: Section 6 Trademarks.
deleted and replaced with:

6. Trademarks. This License does not grant permission to use the
trade names, trademarks, service marks, or product names of the
Licensor and its affiliates, except as required for reproducing
the content of the NOTICE file.

You may obtain a copy of the License at
http://www.apache.org/licenses/LICENSE-2.0
*/

#include <QLineEdit>
#include <QTextBrowser>
#include <QTextDocument>
#include <QWidget>

class ExprHelp : public QWidget {
    Q_OBJECT

  public:
    ExprHelp(QWidget* parent = nullptr);

  protected:
    void findHelper(QTextDocument::FindFlags flags);

  private slots:
    void findNextInHelp();
    void findPrevInHelp();

  private:
    QLineEdit* helpFindBox;
    QTextBrowser* helpBrowser;
    QString prevFind;
};