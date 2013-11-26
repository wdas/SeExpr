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
* @file SeExprEdCompletionModel.h
* @brief Provides a model for providing completion items
* @author  aselle
*/

#ifndef SeExprEdCompletionModel_h
#define SeExprEdCompletionModel_h

#include <QtCore/QAbstractItemModel>
#include <QtCore/QString>
#include <QtCore/QSize>
#include <vector>

class SeExprEdCompletionModel:public QAbstractItemModel // ItemModel
{
public:
    // clear/add functions (these are ones that will be resolved with resolveFunc()
    void clearFunctions();
    void addFunction(const QString& function,const QString& docString);
        
    // clear/add user variables (these are ones that will be resolved with resolveVar()
    void clearVariables();
    void addVariable(const QString& str,const QString& comment);

    // add extras
    void syncExtras(const SeExprEdCompletionModel& otherModel);

    SeExprEdCompletionModel(QObject* parent=0);

    QModelIndex index(int row,int column,const QModelIndex&) const
    {return createIndex(row,column,0);}

    QModelIndex parent(const QModelIndex&) const
    {return QModelIndex();}
    
    int rowCount(const QModelIndex& parent=QModelIndex()) const
    {
        Q_UNUSED(parent);
        int count= builtins.size()+functions.size()+variables.size()+local_variables.size();
        return count;
    }

    int columnCount(const QModelIndex& parent) const
    {Q_UNUSED(parent); return 2;}

    QString getFirstLine(const std::string& all) const
    {
        size_t newline=all.find("\n");
        if(newline!=std::string::npos) return QString(all.substr(0,newline).c_str());
        else return QString(all.c_str());
    }

    QVariant data(const QModelIndex& index,int role=Qt::DisplayRole) const;

    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const 
    {
        Q_UNUSED(orientation);
        if(role==Qt::DisplayRole) return QVariant("");
        else if(role==Qt::SizeHintRole){
            if(section==0) return QVariant(QSize(100,1));
            else return QVariant(QSize(200,1));
        }else return QVariant();
    }
    std::vector<QString> local_variables; // only the expression editor itself should modify these

    QString getDocString(const QString& s);
private:
    static std::vector<QString> builtins;
    std::vector<QString> functions,functions_comment;
    std::map<QString,int> functionNameToFunction;
    std::vector<QString> variables,variables_comment;
};



#endif
