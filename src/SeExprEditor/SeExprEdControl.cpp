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
*
* @file SeExprEditor.cpp
* @brief This provides an exression editor for SeExpr syntax with auto ui features
* @author  aselle
*/
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QToolButton>
#include <QtCore/QRegExp>
#include <QtGui/QSplitter>
#include <QtGui/QLabel>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>
#include <QtGui/QScrollArea>
#include <QtGui/QSpacerItem>
#include <QtGui/QSizePolicy>
#include <QtGui/QTextCharFormat>
#include <QtGui/QCompleter>
#include <QtGui/QAbstractItemView>
#include <QtGui/QStandardItemModel>
#include <QtGui/QStringListModel>
#include <QtGui/QFileDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QScrollBar>
#include <QtGui/QToolTip>
#include <QtGui/QListWidget>
#include <QtGui/QTreeView>

#include "SeExprEdControl.h"
#include "SeExprEdColorCurve.h"
#include "SeExprEdFileDialog.h"
#include "SeExprEdEditable.h"
#ifdef SEEXPR_USE_ANIMLIB
#   include <animlib/AnimCurve.h>
#   include <animlib/AnimKeyframe.h>
#endif

/* XPM */
static const char *refreshXPM[]={
"20 20 4 1",
"# c #303030",
"a c #585858",
"b c #c3c3c3",
". c #dcdcdc",
"....................",
"....................",
"....................",
".......#aaaa#.......",
".....#########......",
"....###bbbbb###.....",
"....##b.....b##.....",
"...bb#b.....b##.....",
"...bbbb....aaaaaa...",
"...........aaaaaa...",
"....##......####....",
"...####......##.....",
"..######............",
"..aaa#aa............",
"....##......bbb.....",
"....##b...bbbab.....",
"....a#abbbb##ab.....",
".....#a####aa#b.....",
".....aaaaaaa#.b.....",
"...................."};

/* XPM */
static const char *graphXPM[]={
"20 20 5 1",
"c c #000040",
"a c #303030",
"# c #58a8ff",
". c #dcdcdc",
"b c #ff0000",
"..........#a.a......",
"..........#a.a.....b",
"..........#.a.....bb",
"..........#aa....bb.",
"..........#.....bb..",
"..........#....bb...",
"..........#....bb...",
"....bbb...#...bb....",
"...bbbbb..#..bbb....",
"...b...bbb#.bbb.....",
"..bb....bb#bbb......",
"##bb####bbbbb#######",
".bb......bbb....c.c.",
".bb.......#......c..",
".b........#.....c.c.",
"bb........#.........",
"b.........#.........",
"..........#.........",
"..........#.........",
"..........#........."};


/* XPM */
static const char *directoryXPM[]={
"20 20 3 1",
". c None",
"# c #000000",
"a c #d8c59e",
"....................",
"....................",
"....................",
"....................",
"...........#######..",
"...........#aaaaa#..",
"..##########aaaaa#..",
"..#aaaaaaaaaaaaaa#..",
"..#aaaaaaaaaaaaaa#..",
"..#aaaaaaaaaaaaaa#..",
"..#aaaaaaaaaaaaaa#..",
"..#aaaaaaaaaaaaaa#..",
"..#aaaaa##a##a##a#..",
"..#aaaaa##a##a##a#..",
"..#aaaaaaaaaaaaaa#..",
"..################..",
"....................",
"....................",
"....................",
"...................."};

/* XPM */
static const char *fileXPM[]={
"20 20 5 1",
". c None",
"# c #000000",
"c c #303030",
"b c #a79b80",
"a c #ddcdaa",
"....................",
"....................",
"....#########.......",
"....#aaaaaaa##......",
"....#aaaaaaa#b#.....",
"....#aaaaaaa#bb#....",
"....#aaaaaaa####....",
"....#aaaaaaaaaa#....",
"....#aaaaaaaaaa#....",
"....#aaaaaaaaaa#....",
"....#aaaaaaaaaa#....",
"....#aaaaaaaaaa#....",
"....#aaaaaaaaaa#....",
"....#aaaaaaaaaa#....",
"....#aaaaaaaaaa#....",
"....#accaccacca#....",
"....#accaccacca#....",
"....#aaaaaaaaaa#....",
"....############....",
"...................."};



