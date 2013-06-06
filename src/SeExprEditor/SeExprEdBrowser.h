/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
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

public:
    SeExprEdBrowser(QWidget* parent, SeExprEditor* editor);
    ~SeExprEdBrowser();
    void addPath(const std::string& name,const std::string& path);
    std::string getSelectedPath();
    void selectPath(const char * path);
    bool getExpressionDirs();
    bool getExpressionDirs(const std::string& context);
    void setSearchPath(const QString& context, const QString& path);
    void expandAll();
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
