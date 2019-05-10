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
* @file ExprBrowser.h
* @brief Browser for a library of expressions from a tree of files
* @author  aselle
*/
#ifndef ExprBrowser_h
#define ExprBrowser_h

#include <atomic>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <vector>

#include <QAbstractItemModel>
#include <QFuture>
#include <QFutureWatcher>
#include <QSortFilterProxyModel>
#include <QWidget>

class QLineEdit;
class QTreeView;

namespace SeExpr2 {

class ExprEditor;

class ExprTreeItem {
  public:
    ExprTreeItem(ExprTreeItem* parent, const QString& label, const QString& path);
    ~ExprTreeItem();

    ExprTreeItem* find(QString path);
    void clear();
    void populate(std::atomic<bool>& cancelRequested);
    void addChild(ExprTreeItem* child);
    ExprTreeItem* getChild(const int row);
    int getChildCount();
    void regen();

    int row;
    ExprTreeItem* parent;
    QString label;
    QString path;

  private:
    std::vector<ExprTreeItem*> children;
    std::atomic<bool> populated;
};

class ExprTreeModel : public QAbstractItemModel {
    Q_OBJECT

    ExprTreeItem* root;

  public:
    ExprTreeModel();
    ~ExprTreeModel();

    void populate();
    void clear();
    void addPath(const char* label, const char* path);
    QModelIndex parent(const QModelIndex& index) const;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    QModelIndex find(QString path);

  signals:
    void updated();

  public slots:
    void update();

  public:
    std::atomic<bool> cancelRequested;
    std::vector<QFuture<void>> futures;
    std::vector<std::unique_ptr<QFutureWatcher<void>>> watchers;
};

class ExprTreeFilterModel : public QSortFilterProxyModel {
    Q_OBJECT

  public:
    ExprTreeFilterModel(QWidget* parent = 0);

    void update();
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const;
};

class ExprBrowser : public QWidget {
    Q_OBJECT

    ExprEditor* editor;
    std::unordered_map<std::string, std::string> paths;
    ExprTreeModel* treeModel;
    ExprTreeFilterModel* proxyModel;
    QTreeView* treeNew;
    QLineEdit* exprFilter;
    std::string _userExprDir;
    std::string _localExprDir;
    std::string _context;
    std::string _searchPath;
    bool _applyOnSelect;
    bool _populated;

  public:
    ExprBrowser(QWidget* parent, ExprEditor* editor);
    ~ExprBrowser();
    void addPath(const std::string& name, const std::string& path);
    std::string getSelectedPath();
    void selectPath(const char* path);
    void addUserExpressionPath(const std::string& context);
    void setSearchPath(const QString& context, const QString& path);
    void expandAll();
    void expandToDepth(int depth);
    void setApplyOnSelect(bool on)
    {
        _applyOnSelect = on;
    }

  signals:
    void selectionChanged(const QString& str);

  public slots:
    void reload();
    void populate();
    void handleSelection(const QModelIndex& current, const QModelIndex& previous);
    void update();
    void clear();
    void clearSelection();
    void saveExpression();
    void saveLocalExpressionAs();
    void saveExpressionAs();
  private slots:
    void modelUpdatedSLOT();
    void clearFilter();
    void filterChanged(const QString& str);
};
}

#endif
