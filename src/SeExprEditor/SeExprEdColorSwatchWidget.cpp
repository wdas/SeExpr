#include <iostream>
#include <sstream>
#include <algorithm>

#include <QtGui/QColorDialog>
#include <QtGui/QDoubleValidator>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QResizeEvent>
#include <QtGui/QPushButton>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QPainter>
#include <QtGui/QMenu>
#include <QtGui/QLabel>

#include <SeExprBuiltins.h>
#ifdef SEEXPR_USE_QDGUI
#   include <qdgui/QdColorPickerDialog.h>
#endif

#include "SeExprEdColorSwatchWidget.h"

// Simple color frame for swatch
SeExprEdColorFrame::SeExprEdColorFrame(SeVec3d value, QWidget* parent) :
    QFrame(parent), _value(value)
{
    setValue(_value);
    setFrameStyle(QFrame::Box | QFrame::Plain);
    QPalette pal = palette();
    pal.setColor(backgroundRole(), pal.highlight().color());
    setPalette(pal);
    setAutoFillBackground(true);
}

void SeExprEdColorFrame::setValue(const SeVec3d &value)
{
    _color = QColor(int(255 * value[0] + 0.5),
                    int(255 * value[1] + 0.5),
                    int(255 * value[2] + 0.5));
    _value = value;
    update();
}

SeVec3d SeExprEdColorFrame::getValue() const
{
    return _value;
}

void SeExprEdColorFrame::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.fillRect(contentsRect(),_color);
}

void SeExprEdColorFrame::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
        deleteSwatchMenu(event->pos());
    else {

#ifdef SEEXPR_USE_QDGUI
        QColor color = QdColorPickerDialog::chooseColorFromDialog(_color,this);
#else
        QColor color = QColorDialog::getColor(_color);
#endif

        if (color.isValid()) {
            _value[0] = color.red() / 255.0;
            _value[1] = color.green() / 255.0;
            _value[2] = color.blue() / 255.0;
            update();
            _color = color;
            emit selValChangedSignal(_value);
            emit swatchChanged(color);
        }
    }
}

void SeExprEdColorFrame::deleteSwatchMenu(const QPoint &pos)
{
    QMenu *menu = new QMenu(this);
    QAction *deleteAction = menu->addAction("Delete Swatch");
    menu->addAction("Cancel");
    QAction *action = menu->exec(mapToGlobal(pos));
    if (action == deleteAction)
        emit deleteSwatch(this);
}

// Simple color widget with or without index label
SeExprEdColorWidget::SeExprEdColorWidget(SeVec3d value, int index,
                                         bool indexLabel, QWidget* parent) :
    QWidget(parent)
{
    _colorFrame = new SeExprEdColorFrame(value);
    _colorFrame->setFixedWidth(32);
    _colorFrame->setFixedHeight(16);

    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->setContentsMargins(0,0,0,0);
    vbox->setSpacing(0);
    vbox->addWidget(_colorFrame);

    if (indexLabel){
        std::stringstream indexSS;
        indexSS << index;
        QLabel *label = new QLabel(indexSS.str().c_str());
        vbox->addWidget(label);
    }

    setLayout(vbox);
    //emit swatchAdded(index, val);
}

SeExprEdColorFrame * SeExprEdColorWidget::getColorFrame()
{
    return _colorFrame;
}