void SeExprEdSlider::mousePressEvent(QMouseEvent* e)
{
    mouseMoveEvent(e);
}

void SeExprEdSlider::mouseMoveEvent(QMouseEvent* e)
{
    float r = maximum()-minimum();
    float v = ((float)(e->x()-2)*r/(width()-5))+minimum()+.5f;
    int iv = std::min(std::max((int)v,minimum()),maximum());
    setValue(iv);
}

void SeExprEdSlider::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    QPainter p(this);
    
    float v = value();
    float r = maximum()-minimum();
    int linepos = (int)((v-minimum())/r*(width()-5)+2);
    
    QColor qcol = palette().color(QPalette::Dark);
    QColor bcol = palette().color(QPalette::Midlight);
    QColor dcol = bcol.lighter(140);
    QColor bgcol = palette().color(QPalette::Base);
    
    if (underMouse())
    {
        bcol = bcol.lighter(110);
        bgcol = bgcol.lighter(110);
        int mx = mapFromGlobal(QCursor::pos()).x();
        if (abs(linepos-mx)<4) dcol = dcol.lighter(200);
    }
    
    p.fillRect(1,1,width()-1,height()-2,bgcol);
    p.fillRect(1,1,linepos-1,height()-2,bcol);
    
    QPen pen = p.pen();
    
    pen.setColor(dcol);
    p.setPen(pen);
    pen.setWidth(3);
    p.setPen(pen);
    p.drawLine(linepos,2,linepos,height()-2);
    pen.setWidth(1);
    pen.setColor(qcol);
    p.setPen(pen);
    p.drawLine(linepos-2,1,linepos-2,height()-1);
    p.drawLine(linepos+2,1,linepos+2,height()-1);
    
    pen.setWidth(1);
    pen.setColor(qcol);
    p.setPen(pen);
    p.drawRect(0,0,width()-1,height()-1);
}

SeExprEdChannelSlider::SeExprEdChannelSlider(int id, QWidget* parent)
 : QWidget(parent), _id(id), _value(0)
{
}

void SeExprEdChannelSlider::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    if (_value < 0 || _value > 1) return;
    int x = int(_value * (width()-3) + 0.5);
    QPainter p(this);
    p.fillRect(contentsRect(), _col);
    p.fillRect(x, 0, 3, height(), QColor(64,64,64));
}

void SeExprEdChannelSlider::mousePressEvent(QMouseEvent* e)
{
    mouseMoveEvent(e);
}


void SeExprEdChannelSlider::mouseMoveEvent(QMouseEvent* e)   
{
    setValue(clamp(float(e->x()-1) / (width()-3), 0, 1));
}


void SeExprEdChannelSlider::setValue(float value)
{
    if (value == _value) return;
    _value = value;
    emit valueChanged(_id, value);
    update();
}


SeExprEdControl::SeExprEdControl(int id,SeExprEdEditable* editable,bool showColorLink)
    :_id(id),_updating(false),_editable(editable)
{
    hbox=new QHBoxLayout(this);
    hbox->setSpacing(2);
    hbox->setMargin(0);

    _colorLinkCB=new QCheckBox(this);
    _colorLinkCB->setFixedWidth(14);
    _colorLinkCB->setFocusPolicy(Qt::NoFocus);
    connect(_colorLinkCB,SIGNAL(stateChanged(int)),this,SLOT(linkStateChange(int)));
    hbox->addWidget(_colorLinkCB);

    _label = new QLabel(QString("<b>")+editable->name.c_str()+"</b>");
    _label->setFixedWidth(72);
    _label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _label->setIndent(2);
    _label->setAutoFillBackground(true);
    hbox->addWidget(_label);

    if(!showColorLink){
        _colorLinkCB->setHidden(true);
        _label->setFixedWidth(84);
    }else{
        _colorLinkCB->setHidden(false);
        _label->setFixedWidth(84-_colorLinkCB->width()+2);
    }

}

