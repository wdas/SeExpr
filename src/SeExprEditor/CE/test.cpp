/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
*/
#include <animlib/AnimCurve.h>
#include <QtGui/QApplication>
#include "CETool.h"
#include "CEMainUI.h"


#include <fenv.h>

int main(int argc,char *argv[])
{ 
    fp();
    QApplication app(argc,argv);
    CETool* tool=new CETool();
    //tool->ui()->show();
    animlib::AnimCurve& anim=*new animlib::AnimCurve((animlib::AnimAttrID()));
    anim.setPreInfinity(animlib::AnimCurve::kInfinityOscillate);
    typedef animlib:: AnimKeyframe  Key;
    Key key1(0.,0.);
    Key key2(2.,2.);
    Key key3(4.,0.);
    Key key4(5.,-2.);
    key1.setInTangentType(animlib::AnimKeyframe::kTangentFixed);
    key1.setInAngle(30);
    key1.setOutTangentType(animlib::AnimKeyframe::kTangentFixed);
    key1.setOutAngle(30);
    anim.addKey(key1);

    key2.setInTangentType(animlib::AnimKeyframe::kTangentFixed);
    key2.setInAngle(-50);
    key2.setOutTangentType(animlib::AnimKeyframe::kTangentFixed);
    key2.setOutAngle(-50);
    anim.addKey(key2);

    key3.setInTangentType(animlib::AnimKeyframe::kTangentFixed);
    key3.setOutTangentType(animlib::AnimKeyframe::kTangentFixed);
    anim.addKey(key3);
    key4.setInTangentType(animlib::AnimKeyframe::kTangentFixed);
    key4.setOutTangentType(animlib::AnimKeyframe::kTangentFixed);
    anim.addKey(key4);

    anim.setWeighted(true);
    QWidget* widg;
    tool->map(widg,0);
    tool->addCurve(&anim);
    widg->setMinimumWidth(512);
    widg->show();
    //tool->disableControls();
    app.exec();
    return 0;
}

