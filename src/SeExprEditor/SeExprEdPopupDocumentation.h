/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
*/
#ifndef _SeExprEdPopupDocumentation_h_
#define _SeExprEdPopupDocumentation_h_

#include <QtGui/QWidget>

class QLabel;
class SeExprEdPopupDocumentation:public QWidget
{
    Q_OBJECT;

public:
    QLabel* label;
    SeExprEdPopupDocumentation(QWidget* parent,const QPoint& cr,const QString& msg);

protected:
    void	mousePressEvent ( QMouseEvent * event );
};

#endif
