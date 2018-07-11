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
* @file ExprBrowser.cpp
* @brief Qt browser widget for list of expressions
* @author  aselle
*/

#include <cassert>

#include <QDir>
#include <QLineEdit>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QTreeView>
#include <QVBoxLayout>

#include "ExprBrowser.h"
#include "ExprEditor.h"
#include "ExprWidgets.h"

#define P3D_CONFIG_ENVVAR "P3D_CONFIG_PATH"

ExprTreeItem::ExprTreeItem(ExprTreeItem* parent, const QString& label, const QString& path)
    : row(-1), parent(parent), label(label), path(path), populated(parent == 0)
{
}

ExprTreeItem::~ExprTreeItem()
{
    for (unsigned int i = 0; i < children.size(); i++)
        delete children[i];
}

ExprTreeItem* ExprTreeItem::find(QString path)
{
    if (this->path == path)
        return this;
    else {
        populate();
        for (unsigned int i = 0; i < children.size(); i++) {
            ExprTreeItem* ret = children[i]->find(path);
            if (ret)
                return ret;
        }
    }
    return 0;
}

void ExprTreeItem::clear()
{
    for (unsigned int i = 0; i < children.size(); i++) {
        delete children[i];
    }
    children.clear();
}

void ExprTreeItem::populate()
{
    if (populated)
        return;
    populated = true;
    QFileInfo info(path);
    if (info.isDir()) {
        QFileInfoList infos = QDir(path).entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

        // std::cerr<<"is dir and populating "<<path.toStdString()<<std::endl;
        for (QList<QFileInfo>::ConstIterator it = infos.constBegin(); it != infos.constEnd(); ++it) {
            const QFileInfo* fi = &*it;
            if (fi->isDir() || fi->fileName().endsWith(".se")) {
                addChild(new ExprTreeItem(this, fi->fileName(), fi->filePath()));
            }
        }
    }
}

void ExprTreeItem::addChild(ExprTreeItem* child)
{
    child->row = children.size();
    children.push_back(child);
}

ExprTreeItem* ExprTreeItem::getChild(const int row)
{
    populate();
    if (row < 0 || row > (int)children.size()) {
        assert(false);
    }
    return children[row];
}

int ExprTreeItem::getChildCount()
{
    populate();
    return children.size();
}

void ExprTreeItem::regen()
{
    std::vector<QString> labels, paths;
    for (unsigned int i = 0; i < children.size(); i++) {
        labels.push_back(children[i]->label);
        paths.push_back(children[i]->path);
        delete children[i];
    }
    children.clear();

    for (unsigned int i = 0; i < labels.size(); i++)
        addChild(new ExprTreeItem(this, labels[i], paths[i]));
}

ExprTreeModel::ExprTreeModel() : root(new ExprTreeItem(0, "", ""))
{
}

ExprTreeModel::~ExprTreeModel()
{
    delete root;
}

void ExprTreeModel::update()
{
    beginResetModel();
    endResetModel();
}

void ExprTreeModel::clear()
{
    beginResetModel();
    root->clear();
    endResetModel();
}

void ExprTreeModel::addPath(const char* label, const char* path)
{
    ExprTreeItem* item = new ExprTreeItem(root, label, path);
    root->addChild(item);
    item->populate();
}

QModelIndex ExprTreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();
    ExprTreeItem* item = (ExprTreeItem*)(index.internalPointer());
    ExprTreeItem* parentItem = item->parent;
    if (parentItem == root)
        return QModelIndex();
    else
        return createIndex(parentItem->row, 0, parentItem);
}

QModelIndex ExprTreeModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();
    else if (!parent.isValid())
        return createIndex(row, column, root->getChild(row));
    else {
        ExprTreeItem* item = (ExprTreeItem*)(parent.internalPointer());
        return createIndex(row, column, item->getChild(row));
    }
}

int ExprTreeModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 1;
}

int ExprTreeModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid())
        return root->getChildCount();
    else {
        ExprTreeItem* item = (ExprTreeItem*)(parent.internalPointer());
        if (!item)
            return root->getChildCount();
        else
            return item->getChildCount();
    }
}

QVariant ExprTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (role != Qt::DisplayRole)
        return QVariant();
    ExprTreeItem* item = (ExprTreeItem*)(index.internalPointer());
    if (!item)
        return QVariant();
    else
        return QVariant(item->label);
}

QModelIndex ExprTreeModel::find(QString path)
{
    ExprTreeItem* item = root->find(path);
    if (!item) {
        beginResetModel();
        root->regen();
        endResetModel();
        item = root->find(path);
    }
    if (item) {
        std::cerr << "found it " << std::endl;
        return createIndex(item->row, 0, item);
    }

    return QModelIndex();
}

ExprTreeFilterModel::ExprTreeFilterModel(QWidget* parent) : QSortFilterProxyModel(parent)
{
}

void ExprTreeFilterModel::update()
{
    beginResetModel();
    endResetModel();
}

bool ExprTreeFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    if (sourceParent.isValid() && sourceModel()->data(sourceParent).toString().contains(filterRegExp()))
        return true;
    QString data = sourceModel()->data(sourceModel()->index(sourceRow, 0, sourceParent)).toString();
    bool keep = data.contains(filterRegExp());

    QModelIndex subIndex = sourceModel()->index(sourceRow, 0, sourceParent);
    if (subIndex.isValid()) {
        for (int i = 0; i < sourceModel()->rowCount(subIndex); ++i)
            keep = keep || filterAcceptsRow(i, subIndex);
    }
    return keep;
}

ExprBrowser::~ExprBrowser()
{
    delete treeModel;
}

ExprBrowser::ExprBrowser(QWidget* parent, ExprEditor* editor)
    : QWidget(parent), editor(editor), _context(""), _searchPath(""), _applyOnSelect(false), _populated(false)
{
    QVBoxLayout* rootLayout = new QVBoxLayout;
    rootLayout->setMargin(0);
    this->setLayout(rootLayout);
    // search and clear widgets
    QHBoxLayout* searchAndClearLayout = new QHBoxLayout();
    searchAndClearLayout->setSpacing(1);
    exprFilter = new QLineEdit();
    connect(exprFilter, SIGNAL(textChanged(const QString&)), SLOT(filterChanged(const QString&)));
    searchAndClearLayout->addWidget(exprFilter, 2);
    QToolButton* clearFilterButton = toolButton(this);
    clearFilterButton->setToolTip("clear filter");
    clearFilterButton->setIcon(QIcon(SEEXPR_EDITOR_ICON_PATH "clearFilter.png"));
    clearFilterButton->setFixedSize(24, 24);
    clearFilterButton->setIconSize(QSize(16, 16));
    searchAndClearLayout->addWidget(clearFilterButton, 1);
    QToolButton* refreshButton = toolButton(this);
    refreshButton->setToolTip("refresh library");
    refreshButton->setIcon(QIcon(SEEXPR_EDITOR_ICON_PATH "reload.png"));
    refreshButton->setFixedSize(24, 24);
    refreshButton->setIconSize(QSize(16, 16));
    refreshButton->setFocusPolicy(Qt::NoFocus);
    searchAndClearLayout->addWidget(refreshButton);
    rootLayout->addLayout(searchAndClearLayout);
    connect(clearFilterButton, SIGNAL(clicked()), SLOT(clearFilter()));
    // model of tree
    treeModel = new ExprTreeModel();
    proxyModel = new ExprTreeFilterModel(this);
    proxyModel->setSourceModel(treeModel);
    // tree widget
    treeNew = new QTreeView;
    treeNew->setModel(proxyModel);
    treeNew->hideColumn(1);
    treeNew->setHeaderHidden(true);
    rootLayout->addWidget(treeNew);
    // selection mode and signal
    treeNew->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(refreshButton, SIGNAL(clicked()), SLOT(reload()));
    connect(treeNew->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
            SLOT(handleSelection(const QModelIndex&, const QModelIndex&)));
    reload();
}