void SeExprEdControl::linkStateChange(int state)
{
    if(_updating) return;

    if(state==Qt::Checked){
        emit linkColorLink(_id);
        emit linkColorEdited(_id,getColor());
    }else{
        emit linkColorLink(-1);
    }
}

void SeExprEdControl::linkDisconnect(int newId)
{
    if(newId!=_id){
        _updating = 1;
        _colorLinkCB->setChecked(false);
        _updating = 0;
    }
}



 
SeExprEdNumberControl::SeExprEdNumberControl(int id,SeExprEdNumberEditable* editable)
    :SeExprEdControl(id,editable,false),_numberEditable(editable)
{

    // slider
    float smin = editable->min, smax = editable->max;
    if(!_numberEditable->isInt) { smin *= 1e5; smax *= 1e5; }
    float srange = smax-smin;
    _slider = new SeExprEdSlider(Qt::Horizontal,this);
    _slider->setRange(int(smin),int(smax));
    _slider->setTickInterval(std::max(1,int(srange/10)));
    _slider->setSingleStep(std::max(1,int(srange/50)));
    _slider->setPageStep(std::max(1,int(srange/10)));
    _slider->setMinimumWidth(0);
    _slider->setFixedHeight(16);
    _slider->setFocusPolicy(Qt::ClickFocus);
    hbox->addWidget(_slider, 3);
    // edit box
    _edit = new SeExprEdLineEdit(0,this);
    _edit->setMinimumWidth(0);
    _edit->setFixedHeight(16);
    hbox->addWidget(_edit);
    connect(_edit,SIGNAL(textChanged(int,const QString&)),SLOT(editChanged(int,const QString&)));
    connect(_slider, SIGNAL(valueChanged(int)), SLOT(sliderChanged(int)));
    // show current values
    updateControl();
}

void SeExprEdNumberControl::sliderChanged(int value)
{
    if (_updating) return;
    setValue(_numberEditable->isInt  ? value : value*1e-5);
    
}

void SeExprEdNumberControl::editChanged(int id,const QString& text)
{
    Q_UNUSED(id);
    if (_updating) return;
    bool ok = 0;
    float val = text.toFloat(&ok);
    if (!ok) return;
    setValue(val);
}

void SeExprEdNumberControl::updateControl()
{
    _updating = 1;
    int sliderval = int(_numberEditable->isInt  ? _numberEditable->v : _numberEditable->v*1e5);
    if (sliderval != _slider->value()) _slider->setValue(sliderval);
    _edit->setText(QString("%1")
        .arg(_numberEditable->v, 0, 'f', _numberEditable->isInt ? 0 : 3));
    _updating=0;
}

void SeExprEdNumberControl::setValue(float value)
{
    //std::cerr<<"In setValue "<<_id<<value<<std::endl;
    if (fabs(_numberEditable->v-value) < 1e-5) return;
    _numberEditable->v=value;
    updateControl();
    emit controlChanged(_id);
}

SeExprEdVectorControl::SeExprEdVectorControl(int id,SeExprEdVectorEditable* editable)
    :SeExprEdControl(id,editable,true),_numberEditable(editable)
{

    if(_numberEditable->isColor){
        _swatch=new SeExprEdCSwatchFrame(editable->v);
        _swatch->setFixedWidth(38);
        _swatch->setFixedHeight(20);
        connect(_swatch,SIGNAL(swatchChanged(QColor)),this,SLOT(swatchChanged(QColor)));
        hbox->addWidget(_swatch);
    }
    for(int i=0;i<3;i++){
        QVBoxLayout* vbl=new QVBoxLayout();
        hbox->addLayout(vbl);
        vbl->setMargin(0);
        vbl->setSpacing(0);

        SeExprEdLineEdit* edit = new SeExprEdLineEdit(i, this);
        vbl->addWidget(edit);
        _edits[i]=edit;
        edit->setMinimumWidth(0);
        edit->setFixedHeight(16);

        SeExprEdChannelSlider* slider = new SeExprEdChannelSlider(i, this);
        vbl->addWidget(slider);
        _sliders[i]=slider;
        slider->setFixedHeight(6);
        // set color
        static const QColor rgb[3] = { QColor(128,64,64), QColor(64,128,64), QColor(64,64,128) };
        if(_numberEditable->isColor) slider->setDisplayColor(rgb[i]);

        connect(edit,SIGNAL(textChanged(int,const QString&)),SLOT(editChanged(int,const QString&)));
        connect(slider, SIGNAL(valueChanged(int,float)), SLOT(sliderChanged(int,float)));

    }
    // update controls
    updateControl();
}