// Grid layout of color swatches
SeExprEdColorSwatchWidget::SeExprEdColorSwatchWidget(bool indexLabel, QWidget* parent) :
    QWidget(parent), _columns(8), _indexLabel(indexLabel)
{
    QHBoxLayout *hboxLayout = new QHBoxLayout();
    hboxLayout->setContentsMargins(0,0,0,0);
    setLayout(hboxLayout);

    QPushButton *addBtn = new QPushButton("+");
    addBtn->setFixedWidth(16);
    addBtn->setFixedHeight(16);
    QVBoxLayout *swatchControlLayout = new QVBoxLayout();
    swatchControlLayout->setContentsMargins(0,0,0,0);
    QHBoxLayout *addRemoveBtnLayout = new QHBoxLayout();
    addRemoveBtnLayout->setContentsMargins(0,0,0,0);
    addRemoveBtnLayout->setSpacing(0);
    addRemoveBtnLayout->addWidget(addBtn);
    swatchControlLayout->addLayout(addRemoveBtnLayout);
    swatchControlLayout->addStretch();

    QHBoxLayout *paletteLayout = new QHBoxLayout();
    paletteLayout->setContentsMargins(0,0,0,0);
    QWidget *colorGrid = new QWidget();
    colorGrid->setMinimumWidth(256);
    _gridLayout = new QGridLayout();
    _gridLayout->setContentsMargins(0,0,0,0);
    _gridLayout->setSpacing(0);
    paletteLayout->addLayout(_gridLayout);
    paletteLayout->addStretch();
    colorGrid->setLayout(paletteLayout);

    hboxLayout->addWidget(colorGrid);
    hboxLayout->addLayout(swatchControlLayout);
    hboxLayout->addStretch();

    // SIGNALS
    connect(addBtn, SIGNAL(clicked()), this, SLOT(addNewColor()));
}

void SeExprEdColorSwatchWidget::addNewColor()
{
    SeVec3d val(.5);
    addSwatch(val, -1);
}

void SeExprEdColorSwatchWidget::addSwatch(SeVec3d &val, int index)
{
    if(index == -1 || index > _gridLayout->count())
        index = _gridLayout->count();
    SeExprEdColorWidget *widget = new SeExprEdColorWidget(val, index,
                                                          _indexLabel, this);
    SeExprEdColorFrame *swatchFrame = widget->getColorFrame();
    _gridLayout->addWidget(widget,index/_columns,index%_columns);
    connect(swatchFrame,SIGNAL(swatchChanged(QColor)),
            this,SLOT(internalSwatchChanged(QColor)));
    connect(swatchFrame,SIGNAL(deleteSwatch(SeExprEdColorFrame *)),
            this,SLOT(removeSwatch(SeExprEdColorFrame *)));
    emit swatchAdded(index, val);
}

void SeExprEdColorSwatchWidget::internalSwatchChanged(QColor newcolor)
{
    Q_UNUSED(newcolor);
    SeExprEdColorFrame *swatchFrame = (SeExprEdColorFrame *)sender();
    SeVec3d value=swatchFrame->getValue();
    int index = _gridLayout->indexOf(swatchFrame->parentWidget());
    emit swatchChanged(index, value);
}

void SeExprEdColorSwatchWidget::removeSwatch(SeExprEdColorFrame *widget)
{
    QWidget * parentWidget = widget->parentWidget();
    // Find given widget to remove from grid layout
    for (int i=0; i<_gridLayout->count(); i++){
        if (_gridLayout->itemAt(i)->widget() == parentWidget){
            _gridLayout->removeWidget(parentWidget);
            parentWidget->deleteLater();
            emit swatchRemoved(i);
            break;
        }
    }
}

void SeExprEdColorSwatchWidget::setSwatchColor(int index, QColor color)
{
    if(index >=0 && index < _gridLayout->count()){
        SeVec3d newColor(color.redF(),color.greenF(),color.blueF());
        QLayoutItem *layoutItem = _gridLayout->itemAt(index);
        if (layoutItem && layoutItem->widget()){
            QWidget *widget = layoutItem->widget();
            SeExprEdColorFrame *cFrame = ((SeExprEdColorWidget *)widget)->getColorFrame();
            cFrame->setValue(newColor);
        }
    }
}

QColor SeExprEdColorSwatchWidget::getSwatchColor(int index)
{
    if(index >=0 && index < _gridLayout->count()){
        QLayoutItem *layoutItem = _gridLayout->itemAt(index);
        if (layoutItem && layoutItem->widget()){
            QWidget *widget = layoutItem->widget();
            SeExprEdColorFrame *cFrame = ((SeExprEdColorWidget *)widget)->getColorFrame();
            SeVec3d val = cFrame->getValue();
            return QColor::fromRgbF(val[0],val[1],val[2],1);
        }
    }
    return QColor();
}

