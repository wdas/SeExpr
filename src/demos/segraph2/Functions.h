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
*/ #ifndef _Functions_h_
#define _Functions_h_

#include <QtGui/QTableView>
#include <iostream>
#include <cfloat>

#include <Expression.h>

    //! Simple variable that just returns its internal value
    struct SimpleVar : public ExprVarRef {
    double val;  // independent variable

    SimpleVar() : ExprVarRef(ExprType().FP(1).Varying()), val(0.0) {}

    void eval(double* result) { result[0] = val; }

    void eval(char** result) {}
};

//! Model representing all the functions that the grapher handles
class GrapherExpr;
class Functions : public QAbstractTableModel {
    Q_OBJECT;

    //! List of expression classes
    std::vector<GrapherExpr*> functions;
    //! Expression strings
    std::vector<QString> strings;
    //! Whether item is selected
    std::vector<bool> selected;
    //! Extra variables
    std::map<std::string, SimpleVar> variables;

  public:
    Functions();
    ~Functions();
    //! Sets up a function for evaluation
    void setupFunction(const std::string& s, GrapherExpr** exprDest);
    //! Adds a function given a qstring
    void addFunction(const QString& s);
    //! Evaluates functionId given at independent variable x
    bool isValid(int functionId) const;
    //! Evaluates functionId given at independent variable x
    double eval(int functionId, double x) const;
    //! Returns true if the given functionId is selected
    bool isSelected(unsigned int functionId) const;
    //! Sets if the given functionId is selected
    void setSelected(unsigned int functionId, bool val);
    //! Gets a list of all selection functions
    void getSelected(std::vector<int>& selectedFunctions) const;
    //! Set the time
    void setVar(const std::string& name, const double val);

    /**************************************************
     * Items below implement the QAbstractItemModel
     *************************************************/
    int rowCount(const QModelIndex&) const;
    int columnCount(const QModelIndex&) const;
    QColor getColor(const int row) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int column, Qt::Orientation orient, int role) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role);
    Qt::ItemFlags flags(const QModelIndex& index) const;
};

#endif