void SeExprEdVectorControl::swatchChanged(QColor gah)
{
    Q_UNUSED(gah);
    SeVec3d color=_swatch->getValue();
    setValue(0,color[0]);
    setValue(1,color[1]);
    setValue(2,color[2]);
}

QColor SeExprEdVectorControl::getColor()
{
    return QColor::fromRgbF(clamp(_numberEditable->v[0],0,1),
                            clamp(_numberEditable->v[1],0,1),
			    clamp(_numberEditable->v[2],0,1));
}

void SeExprEdVectorControl::setColor(QColor color)
{
    setValue(0,color.redF());
    setValue(1,color.greenF());
    setValue(2,color.blueF());
}

void SeExprEdVectorControl::sliderChanged(int id,float value)
{
    if (_updating) return;
    setValue(id,_numberEditable->min + value * (_numberEditable->max-_numberEditable->min));
    if (_numberEditable->isColor) emit linkColorEdited(_id,getColor());
}

void SeExprEdVectorControl::editChanged(int id,const QString& text)
{
    if (_updating) return;
    bool ok = 0;
    float val = text.toFloat(&ok);
    if (!ok) return;
    setValue(id,val);
}

void SeExprEdVectorControl::updateControl()
{
//    //std::cerr<<"In update control "<<_id<<std::endl;
    _updating = 1;
    for (unsigned int i = 0; i < 3; i++)
        _edits[i]->setText(QString("%1")
            .arg(_numberEditable->v[i], 0, 'f', 3));
    double min=_numberEditable->min,max=_numberEditable->max;
    for (unsigned int i = 0; i < 3; i++){
        _sliders[i]->setValue((_numberEditable->v[i]-min)/(max-min));
    }
    if(_numberEditable->isColor){
        //std::cerr<<"trying to set color"<<std::endl;
        SeVec3d val=_numberEditable->v;
        float r = clamp(val[0],0,1);
        float g = clamp(val[1],0,1);
        float b = clamp(val[2],0,1);
        float lum = r*.2+g*.7+b*.1;
        //std::cerr<<" rgb "<<r<<" "<<g<<" "<<b<<std::endl;
        QPalette pal = palette();
        pal.setColor(QPalette::Window,QColor(int(r*255),int(g*255),int(b*255)));
        pal.setColor(QPalette::WindowText,(lum<0.5) ? QColor(255,255,255) : QColor(0,0,0));
        _label->setPalette(pal);

    }
    _updating = 0;
}

void SeExprEdVectorControl::setValue(int n,float value)
{
    if (n < 0 || n >= 3) return;
    if (fabs(_numberEditable->v[n]-value) < 1e-5) return;
    _numberEditable->v[n] = value;
    if(_swatch) _swatch->setValue(_numberEditable->v);
    updateControl();
    emit controlChanged(_id);
}

