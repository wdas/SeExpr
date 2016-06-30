// %Include Qt/Qtmod.sip

%Module expreditor2

%MappedType std::string
{
%TypeHeaderCode
#include <iostream>
#include <string>
%End

%ConvertFromTypeCode
    // convert an std::string to a Python (unicode) string
    PyObject* newstring;
    newstring = PyUnicode_DecodeUTF8(sipCpp->c_str(), sipCpp->length(), NULL);
    if(newstring == NULL) {
        PyErr_Clear();
        newstring = PyString_FromString(sipCpp->c_str());
    }
    return newstring;
%End

%ConvertToTypeCode
     // Allow a Python string (or a unicode string) whenever a string is
     // expected.
     // If argument is a Unicode string, just decode it to UTF-8
     // If argument is a Python string, assume it's UTF-8
     if (sipIsErr == NULL)
                return (PyString_Check(sipPy) || PyUnicode_Check(sipPy));
        if (sipPy == Py_None) {
                *sipCppPtr = new std::string;
                return 1;
        }
        if (PyUnicode_Check(sipPy))        {
        PyObject* s = PyUnicode_AsEncodedString(sipPy, "UTF-8", "");
        *sipCppPtr = new std::string(PyString_AS_STRING(s));
        Py_DECREF(s);
        return 1;
        }
        if (PyString_Check(sipPy)) {
        *sipCppPtr = new std::string(PyString_AS_STRING(sipPy));
            return 1;
        }
    return 0;
%End
};

%Import QtGui/QtGuimod.sip

class ExprDialog:QDialog{
%TypeHeaderCode
#include "ExprDialog.h"
%End
public:
    ExprEditor* editor;


    ExprDialog(QWidget* parent /TransferThis/ = NULL);
    std::string getExpressionString();
    void setExpressionString(const std::string&);
    void show();

signals:
    void preview();
    void expressionApplied();

public slots:
    void applyExpression();
    void clearExpression();
    
};

class ExprControlCollection:QWidget{
%TypeHeaderCode
#include "ExprControlCollection.h"
%End
public:
        ExprControlCollection(QWidget* parent /TransferThis/=NULL);
};

class ExprEditor:QWidget{
%TypeHeaderCode
#include "ExprEditor.h"
%End

public:
    ExprEditor(QWidget* parent /TransferThis/,ExprControlCollection* collection);

signals:
    void applyExpression(); // This is when text or controls are manipulated
    //void needNewPreview();
    void clearExpression();

public slots:
    void exprChanged();
    void rebuildControls();
    void controlChanged(int id);

public:
    std::string getExpr();
    void setExpr(const std::string& expression,const bool apply=false);
    void addError(const int startPos,const int endPos,const std::string& error);
    void clearErrors();
    void clearExtraCompleters();
    void registerExtraFunction(const std::string& name,const std::string& docString);
    void registerExtraVariable(const std::string& name,const std::string& docString);
    void updateCompleter();
};

/**

class ExprGrapher2d
{
%TypeHeaderCode
#include "ExprGrapher2d.h"
%End

public:
    ExprGrapher2d(QWidget* parent /TransferThis/, int width, int height);
    void update();
};


**/

class ExprBrowser : QWidget
{
%TypeHeaderCode
#include "ExprBrowser.h"
%End


public:
    ExprBrowser(QWidget* parent /TransferThis/, ExprEditor* editor /TransferThis/);

    void addPath(const std::string& name,const std::string& path);
    std::string getSelectedPath();
    void selectPath(const char * path);
    void setSearchPath(const QString& context, const QString& path);

public slots:
    void update();
    void clear();

    void saveExpression();
    void saveExpressionAs();
    void saveLocalExpressionAs();

};

class ExprShortTextEdit:QTextEdit
{
%TypeHeaderCode
#include "ExprShortEdit.h"
%End

public:
    ExprShortTextEdit(QWidget* parent);

signals:
    void textChanged();
};


class ExprShortEdit:QWidget
{
%TypeHeaderCode
#include "ExprShortEdit.h"
%End

public:
    ExprShortEdit(QWidget* parent);
    std::string getExpressionString() const;
    void setExpressionString(const std::string&);
    void clearExtraCompleters();
    void registerExtraFunction(const std::string& name,const std::string& docString);
    void registerExtraVariable(const std::string& name,const std::string& docString);
    void setSimple(bool enabled);
    void updateCompleter();
    void setDetailsMenu(QMenu *menu);
    void setSearchPath(const QString& context, const QString& path);
    void setVerticalScrollBarPolicy(Qt::ScrollBarPolicy policy);
    virtual void hideErrors(bool hidden, const std::string &err);
    void showDetails(int idx);

    QToolButton* expandButton;
    ExprShortTextEdit* edit;

protected slots:
    virtual void detailPressed();
    virtual void expandPressed();
    virtual void textFinished();
    virtual void handleTextEdited();
    virtual void controlChanged(int id);
    virtual void rebuildControls();
    virtual void expressionApplied();
    virtual void dialogClosed();

signals:
    void exprChanged();
};

class ExprCurve:QWidget{
%TypeHeaderCode
#include "ExprCurve.h"
%End
public:
    ExprCurve(QWidget* parent = NULL, QString pLabel = "", QString vLabel = "", QString iLabel = "");
};

class ExprColorCurve:QWidget{
%TypeHeaderCode
#include "ExprColorCurve.h"
%End
public:
    ExprColorCurve(QWidget* parent = NULL, QString pLabel = "", QString vLabel = "", QString iLabel = "");
};


class ExprColorSwatchWidget:QWidget{
%TypeHeaderCode
#include "ExprColorSwatch.h"
%End
public:
    ExprColorSwatchWidget(bool indexLabel = false, QWidget* parent = NULL);
};

class ExprDeepWater:QWidget{
%TypeHeaderCode
#include "ExprDeepWater.h"
%End
public:
    ExprDeepWater(QWidget* parent = NULL);
};
