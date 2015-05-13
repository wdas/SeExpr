// %Include Qt/Qtmod.sip

%Module expreditor

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

class SeExprEdDialog:QDialog{
%TypeHeaderCode
#include "SeExprEdDialog.h"
%End
public:
    SeExprEditor* editor;


    SeExprEdDialog(QWidget* parent /TransferThis/ = NULL);
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

class SeExprEdControlCollection:QWidget{
%TypeHeaderCode
#include "SeExprEdControlCollection.h"
%End
public:
        SeExprEdControlCollection(QWidget* parent /TransferThis/=NULL);
        static void setAnimCurveCallback(SIP_PYCALLABLE /AllowNone/);
%MethodCode
        {
            std::string (*old)(const std::string&);
#ifdef SEEXPR_USE_ANIMLIB
            SeExprEdControlCollection::setAnimCurveCallback((a0!=Py_None) ? animCurveCallbackPyWrapper : 0);
            Py_XDECREF(animCurveCallbackPy);
            animCurveCallbackPy=a0;
            Py_INCREF(animCurveCallbackPy);
#endif
        }
%End
};

%ModuleCode
#ifdef SEEXPR_USE_ANIMLIB
#include <iostream>
#include <animlib/AnimCurve.h>
static PyObject* animCurveCallbackPy=0;
void animCurveCallbackPyWrapper(const std::string& arg,animlib::AnimCurve& curve)
{
    PyObject* res;
    SIP_BLOCK_THREADS;
    res=sipCallMethod(0,animCurveCallbackPy,"s",arg.c_str());
    char* buf;
    if(res != NULL){
        const int argsPerItem=9;
        if(!PyTuple_Check(res))
            std::cerr<<"python callback for anim curve did not yield tuple of args"<<std::endl;
        else if((PyTuple_Size(res)-4)%argsPerItem != 0){
            std::cerr<<"did not get argument count of form 4+9i we got "<<PyTuple_Size(res)<<std::endl;
        }else{
            curve.clearKeys();
            if(PyObject* preInf=PyTuple_GetItem(res,0))
                if(char* s=PyString_AsString(preInf))
                    curve.setPreInfinity(curve.stringToInfinityType(s));
            if(PyObject* postInf=PyTuple_GetItem(res,1))
                if(char* s=PyString_AsString(postInf)) 
                    if(s) curve.setPostInfinity(curve.stringToInfinityType(s));
            if(PyObject* weighted=PyTuple_GetItem(res,2))
                if(PyLong_Check(weighted))
                    curve.setWeighted(PyLong_AsLong(weighted));
            int count=PyTuple_Size(res);
            for(int base=4;base<count;base+=argsPerItem){
                // this would be super easy if pytuple supported slicing
                PyObject* slice=PyTuple_GetSlice(res,base,base+9);
                double pos,val,inWeight,outWeight,inAngle,outAngle;
                char *inTang,*outTang;
                int locked;
                if(!PyArg_ParseTuple(slice,"ddddddssi",&pos,&val,&inWeight,&outWeight,&inAngle,&outAngle,&inTang,&outTang,&locked)){
                    std::cerr<<"expreditor: unable to parse anim key\n";
                }
                animlib::AnimKeyframe key(pos,val);
                key.setInWeight(inWeight);key.setOutWeight(outWeight);
                key.setInAngle(inAngle);key.setOutAngle(outAngle);
                key.setInTangentType(key.stringToTangentType(inTang));
                key.setOutTangentType(key.stringToTangentType(outTang));
                curve.addKey(key);
            }
        }

        Py_XDECREF(res);
        
    }
    SIP_UNBLOCK_THREADS;
}
#endif
%End

class SeExprEditor:QWidget{
%TypeHeaderCode
#include "SeExprEditor.h"
%End

public:
    SeExprEditor(QWidget* parent /TransferThis/,SeExprEdControlCollection* collection);

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

class SeExprEdGrapher2d
{
%TypeHeaderCode
#include "SeExprEdGrapher2d.h"
%End

public:
    SeExprEdGrapher2d(QWidget* parent /TransferThis/, int width, int height);
    void update();
};


**/

class SeExprEdBrowser : QWidget
{
%TypeHeaderCode
#include "SeExprEdBrowser.h"
%End


public:
    SeExprEdBrowser(QWidget* parent /TransferThis/, SeExprEditor* editor /TransferThis/);

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

class SeExprEdShortTextEdit:QTextEdit
{
%TypeHeaderCode
#include "SeExprEdShortEdit.h"
%End

public:
    SeExprEdShortTextEdit(QWidget* parent);

signals:
    void textChanged();
};


class SeExprEdShortEdit:QWidget
{
%TypeHeaderCode
#include "SeExprEdShortEdit.h"
%End

public:
    SeExprEdShortEdit(QWidget* parent);
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
    SeExprEdShortTextEdit* edit;

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

class SeExprEdCurve:QWidget{
%TypeHeaderCode
#include "SeExprEdCurve.h"
%End
public:
    SeExprEdCurve(QWidget* parent = NULL, QString pLabel = "", QString vLabel = "", QString iLabel = "");
};

class SeExprEdColorCurve:QWidget{
%TypeHeaderCode
#include "SeExprEdColorCurve.h"
%End
public:
    SeExprEdColorCurve(QWidget* parent = NULL, QString pLabel = "", QString vLabel = "", QString iLabel = "");
};


class SeExprEdColorSwatchWidget:QWidget{
%TypeHeaderCode
#include "SeExprEdColorSwatchWidget.h"
%End
public:
    SeExprEdColorSwatchWidget(bool indexLabel = false, QWidget* parent = NULL);
};

