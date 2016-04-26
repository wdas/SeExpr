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

