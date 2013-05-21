/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
*/
#ifndef _QdSePopupDocumentation_h_
#define _QdSePopupDocumentation_h_

#include <QtGui/QWidget>

class QLabel;
class QdSePopupDocumentation:public QWidget
{
    Q_OBJECT;

public:
    QLabel* label;
    QdSePopupDocumentation(QWidget* parent,const QPoint& cr,const QString& msg);

protected:
    void	mousePressEvent ( QMouseEvent * event );
};

#endif
