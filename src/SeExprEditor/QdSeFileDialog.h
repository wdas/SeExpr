/* 
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
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


class QdSePreviewWidget : public QWidget
{
public:
    QdSePreviewWidget( QWidget *parent );
    void makePreview(const QString& path);
    void reset() { _pm->setPixmap(QPixmap()); }

private:
    QLabel* _pm;
};


class QdSeFileDialog : public QFileDialog
{
    Q_OBJECT
 public:
    QdSeFileDialog( QWidget* parent = 0 );
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
    QdSePreviewWidget* _pw;
    QCheckBox* _cb;
    QLabel* _combolabel;
    QComboBox* _combo;
};

#endif