SeExprEdStringControl::SeExprEdStringControl(int id,SeExprEdStringEditable* editable)
    :SeExprEdControl(id,editable,false),_stringEditable(editable)
{
    // make line edit
    _edit=new QLineEdit();
    _edit->setFixedHeight(20);
    connect(_edit,SIGNAL(textChanged(const QString&)),SLOT(textChanged(const QString&)));
    // make a button if we are a file or directory
    if(_stringEditable->type=="file" || _stringEditable->type=="directory"){
        QPushButton* button=new QPushButton();
        button->setFixedSize(20,20);

        hbox->addWidget(_edit,3);
        hbox->addWidget(button,1);
        if(_stringEditable->type=="directory"){
            connect(button,SIGNAL(clicked()),SLOT(directoryBrowse()));
            button->setIcon(QIcon(QPixmap(directoryXPM)));
        } else if(_stringEditable->type=="file"){
            connect(button,SIGNAL(clicked()),SLOT(fileBrowse()));
            button->setIcon(QIcon(QPixmap(fileXPM)));
        }
        
    }else{
        hbox->addWidget(_edit,3);
    }
    // update controls
    updateControl();
}

void SeExprEdStringControl::fileBrowse()
{
    SeExprEdFileDialog dialog(this);
    dialog.setPreview();
    QString newFilename=dialog.getOpenFileName("Please choose a file",_edit->text(),tr("Images (*.tif *.tx *.jpg *.ptx *.png)"));
    if(newFilename!="") _edit->setText(newFilename);
}

void SeExprEdStringControl::directoryBrowse()
{
    SeExprEdFileDialog dialog(this);
    dialog.setPreview();
    QString newFilename=dialog.getExistingDirectory("Please choose a file",_edit->text());
    if(newFilename!="") _edit->setText(newFilename);
}

void SeExprEdStringControl::updateControl()
{
    _edit->setText(_stringEditable->v.c_str());
}

void SeExprEdStringControl::textChanged(const QString& newText)
{
    if(_updating) return;
    _stringEditable->v=newText.toStdString();
    emit controlChanged(_id);
}

SeExprEdCurveControl::SeExprEdCurveControl(int id,SeExprEdCurveEditable* editable)
    :SeExprEdControl(id,editable,false),_curveEditable(editable)
{
    _curve = new SeExprEdCurve(this, "Pos:", "Val:", "Interp:");
    _curve->setFixedHeight(80);
    
    const int numVal = _curveEditable->cvs.size();
    for (int i = 0; i < numVal; i++) {
        const SeExpr::SeCurve<double>::CV& cv=_curveEditable->cvs[i];
        _curve->addPoint(cv._pos,cv._val,cv._interp);
    }
    hbox->addWidget(_curve, 3);
    connect(_curve->_scene, SIGNAL(curveChanged()), SLOT(curveChanged()));
    // unneded? updateControl();
}

void SeExprEdCurveControl::curveChanged()
{
    if(_curve && _curveEditable){
        _curveEditable->cvs=_curve->_scene->_cvs;
        emit controlChanged(_id);
    }
}

SeExprEdCCurveControl::SeExprEdCCurveControl(int id,SeExprEdColorCurveEditable* editable)
    :SeExprEdControl(id,editable,true),_curveEditable(editable)
{
    _curve = new SeExprEdColorCurve(this, "Pos:", "Val:", "Interp:");
    _curve->setFixedHeight(80);
    
    const int numVal = _curveEditable->cvs.size();
    for (int i = 0; i < numVal; i++) {
        const SeExpr::SeCurve<SeVec3d>::CV& cv=_curveEditable->cvs[i];
        _curve->addPoint(cv._pos,cv._val,cv._interp);
    }
    hbox->addWidget(_curve, 3);
    connect(_curve->_scene, SIGNAL(curveChanged()), SLOT(curveChanged()));
    // unneeded? updateControl();
}

void SeExprEdCCurveControl::curveChanged()
{
    if(_curve && _curveEditable){
        _curveEditable->cvs=_curve->_scene->_cvs;
        emit controlChanged(_id);

    }
}


QColor SeExprEdCCurveControl::getColor()
{
    return _curve->getSwatchColor();
}

void SeExprEdCCurveControl::setColor(QColor color)
{
    _curve->setSwatchColor(color);
}

struct SeExprEdGraphPreview:public QWidget
{
    std::vector<float> x,y;
    std::vector<float> cpx,cpy;
    float xmin,xmax,ymin,ymax,dx,dy;;
    float win_xmin,win_xmax,win_ymin,win_ymax,win_dx,win_dy;;
    SeExprEdGraphPreview(QWidget* parent=0)
        :QWidget(parent)
    {
        win_xmin=-1.;
        win_xmax=2.;
        win_ymin=-1;
        win_ymax=2.;
    }

