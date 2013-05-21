/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
*
* @file QdSeShortEdit.h
* @brief This provides an exression editor for SeExpr syntax with auto ui features
* @author  aselle
*/


#ifndef QdSeShortEditor_h
#define QdSeShortEditor_h

#include <vector>
#include <string>

#include <QtGui/QWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QWheelEvent>

class QdSeControlCollection;
class QToolButton;
class QVBoxLayout;
class QHBoxLayout;
class QTextEdit;
class QdSeHighlighter;
class QCompleter;
class QdSeCompletionModel;
class QdSeShortTextEdit;
class QLabel;
class QdSePopupDocumentation;

class QdSeShortEdit : public QWidget
{
    Q_OBJECT
    
protected:
    QTimer* controlRebuildTimer;
    QToolButton* editDetail;
    QdSeControlCollection* controls;
    QVBoxLayout* vboxlayout;
    QHBoxLayout* hboxlayout;
    QLabel* error;
    std::string _context;
    std::string _searchPath;
public:
    QdSeShortEdit(QWidget* parent, bool expanded = true);
    virtual ~QdSeShortEdit();
    
    // Gets the string that is in the edit widget
    std::string getExpressionString() const;
    QString getExpression() const;
    // Sets the string that is in the edit widget
    void setExpressionString(const std::string& expression);
    // Removes all extra completion symbols
    void clearExtraCompleters();
    // Registers an extra function and associated do cstring
    void registerExtraFunction(const std::string& name,const std::string& docString);
    // Register an extra variable (i.e. $P, or $u, something provided by resolveVar)
    void registerExtraVariable(const std::string& name,const std::string& docString);
    // Updates the completion widget, must call after registering any new functions/variables
    void updateCompleter();
    // Hides the expression part of the interface
    void setSimple(bool enabled);
    // Set a menu on the "details" button
    void setDetailsMenu(QMenu *menu);
    // Set a colon-delimited path variable for finding expressions
    void setSearchPath(const QString& context, const QString& path);
    // Set the vertical scrollbar policy -- set to Qt::ScrollBarAlwaysOff to
    // disable it
    void setVerticalScrollBarPolicy(Qt::ScrollBarPolicy policy);
    //  Open the details window and open the Nth editor
    //  Pass -1 to not show the editor
    int showDetails(int idx);

    virtual QSize sizeHint() const{return QSize(400, 50);}
    virtual void hideErrors(bool hidden, const std::string &err);

    // Exposed via Python
    QToolButton* expandButton;
    QdSeShortTextEdit* edit;

protected:
    void checkErrors();

protected slots:
    virtual void detailPressed();
    virtual void expandPressed();
    virtual void textFinished();
    virtual void handleTextEdited();
    virtual void controlChanged(int id);
    virtual void rebuildControls();

signals:
    void exprChanged();
};


class QdSeShortTextEdit:public QTextEdit
{
    Q_OBJECT;

    QdSeHighlighter* highlighter;
    
    bool editing;
    QString savedText;
    QdSePopupDocumentation* _tip;
    QStyle* lastStyleForHighlighter;

public:
    QCompleter* completer;
    QdSeCompletionModel* completionModel;


    QdSeShortTextEdit(QWidget* parent);
protected:
    void showTip(const QString& string);
    void hideTip();

    void paintEvent(QPaintEvent* e);
    virtual void keyPressEvent(QKeyEvent* e);
    virtual void focusInEvent(QFocusEvent* e);
    virtual void focusOutEvent(QFocusEvent* e);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* e) { e->ignore(); }

    void setColor(bool editing);
    void finishEdit();
signals:
    void editingFinished();
private slots:
    void insertCompletion(const QString& completion);

};

#endif
