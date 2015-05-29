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
*/
#include "Functions.h"


//! Simple expression class to support our function grapher
class GrapherExpr:public Expression
{
    const std::map<std::string,SimpleVar>& vars;
public:
    //! Constructor that takes the expression to parse
    GrapherExpr(const std::string& expr,const std::map<std::string,SimpleVar>& vars)
        :Expression(expr),vars(vars)
    {}

    //! set the independent variable
    void setX(double x_input)
    {x.val=x_input;}

private:
    //! independent variable
    mutable SimpleVar x;

    //! resolve function that only supports one external variable 'x'
    ExprVarRef* resolveVar(const std::string& name) const
    {
        // check my internal variable
        if(name == "x") return &x;
        // check external variable table
        std::map<std::string,SimpleVar>::const_iterator i=vars.find(name);
        if(i!=vars.end()) return const_cast<SimpleVar*>(&i->second);
        // nothing found
        return 0;
    }
};

void Functions::
setVar(const std::string& name,const double val)
{
    variables[name].val=val;
}

Functions::
Functions()
{
    variables["t"].val=0.;
}

Functions::
~Functions()
{
    for(size_t i=0;i<functions.size();i++) delete functions[i];
}

void Functions::
setupFunction(const std::string& s,GrapherExpr** exprDest)
{
    GrapherExpr* expr=new GrapherExpr(s,variables);
    if(!expr->isValid()){
        std::cerr<<"parse error on '"<<s<<"'"<<std::endl;
        std::cerr<<expr->parseError()<<std::endl;
        delete expr;
        expr=0;
    }else if(!expr->returnType().isFP(1)){
        std::cerr<<"expected floating point 1 type got "<<expr->returnType().toString()<<std::endl;
        delete expr;
        expr=0;

    }else{
        *exprDest=expr; 
    }
}

void Functions::
addFunction(const QString& s)
{
    strings.push_back(s);
    functions.push_back(0);
    selected.push_back(false);
    int newrow=functions.size()-1;
    setupFunction(s.toStdString(),&functions[newrow]);
    emit(layoutChanged());
}


bool Functions::
isValid(int functionId) const
{
    return functions[functionId]!=0;
}

double Functions::
eval(int functionId,double x) const
{
    functions[functionId]->setX(x);
    const double* val=functions[functionId]->evalFP();
    //std::cerr<<"evaluating x="<<x<<" val is "<<val<<std::endl;
    return val[0];
}

bool Functions::
isSelected(unsigned int functionId) const
{
    if(functionId>=functions.size()) return false;
    return selected[functionId];
}

//! Sets if the given functionId is selected
void Functions::
setSelected(unsigned int functionId,bool val)
{
    if(functionId<functions.size()) selected[functionId]=val;
}

//! Gets a list of all selection functions
void Functions::
getSelected(std::vector<int>& selectedFunctions) const
{
    for(size_t i=0;i<selected.size();i++)
        if(selected[i]) selectedFunctions.push_back(i);
}


/**************************************************
 * Items below implement the QAbstractItemModel
 *************************************************/

int Functions::
rowCount(const QModelIndex&) const
{
    return functions.size();
}

int Functions::
columnCount(const QModelIndex&) const
{
    return 2;
}

QColor Functions::
getColor(const int row) const
{
    QColor color;
    color.setHsv(50*row,200,200,180);
    return color;
}

QVariant Functions::
data(const QModelIndex& index,int role) const
{
    if(!index.isValid()) return QVariant();
    if(role==Qt::DisplayRole || role==Qt::EditRole){
        if(index.column()==1) return strings[index.row()];
        return QVariant(" ");
    }else if(role==Qt::BackgroundRole && index.column()==0){
        return getColor(index.row());
    }
    return QVariant();
}

QVariant Functions::
headerData(int column,Qt::Orientation orient,int role) const
{
    if(role==Qt::DisplayRole && orient == Qt::Horizontal){
        if(column==1) return QString("f(x)");
    }
    return QVariant();
}

bool Functions::
setData(const QModelIndex& index,const QVariant& value,int role)
{
    if(role==Qt::EditRole && index.column()==1 && index.isValid()){
        strings[index.row()]=value.toString();
        if(functions[index.row()]) delete functions[index.row()];
        functions[index.row()]=0;
        setupFunction(value.toString().toStdString(),&functions[index.row()]);
        emit dataChanged(index,index);

        return true;
    }
    return false;
}

Qt::ItemFlags Functions::
flags(const QModelIndex& index) const
{
    if(!index.isValid()) return Qt::NoItemFlags;
    if(index.column()==1) return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    return Qt::NoItemFlags;
}