    QPointF toScreen(float x,float y){
        return QPointF((x-win_xmin)*win_dx,height()-(y-win_ymin)*win_dy);
    }


    void paintEvent(QPaintEvent* event){
        QWidget::paintEvent(event);
	QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing,true);
        painter.setPen(QColor(255,255,255));
        win_xmin=xmin;win_xmax=xmax;
        win_ymin=ymin;win_ymax=ymax;
        float percentXpad=.1*(win_xmax-win_xmin);
        float percentYpad=.1*(win_ymax-win_ymin);
        win_xmin-=percentXpad;
        win_xmax+=percentXpad;
        win_ymin-=percentYpad;
        win_ymax+=percentYpad;
        
        // make space for text
        int x_pad_in_pixels=25,y_pad_in_pixels=15;
        float xpad=x_pad_in_pixels*(win_xmax-win_xmin)/(width()-x_pad_in_pixels);
        float ypad=y_pad_in_pixels*(win_ymax-win_ymin)/(height()-y_pad_in_pixels);
        win_ymin-=ypad;
        win_xmax+=xpad;

        win_dx=width()/(win_xmax-win_xmin);
        win_dy=height()/(win_ymax-win_ymin);

        //int h=height();
        QPainterPath path;
        QRectF fullarea(toScreen(win_xmin,win_ymax),toScreen(win_xmax,win_ymin));
        QBrush darkbrush(QColor(100,100,100),Qt::SolidPattern);
        QRectF area(toScreen(xmin,ymax),toScreen(xmax,ymin));
        QBrush brush(QColor(150,150,150),Qt::SolidPattern);
        //painter.fillRect(fullarea,darkbrush);
        //painter.setBrush(darkbrush);
        //painter.drawRoundedRect(fullarea,3,3);
        //painter.setBrush(QBrush());
        painter.fillRect(area,brush);
        if(x.size()>0){
            path.moveTo(toScreen(x[0],y[0]));
            for(int i=1;i<(int)x.size();i++)
                path.lineTo(toScreen(x[i],y[i]));

        }
        QRectF right(toScreen(xmax,ymax),toScreen(win_xmax,ymin));
        QRectF bottom(toScreen(xmin,ymin),toScreen(xmax,win_ymin));

        painter.setPen(QColor(75,50,50));
        painter.drawPath(path);

        painter.setPen(QPen());
        painter.drawText(right,Qt::AlignTop|Qt::AlignLeft,QString("%1").arg(ymax,0,'f',1));
        painter.drawText(right,Qt::AlignBottom|Qt::AlignLeft,QString("%1").arg(ymin,0,'f',1));
        painter.drawText(bottom,Qt::AlignTop|Qt::AlignLeft,QString("%1").arg(xmin,0,'f',1));
        painter.drawText(bottom,Qt::AlignTop|Qt::AlignRight,QString("%1").arg(xmax,0,'f',1));

        painter.setBrush(QBrush(QColor(0,0,0),Qt::SolidPattern));
        for(size_t i=0;i<cpx.size();i++){
            painter.drawEllipse(toScreen(cpx[i],cpy[i]),2,2);
        }
    }

