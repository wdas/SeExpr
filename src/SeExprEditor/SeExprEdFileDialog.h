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
*/

// NOTE: This is based on Dan's paint3d FileDialog

#ifndef QDSEFILEDIALOG_H
#define QDSEFILEDIALOG_H

#include <QtGui/QLabel>
#include <QtGui/QComboBox>
#include <QtGui/QCheckBox>
#include <QtGui/QLineEdit>
#include <QtGui/QPixmap>
#include <QtGui/QFileDialog>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>


class SeExprEdPreviewWidget : public QWidget
{
public:
    SeExprEdPreviewWidget( QWidget *parent );
    void makePreview(const QString& path);
    void reset() { _pm->setPixmap(QPixmap()); }

private:
    QLabel* _pm;
};


class SeExprEdFileDialog : public QFileDialog
{
    Q_OBJECT
 public:
    SeExprEdFileDialog( QWidget* parent = 0 );
    void addLookInEntries(QStringList paths);
    void saveLookInEntries();
    void restoreLookInEntries();
    QString getOpenFileName( const QString & caption = QString::null,
	const QString & startWith = QString::null,
	const QString & filter = QString::null );
    QString getExistingDirectory( const QString & caption = QString::null,
	const QString & startWith = QString::null,
	const QString & filter = QString::null );
    QString getExistingOrNewDirectory( const QString & caption = QString::null,
	const QString & startWith = QString::null,
	const QString & filter = QString::null );
    QStringList getOpenFileNames( const QString & caption = QString::null,
        const QString & startWith = QString::null,
	const QString & filter = QString::null );
    QString getSaveFileName( const QString & caption = QString::null,
	const QString & startWith = QString::null,
	const QString & filter = QString::null );
    void setPreview();
    void resetPreview();
    void addCheckBox(QString s);
    void addFavoritesButton(QString dirname, QString linkname, QString linkdir);
    bool checkBoxStatus();
    void showCheckBox();
    void hideCheckBox();
    void addComboBox(QString s, QStringList sl);
    void showComboBox();
    void hideComboBox();
    QComboBox* getComboBox() { return _combo; }
    void setButtonName(const QString& str);
    void addSidebarShortcut(const QString& s);

 private slots:
    void handleOk();
    void editReturnPress();
    void gotoFavorites();
    void selChanged(const QString& path);
    void resetDir()
    { if (!_temppath.isEmpty()) setDirectory(_temppath); _temppath = ""; }

 private:
    QString _workingDirectory, _favDir;
    QString _temppath;
    QStringList _lookInList;
    QLineEdit* _nameEdit;
    QPushButton* _okButton;
    bool _createDir;
    SeExprEdPreviewWidget* _pw;
    QCheckBox* _cb;
    QLabel* _combolabel;
    QComboBox* _combo;
};

#endif
