/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
*
* @file QdSeEditor.h
* @brief This provides an exression editor for SeExpr syntax with auto ui features
* @author  aselle
*/
#ifndef QdSeEditor_h
#define QdSeEditor_h

#include <vector>

#include <QtGui/QTextBrowser>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QDialog>
#include <QtCore/QTimer>
#include <QtCore/QRegExp>
#include <QtGui/QLineEdit>
#include <QtGui/QCheckBox>
#include <QtGui/QSlider>

class QLabel;
class QPushButton;
class QLineEdit;
class QMouseEvent;
class QPaintEvent;
class QKeyEvent;
class QCompleter;
class QToolTip;
class QListWidget;
class QListWidgetItem;
class QdSeCompletionModel;
class QdSeControl;
class QdSeControlCollection;


class QdSeEditor;
class SeExprCompletionModel;
class QdSeHighlighter;
class QdSePopupDocumentation;

class QdExpressionTextEdit : public QTextEdit
{
    Q_OBJECT

        QToolTip* functionTip;
        std::map<std::string,std::string> functionTooltips;
        QdSeHighlighter* highlighter;
        QStyle* lastStyleForHighlighter;
        QdSePopupDocumentation* _tip;
        QAction* _popupEnabledAction;
    public:
        QCompleter* completer;
        QdSeCompletionModel* completionModel;
   

    public:
        QdExpressionTextEdit(QWidget* parent = 0);
        ~QdExpressionTextEdit();
        void updateStyle();

    protected:
        void showTip(const QString& string);
        void hideTip();

        virtual void keyPressEvent(QKeyEvent* e);
        void focusInEvent(QFocusEvent* e);
        void focusOutEvent(QFocusEvent* e);
        void mousePressEvent(QMouseEvent* event);
        void mouseDoubleClickEvent(QMouseEvent* event);
        void paintEvent(QPaintEvent* e);
        void wheelEvent(QWheelEvent* e);                                       
        void contextMenuEvent(QContextMenuEvent* event);

    private slots:
        void insertCompletion(const QString& completion);
    signals:
        void applyShortcut();
        void nextError();

};


class QdSeEditor : public QWidget
{
    Q_OBJECT

    public:
        QdSeEditor(QWidget* parent,QdSeControlCollection* controls);
        virtual ~QdSeEditor();

    public slots:
        void exprChanged();
        void rebuildControls();
        void controlChanged(int id);
        void nextError();
        void selectError();
        void sendApply();
        void sendPreview();
        //void handlePreviewTimer();
    signals:
        void apply();
        void preview();
    public:
        // Get the expression that is in the editor
        std::string getExpr();
        // Sets the expression that is in the editor
        void setExpr(const std::string& expression,const bool apply=false);
        // Append string
        void appendStr(const std::string& str);
    public slots:
        // Insert string
        void insertStr(const std::string& str);
    public:
        // Adds an error and its associated position
        void addError(const int startPos,const int endPos,const std::string& error);
        // Removes all errors and hides the completion widget   
        void clearErrors();
        // Removes all extra completion symbols
        void clearExtraCompleters();
        // Registers an extra function and associated do cstring
        void registerExtraFunction(const std::string& name,const std::string& docString);
        // Register an extra variable (i.e. $P, or $u, something provided by resolveVar)
        void registerExtraVariable(const std::string& name,const std::string& docString);
        // Replace extras
        void replaceExtras(const QdSeCompletionModel& completer);
        // Updates the completion widget, must call after registering any new functions/variables
        void updateCompleter();
        // Updates style
        void updateStyle();
    private:
        QdExpressionTextEdit* exprTe;
        QdSeControlCollection* controls;
        QListWidget* errorWidget;

        QTimer* controlRebuildTimer;
        QTimer* previewTimer;


        bool _updatingText;
        int errorHeight;

};

#endif