#ifdef SEEXPR_USE_ANIMLIB
    void sample(const animlib::AnimCurve& curve){
        int numKeys=curve.getNumKeys();
        x.clear();y.clear();
        cpx.clear();cpy.clear();
        if(numKeys>0){
            const animlib::AnimKeyframe* key=&*curve.getFirstKey();
            xmin=key[0].getTime();
            xmax=key[numKeys-1].getTime();
            ymin=FLT_MAX;
            ymax=-FLT_MAX;
            
            for(int i=0;i<numKeys;i++){
                cpx.push_back(key[i].getTime());
                cpy.push_back(key[i].getValue());
            }

            int nsamples=100;
            float dx=(xmax-xmin)/nsamples;
            float xeval=xmin;
            for(int i=0;i<nsamples;i++){
                x.push_back(xeval);
                float yeval=curve.getValue(xeval);
                ymin=std::min(ymin,yeval);
                ymax=std::max(ymax,yeval);
                y.push_back(yeval);
                xeval+=dx;
            }
            // pad window AFTER sampling
            //std::cerr<<"we have xmin xmax ymin ymax "<<xmin<<" "<<xmax<<" "<<ymin<<" "<<ymax<<std::endl;
        }else{
            xmin=-1;
            xmax=1;
            ymin=-1;
            ymax=1;
        }
        //std::cerr<<"we have xmin xmax ymin ymax "<<xmin<<" "<<xmax<<" "<<ymin<<" "<<ymax<<std::endl;
    }
#endif
};

SeExprEdAnimCurveControl::SeExprEdAnimCurveControl(int id,SeExprEdAnimCurveEditable* editable)
:SeExprEdControl(id,editable,false),_editable(editable)
{

    _preview=new SeExprEdGraphPreview();
    _preview->setMinimumWidth(200);
    _preview->setMinimumHeight(60);
    hbox->addWidget(_preview);
    //QPushButton* button=new QPushButton();
    //button->setIcon(QIcon(QPixmap(graphXPM)));
    Q_UNUSED(graphXPM)
    QPushButton* refreshButton=new QPushButton();
    refreshButton->setMaximumWidth(30);
    refreshButton->setIcon(QIcon(QPixmap(refreshXPM)));
    QPushButton* expandButton=new QPushButton(">");
    expandButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    expandButton->setFixedWidth(15);
    hbox->addWidget(expandButton);
    //hbox->addWidget(button);
    refreshButton->setVisible(_editable->link!="");
    hbox->addWidget(refreshButton);

#ifdef SEEXPR_USE_ANIMLIB
    _preview->sample(editable->curve);
#endif
    connect(expandButton,SIGNAL(clicked()),this,SLOT(editGraphClicked()));
    //connect(_preview,SIGNAL(clicked()),this,SLOT(editGraphClicked()));
    connect(refreshButton,SIGNAL(clicked()),this,SLOT(refreshClicked()));
}

#ifdef SEEXPR_USE_ANIMLIB
#include <CE/CETool.h>
#endif

void SeExprEdAnimCurveControl::refreshClicked()
{
    if(callback){
#ifdef SEEXPR_USE_ANIMLIB
        callback(_editable->link,_editable->curve);
        _preview->sample(_editable->curve);
        _preview->repaint();
        emit controlChanged(_id);
#endif
    }
}

void SeExprEdAnimCurveControl::editGraphClicked(){
#ifdef SEEXPR_USE_ANIMLIB
    QDialog* dialog=new QDialog(this);
    CETool* tool=new CETool;
    animlib::AnimAttrID attr1("","");
    animlib::AnimCurve& anim=*new animlib::AnimCurve(attr1);
    anim=_editable->curve;
    
    QWidget *widg;
    tool->map(widg,0);
    QVBoxLayout* layout=new QVBoxLayout();
    dialog->resize(QSize(1024,640));
    dialog->setLayout(layout);
    layout->addWidget(widg);
    tool->addCurve(&anim);

    QDialogButtonBox* buttonbar=new QDialogButtonBox();
    buttonbar->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    connect(buttonbar,SIGNAL(accepted()),dialog,SLOT(accept()));
    connect(buttonbar,SIGNAL(rejected()),dialog,SLOT(reject()));
    layout->addWidget(buttonbar);

    if(dialog->exec()==QDialog::Accepted){
        // copy points back from child
        _editable->curve=anim;
        _preview->sample(_editable->curve);
        _preview->repaint();
        emit controlChanged(_id);
    }
#endif
}

void SeExprEdAnimCurveControl::setAnimCurveCallback(AnimCurveCallback newCallback)
{
    callback=newCallback;
}

SeExprEdAnimCurveControl::AnimCurveCallback SeExprEdAnimCurveControl::callback=0;












