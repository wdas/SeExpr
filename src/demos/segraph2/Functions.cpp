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
#include "Functions.h"

//! Simple expression class to support our function grapher
class GrapherExpr : public Expression {
    const std::map<std::string, SimpleVar>& vars;

  public:
    //! Constructor that takes the expression to parse
    GrapherExpr(const std::string& expr, const std::map<std::string, SimpleVar>& vars) : Expression(expr), vars(vars) {}

    //! set the independent variable
    void setX(double x_input) { x.val = x_input; }

  private:
    //! independent variable
    mutable SimpleVar x;

    //! resolve function that only supports one external variable 'x'
    ExprVarRef* resolveVar(const std::string& name) const {
        // check my internal variable
        if (name == "x") return &x;
        // check external variable table
        std::map<std::string, SimpleVar>::const_iterator i = vars.find(name);
        if (i != vars.end()) return const_cast<SimpleVar*>(&i->second);
        // nothing found
        return 0;
    }
};

void Functions::setVar(const std::string& name, const double val) { variables[name].val = val; }

Functions::Functions() { variables["t"].val = 0.; }

Functions::~Functions() {
    for (size_t i = 0; i < functions.size(); i++) delete functions[i];
}

void Functions::setupFunction(const std::string& s, GrapherExpr** exprDest) {
    GrapherExpr* expr = new GrapherExpr(s, variables);
    if (!expr->isValid()) {
        std::cerr << "parse error on '" << s << "'" << std::endl;
        std::cerr << expr->parseError() << std::endl;
        delete expr;
        expr = 0;
    } else if (!expr->returnType().isFP(1)) {
        std::cerr << "expected floating point 1 type got " << expr->returnType().toString() << std::endl;
        delete expr;
        expr = 0;

    } else {
        *exprDest = expr;
    }
}

void Functions::addFunction(const QString& s) {
    strings.push_back(s);
    functions.push_back(0);
    selected.push_back(false);
    int newrow = functions.size() - 1;
    setupFunction(s.toStdString(), &functions[newrow]);
    emit(layoutChanged());
}

bool Functions::isValid(int functionId) const { return functions[functionId] != 0; }

double Functions::eval(int functionId, double x) const {
    functions[functionId]->setX(x);
    const double* val = functions[functionId]->evalFP();
    // std::cerr<<"evaluating x="<<x<<" val is "<<val<<std::endl;
    return val[0];
}

bool Functions::isSelected(unsigned int functionId) const {
    if (functionId >= functions.size()) return false;
    return selected[functionId];
}

//! Sets if the given functionId is selected
void Functions::setSelected(unsigned int functionId, bool val) {
    if (functionId < functions.size()) selected[functionId] = val;
}

//! Gets a list of all selection functions
void Functions::getSelected(std::vector<int>& selectedFunctions) const {
    for (size_t i = 0; i < selected.size(); i++)
        if (selected[i]) selectedFunctions.push_back(i);
}

/**************************************************
 * Items below implement the QAbstractItemModel
 *************************************************/

int Functions::rowCount(const QModelIndex&) const { return functions.size(); }

int Functions::columnCount(const QModelIndex&) const { return 2; }

QColor Functions::getColor(const int row) const {
    QColor color;
    color.setHsv(50 * row, 200, 200, 180);
    return color;
}

QVariant Functions::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        if (index.column() == 1) return strings[index.row()];
        return QVariant(" ");
    } else if (role == Qt::BackgroundRole && index.column() == 0) {
        return getColor(index.row());
    }
    return QVariant();
}

QVariant Functions::headerData(int column, Qt::Orientation orient, int role) const {
    if (role == Qt::DisplayRole && orient == Qt::Horizontal) {
        if (column == 1) return QString("f(x)");
    }
    return QVariant();
}

bool Functions::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (role == Qt::EditRole && index.column() == 1 && index.isValid()) {
        strings[index.row()] = value.toString();
        if (functions[index.row()]) delete functions[index.row()];
        functions[index.row()] = 0;
        setupFunction(value.toString().toStdString(), &functions[index.row()]);
        emit dataChanged(index, index);

        return true;
    }
    return false;
}

Qt::ItemFlags Functions::flags(const QModelIndex& index) const {
    if (!index.isValid()) return Qt::NoItemFlags;
    if (index.column() == 1) return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    return Qt::NoItemFlags;
}