void ExprBrowser::addPath(const std::string& name, const std::string& path)
{
    paths[name] = path;
    treeModel->addPath(name.c_str(), path.c_str());
}

void ExprBrowser::setSearchPath(const QString& context, const QString& path)
{
    _context = context.toStdString();
    _searchPath = path.toStdString();
}

std::string ExprBrowser::getSelectedPath()
{
    QModelIndex sel = treeNew->currentIndex();
    if (sel.isValid()) {
        QModelIndex realCurrent = proxyModel->mapToSource(sel);
        ExprTreeItem* item = (ExprTreeItem*)realCurrent.internalPointer();
        return item->path.toStdString();
    }
    return std::string("");
}

void ExprBrowser::selectPath(const char* path)
{
    QModelIndex index = treeModel->find(path);
    treeNew->setCurrentIndex(proxyModel->mapFromSource(index));
}

void ExprBrowser::update()
{
    treeModel->update();
    proxyModel->update();
}

void ExprBrowser::handleSelection(const QModelIndex& current, const QModelIndex& previous)
{
    Q_UNUSED(previous)
    if (current.isValid()) {
        QModelIndex realCurrent = proxyModel->mapToSource(current);
        ExprTreeItem* item = (ExprTreeItem*)realCurrent.internalPointer();
        QString path = item->path;
        if (path.endsWith(".se")) {
            std::ifstream file(path.toStdString().c_str());
            std::string fileContents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            editor->setExpr(fileContents, _applyOnSelect);
            emit selectionChanged(path);
        }
    }
}

void ExprBrowser::clear()
{
    clearSelection();
    treeModel->clear();
}

void ExprBrowser::clearSelection()
{
    treeNew->clearSelection();
}

void ExprBrowser::clearFilter()
{
    exprFilter->clear();
}

void ExprBrowser::filterChanged(const QString& str)
{
    proxyModel->setFilterRegExp(QRegExp(str));
    proxyModel->setFilterKeyColumn(0);
    if (str != "") {
        treeNew->expandAll();
    } else {
        treeNew->collapseAll();
    }
}

void ExprBrowser::saveExpressionAs()
{
    QString path = QFileDialog::getSaveFileName(this, "Save Expression", QString::fromStdString(_userExprDir), "*.se");

    if (path.length() > 0) {
        std::ofstream file(path.toStdString().c_str());
        if (!file) {
            QString msg = QString("Could not open file %1 for writing").arg(path);
            QMessageBox::warning(this, "Error", QString("<font face=fixed>%1</font>").arg(msg));
            return;
        }
        file << editor->getExpr();
        file.close();

        update();
        selectPath(path.toStdString().c_str());
    }
}

void ExprBrowser::saveLocalExpressionAs()
{
    QString path = QFileDialog::getSaveFileName(this, "Save Expression", QString::fromStdString(_localExprDir), "*.se");

    if (path.length() > 0) {
        std::ofstream file(path.toStdString().c_str());
        if (!file) {
            QString msg = QString("Could not open file %1 for writing").arg(path);
            QMessageBox::warning(this, "Error", QString("<font face=fixed>%1</font>").arg(msg));
            return;
        }
        file << editor->getExpr();
        file.close();

        update();
        selectPath(path.toStdString().c_str());
    }
}

void ExprBrowser::saveExpression()
{
    std::string path = getSelectedPath();
    if (path.length() == 0) {
        saveExpressionAs();
        return;
    }
    std::ofstream file(path.c_str());
    if (!file) {
        QString msg =
            QString("Could not open file %1 for writing.  Is it read-only?").arg(QString::fromStdString(path));
        QMessageBox::warning(this, "Error", QString("<font face=fixed>%1</font>").arg(msg));
        return;
    }
    file << editor->getExpr();
    file.close();
}

