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
#include <QDir>
#include <QFileInfo>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QHeaderView>
#include <QLabel>
#include <QTextBrowser>
#include <QPushButton>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QSortFilterProxyModel>
#include <QFileDialog>
#include <QMessageBox>

#include <cassert>
#include "ExprEditor.h"
#include "ExprBrowser.h"

#define P3D_CONFIG_ENVVAR "P3D_CONFIG_PATH"

class ExprTreeItem {
  public:
    ExprTreeItem(ExprTreeItem* parent, const QString& label, const QString& path)
        : row(-1), parent(parent), label(label), path(path), populated(parent == 0) {}

    ~ExprTreeItem() {
        for (unsigned int i = 0; i < children.size(); i++) delete children[i];
    }

    ExprTreeItem* find(QString path) {
        if (this->path == path)
            return this;
        else {
            populate();
            for (unsigned int i = 0; i < children.size(); i++) {
                ExprTreeItem* ret = children[i]->find(path);
                if (ret) return ret;
            }
        }
        return 0;
    }

    void clear() {
        for (unsigned int i = 0; i < children.size(); i++) {
            delete children[i];
        }
        children.clear();
    }

    void populate() {
        if (populated) return;
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

    void addChild(ExprTreeItem* child) {
        child->row = children.size();
        children.push_back(child);
    }

    ExprTreeItem* getChild(const int row) {
        populate();
        if (row < 0 || row > (int)children.size()) {
            assert(false);
        }
        return children[row];
    }

    int getChildCount() {
        populate();
        return children.size();
    }

    void regen() {
        std::vector<QString> labels, paths;
        for (unsigned int i = 0; i < children.size(); i++) {
            labels.push_back(children[i]->label);
            paths.push_back(children[i]->path);
            delete children[i];
        }
        children.clear();

        for (unsigned int i = 0; i < labels.size(); i++) addChild(new ExprTreeItem(this, labels[i], paths[i]));
    }

    int row;
    ExprTreeItem* parent;
    QString label;
    QString path;

  private:
    std::vector<ExprTreeItem*> children;
    bool populated;
};

class ExprTreeModel : public QAbstractItemModel {
    ExprTreeItem* root;

  public:
    ExprTreeModel() : root(new ExprTreeItem(0, "", "")) {}

    ~ExprTreeModel() { delete root; }

    void update()
    {
        beginResetModel();
        endResetModel();
    }

    void clear() {
        beginResetModel();
        root->clear();
        endResetModel();
    }

    void addPath(const char* label, const char* path) { root->addChild(new ExprTreeItem(root, label, path)); }

    QModelIndex parent(const QModelIndex& index) const {
        if (!index.isValid()) return QModelIndex();
        ExprTreeItem* item = (ExprTreeItem*)(index.internalPointer());
        ExprTreeItem* parentItem = item->parent;
        if (parentItem == root)
            return QModelIndex();
        else
            return createIndex(parentItem->row, 0, parentItem);
    }

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const {
        if (!hasIndex(row, column, parent))
            return QModelIndex();
        else if (!parent.isValid())
            return createIndex(row, column, root->getChild(row));
        else {
            ExprTreeItem* item = (ExprTreeItem*)(parent.internalPointer());
            return createIndex(row, column, item->getChild(row));
        }
    }

    int columnCount(const QModelIndex& parent) const {
        Q_UNUSED(parent);
        return 1;
    }

