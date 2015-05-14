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
* @file SeExprEdBrowser.h
* @brief Browser for a library of expressions from a tree of files
* @author  aselle
*/
#ifndef SeExprEdBrowser_h
#define SeExprEdBrowser_h


#include <QtGui/QWidget>
#include <QtCore/QAbstractItemModel>

#include <iostream>
#include <fstream>
#include <sstream>

class QLineEdit;
class QTreeWidget;
class QTreeView;
class QTreeWidgetItem;
class QTextBrowser;
class SeExprEditor;
class QSortFilterProxyModel;
class QDir;

class SeExprEdTreeModel;
class SeExprEdTreeFilterModel;

class SeExprEdBrowser : public QWidget
{
    Q_OBJECT

    SeExprEditor* editor;
    QList<QString> labels;
    QList<QString> paths;
    SeExprEdTreeModel* treeModel;
    SeExprEdTreeFilterModel* proxyModel;
    QTreeView* treeNew;
    QLineEdit* exprFilter;
    std::string _userExprDir;
    std::string _localExprDir;
    std::string _context;
    std::string _searchPath;
    bool _applyOnSelect;

public:
    SeExprEdBrowser(QWidget* parent, SeExprEditor* editor);
    ~SeExprEdBrowser();
    void addPath(const std::string& name,const std::string& path);
    std::string getSelectedPath();
    void selectPath(const char * path);
    void addUserExpressionPath(const std::string &context);
    bool getExpressionDirs();
    bool getExpressionDirs(const std::string& context);
    void setSearchPath(const QString& context, const QString& path);
    void expandAll();
    void expandToDepth(int depth);
    void setApplyOnSelect(bool on) {_applyOnSelect = on; }
public slots:
    void handleSelection(const QModelIndex& current,const QModelIndex& previous);
    void update();
    void clear();
    void clearSelection();
    void saveExpression();
    void saveLocalExpressionAs();
    void saveExpressionAs();
private slots:
    void clearFilter();
    void filterChanged(const QString& str);

};

#endif
