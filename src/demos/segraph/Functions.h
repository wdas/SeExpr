/*
 SEEXPR SOFTWARE
 Copyright 2011 Disney Enterprises, Inc. All rights reserved
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are
 met:
 
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in
 the documentation and/or other materials provided with the
 distribution.
 
 * The names "Disney", "Walt Disney Pictures", "Walt Disney Animation
 Studios" or the names of its contributors may NOT be used to
 endorse or promote products derived from this software without
 specific prior written permission from Walt Disney Pictures.
 
 Disclaimer: THIS SOFTWARE IS PROVIDED BY WALT DISNEY PICTURES AND
 CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR PURPOSE, NONINFRINGEMENT AND TITLE ARE DISCLAIMED.
 IN NO EVENT SHALL WALT DISNEY PICTURES, THE COPYRIGHT HOLDER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND BASED ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*/#ifndef _Functions_h_
#define _Functions_h_

#include <QtGui/QTableView>
#include <iostream>
#include <cfloat>

#include <SeExpression.h>

//! Simple variable that just returns its internal value
struct SimpleVar:public SeExprVarRef
{
    double val; // independent variable

    SimpleVar()
        : SeExprVarRef(SeExprType().FP(1).Varying()), val(0.0)
    {}

    void eval(double* result)
    {result[0]=val;}
    
    void eval(char** result)
    {}
};

//! Model representing all the functions that the grapher handles
class GrapherExpr;
class Functions:public QAbstractTableModel
{
    Q_OBJECT;

    //! List of expression classes
    std::vector<GrapherExpr*> functions;
    //! Expression strings
    std::vector<QString> strings;
    //! Whether item is selected
    std::vector<bool> selected;
    //! Extra variables
    std::map<std::string,SimpleVar> variables;
 
public:
    Functions();
    ~Functions();
    //! Sets up a function for evaluation
    void setupFunction(const std::string& s,GrapherExpr** exprDest);
    //! Adds a function given a qstring
    void addFunction(const QString& s);
    //! Evaluates functionId given at independent variable x
    bool isValid(int functionId) const;
    //! Evaluates functionId given at independent variable x
    double eval(int functionId,double x) const;
    //! Returns true if the given functionId is selected
    bool isSelected(unsigned int functionId) const;
    //! Sets if the given functionId is selected
    void setSelected(unsigned int functionId,bool val);
    //! Gets a list of all selection functions
    void getSelected(std::vector<int>& selectedFunctions) const;
    //! Set the time
    void setVar(const std::string& name,const double val);


    /**************************************************
     * Items below implement the QAbstractItemModel
     *************************************************/
    int rowCount(const QModelIndex&) const;
    int columnCount(const QModelIndex&) const;
    QColor getColor(const int row) const;
    QVariant data(const QModelIndex& index,int role) const;
    QVariant headerData(int column,Qt::Orientation orient,int role) const;
    bool setData(const QModelIndex& index,const QVariant& value,int role);
    Qt::ItemFlags flags(const QModelIndex& index) const;
    
};

#endif