void ExprBrowser::expandAll()
{
    treeNew->expandAll();
}

void ExprBrowser::expandToDepth(int depth)
{
    treeNew->expandToDepth(depth);
}

// Location for storing user's expression files
void ExprBrowser::addUserExpressionPath(const std::string& context)
{
    char* homepath = getenv("HOME");
    if (homepath) {
        std::string path = std::string(homepath) + "/" + context + "/expressions/";
        if (QDir(QString(path.c_str())).exists()) {
            _userExprDir = path;
            paths["My Expressions"] = path;
        }
    }
}

void ExprBrowser::reload()
{
    clear();
    populate();
}

/*
 * NOTE: The hard-coded paint3d assumptions can be removed once
 * it (and bonsai?) are adjusted to call setSearchPath(context, path)
 */

void ExprBrowser::populate()
{
    if (_populated)
        return;

    _populated = true;

    const char* env;
    bool enableLocal = false;
    /*bool homeFound = false; -- for xgen's config.txt UserRepo section below */

    if (_searchPath.length() > 0)
        env = _searchPath.c_str();
    else
        env = getenv(P3D_CONFIG_ENVVAR); /* For backwards compatibility */

    if (!env)
        return;

    std::string context;
    if (_context.length() > 0) {
        context = _context;
    } else {
        context = "paint3d"; /* For backwards compatibility */
    }

    std::string configFile = std::string(env) + "/config.txt";
    std::ifstream file(configFile.c_str());
    if (file) {
        std::string key;
        while (file) {
            file >> key;

            if (key[0] == '#') {
                char buffer[1024];
                file.getline(buffer, 1024);
            } else {
                if (key == "ExpressionDir") {
                    std::string label, path;
                    file >> label;
                    file >> path;
                    if (QDir(QString(path.c_str())).exists())
                        paths[label] = path;
                } else if (key == "ExpressionSubDir") {
                    std::string path;
                    file >> path;
                    _localExprDir = path;
                    if (QDir(QString(path.c_str())).exists()) {
                        paths["Local"] = _localExprDir;
                        enableLocal = true;
                    }
                    /* These are for compatibility with xgen.
                     * Long-term, xgen should use the same format.
                     * Longer-term, we should use JSON or something */
                } else if (key == "GlobalRepo") {
                    std::string path;
                    file >> path;
                    path += "/expressions/";
                    if (QDir(QString(path.c_str())).exists())
                        paths["Global"] = path;
                } else if (key == "LocalRepo") {
                    std::string path;
                    file >> path;
                    path += "/expressions/";
                    _localExprDir = path;
                    if (QDir(QString(path.c_str())).exists()) {
                        paths["Local"] = _localExprDir;
                        enableLocal = true;
                    }

                    /*
                     * xgen's config.txt has a "UserRepo" section but we
                     * intentionally ignore it since we already add the user dir
                     * down where the HOME stuff is handled
                     */

                    /*
                    } else if (key == "UserRepo") {
                        std::string path;
                        file>>path;
                        path += "/expressions/";

                        size_t found = path.find("${HOME}");

                        if (found != std::string::npos) {
                            char *homepath = getenv("HOME");
                            if (homepath) {
                                path.replace(found, strlen("${HOME}"), homepath);
                            } else {
                                continue;
                            }
                        }
                        if(QDir(QString(path.c_str())).exists()){
                            paths["User"] = path;
                            homeFound = true;
                        }
                    */
                } else {
                    char buffer[1024];
                    file.getline(buffer, 1024);
                }
            }
        }
    }
    addUserExpressionPath(context);

    for (const auto& pair : paths) {
        treeModel->addPath(pair.first.c_str(), pair.second.c_str());
    }

    update();
    return;
}