    int rowCount(const QModelIndex& parent = QModelIndex()) const {
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

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const {
        if (!index.isValid()) return QVariant();
        if (role != Qt::DisplayRole) return QVariant();
        ExprTreeItem* item = (ExprTreeItem*)(index.internalPointer());
        if (!item)
            return QVariant();
        else
            return QVariant(item->label);
    }

    QModelIndex find(QString path) {
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
};

class ExprTreeFilterModel : public QSortFilterProxyModel {
  public:
    ExprTreeFilterModel(QWidget* parent = 0) : QSortFilterProxyModel(parent) {}

    void update()
    {
        beginResetModel();
        endResetModel();
    }

    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const {
        if (sourceParent.isValid() && sourceModel()->data(sourceParent).toString().contains(filterRegExp()))
            return true;
        QString data = sourceModel()->data(sourceModel()->index(sourceRow, 0, sourceParent)).toString();
        bool keep = data.contains(filterRegExp());

        QModelIndex subIndex = sourceModel()->index(sourceRow, 0, sourceParent);
        if (subIndex.isValid()) {
            for (int i = 0; i < sourceModel()->rowCount(subIndex); ++i) keep = keep || filterAcceptsRow(i, subIndex);
        }
        return keep;
    }
};

ExprBrowser::~ExprBrowser() { delete treeModel; }

ExprBrowser::ExprBrowser(QWidget* parent, ExprEditor* editor)
    : QWidget(parent), editor(editor), _context(""), _searchPath(""), _applyOnSelect(true) {
    QVBoxLayout* rootLayout = new QVBoxLayout;
    rootLayout->setMargin(0);
    this->setLayout(rootLayout);
    // search and clear widgets
    QHBoxLayout* searchAndClearLayout = new QHBoxLayout();
    exprFilter = new QLineEdit();
    connect(exprFilter, SIGNAL(textChanged(const QString&)), SLOT(filterChanged(const QString&)));
    searchAndClearLayout->addWidget(exprFilter, 2);
    QPushButton* clearFilterButton = new QPushButton("X");
    clearFilterButton->setFixedWidth(24);
    searchAndClearLayout->addWidget(clearFilterButton, 1);
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
    connect(treeNew->selectionModel(),
            SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
            SLOT(handleSelection(const QModelIndex&, const QModelIndex&)));
}

void ExprBrowser::addPath(const std::string& name, const std::string& path) {
    labels.append(QString::fromStdString(name));
    paths.append(QString::fromStdString(path));
    treeModel->addPath(name.c_str(), path.c_str());
}

void ExprBrowser::setSearchPath(const QString& context, const QString& path) {
    _context = context.toStdString();
    _searchPath = path.toStdString();
}

std::string ExprBrowser::getSelectedPath() {
    QModelIndex sel = treeNew->currentIndex();
    if (sel.isValid()) {
        QModelIndex realCurrent = proxyModel->mapToSource(sel);
        ExprTreeItem* item = (ExprTreeItem*)realCurrent.internalPointer();
        return item->path.toStdString();
    }
    return std::string("");
}

void ExprBrowser::selectPath(const char* path) {
    QModelIndex index = treeModel->find(path);
    treeNew->setCurrentIndex(proxyModel->mapFromSource(index));
}

void ExprBrowser::update() {
    treeModel->update();
    proxyModel->update();
}

void ExprBrowser::handleSelection(const QModelIndex& current, const QModelIndex& previous) {
    Q_UNUSED(previous)
    if (current.isValid()) {
        QModelIndex realCurrent = proxyModel->mapToSource(current);
        ExprTreeItem* item = (ExprTreeItem*)realCurrent.internalPointer();
        QString path = item->path;
        if (path.endsWith(".se")) {
            std::ifstream file(path.toStdString().c_str());
            std::string fileContents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            editor->setExpr(fileContents, _applyOnSelect);
        }
    }
}

void ExprBrowser::clear() {
    labels.clear();
    paths.clear();
    clearSelection();

    treeModel->clear();
}

void ExprBrowser::clearSelection() { treeNew->clearSelection(); }

void ExprBrowser::clearFilter() { exprFilter->clear(); }

void ExprBrowser::filterChanged(const QString& str) {
    proxyModel->setFilterRegExp(QRegExp(str));
    proxyModel->setFilterKeyColumn(0);
    if (str != "") {
        treeNew->expandAll();
    } else {
        treeNew->collapseAll();
    }
}

void ExprBrowser::saveExpressionAs() {
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

void ExprBrowser::saveLocalExpressionAs() {
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

void ExprBrowser::saveExpression() {
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

void ExprBrowser::expandAll() { treeNew->expandAll(); }

void ExprBrowser::expandToDepth(int depth) { treeNew->expandToDepth(depth); }

// Location for storing user's expression files
void ExprBrowser::addUserExpressionPath(const std::string& context) {
    char* homepath = getenv("HOME");
    if (homepath) {
        std::string path = std::string(homepath) + "/" + context + "/expressions/";
        if (QDir(QString(path.c_str())).exists()) {
            _userExprDir = path;
            addPath("My Expressions", path);
        }
    }
}

/*
 * NOTE: The hard-coded paint3d assumptions can be removed once
 * it (and bonsai?) are adjusted to call setSearchPath(context, path)
 */

bool ExprBrowser::getExpressionDirs() {
    const char* env;
    bool enableLocal = false;
    /*bool homeFound = false; -- for xgen's config.txt UserRepo section below */

    if (_searchPath.length() > 0)
        env = _searchPath.c_str();
    else
        env = getenv(P3D_CONFIG_ENVVAR); /* For backwards compatibility */

    if (!env) return enableLocal;

    std::string context;
    if (_context.length() > 0) {
        context = _context;
    } else {
        context = "paint3d"; /* For backwards compatibility */
    }

    clear();

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
                    if (QDir(QString(path.c_str())).exists()) addPath(label, path);
                } else if (key == "ExpressionSubDir") {
                    std::string path;
                    file >> path;
                    _localExprDir = path;
                    if (QDir(QString(path.c_str())).exists()) {
                        addPath("Local", _localExprDir);
                        enableLocal = true;
                    }
                    /* These are for compatibility with xgen.
                     * Long-term, xgen should use the same format.
                     * Longer-term, we should use JSON or something */
                } else if (key == "GlobalRepo") {
                    std::string path;
                    file >> path;
                    path += "/expressions/";
                    if (QDir(QString(path.c_str())).exists()) addPath("Global", path);
                } else if (key == "LocalRepo") {
                    std::string path;
                    file >> path;
                    path += "/expressions/";
                    _localExprDir = path;
                    if (QDir(QString(path.c_str())).exists()) {
                        addPath("Local", _localExprDir);
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
                            addPath("User", path);
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
    update();
    return enableLocal;
}
