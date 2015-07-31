#ifndef _SeExprEdColorSwatchWidget_h_
#define _SeExprEdColorSwatchWidget_h_

#include <vector>
#include <QtCore/QObject>
#include <SeVec3d.h>
#include <QtGui/QFrame>
#include <QtGui/QWidget>

class QGridLayout;

class SeExprEdColorFrame : public QFrame
{
    Q_OBJECT
public:
    SeExprEdColorFrame(SeVec3d value, QWidget* parent = 0);
    ~SeExprEdColorFrame() {}

    void setValue(const SeVec3d &value);
    SeVec3d getValue() const;
    bool selected() {return _selected;};

protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);

private slots:
    void deleteSwatchMenu(const QPoint &pos);

signals:
    void selValChangedSignal(SeVec3d value);
    void swatchChanged(QColor color);
    void deleteSwatch(SeExprEdColorFrame *swatch);

private:
    SeVec3d _value;
    QColor _color;
    bool _selected;
};

// Simple color widget with or without index label
class SeExprEdColorWidget : public QWidget
{
    Q_OBJECT
public:
    SeExprEdColorWidget(SeVec3d value, int index, bool indexLabel, QWidget* parent);
    SeExprEdColorFrame *getColorFrame();
private:
    SeExprEdColorFrame *_colorFrame;
};


class SeExprEdColorSwatchWidget : public QWidget
{
    Q_OBJECT

public:
    SeExprEdColorSwatchWidget(bool indexLabel, QWidget* parent = 0);
    ~SeExprEdColorSwatchWidget() {}

    // Convenience Functions
    void addSwatch(SeVec3d &val, int index=-1);
    void setSwatchColor(int index, QColor color);
    QColor getSwatchColor(int index);

private slots:
    void addNewColor();
    void removeSwatch(SeExprEdColorFrame *);
    void internalSwatchChanged(QColor color);

signals:
    void selValChangedSignal(SeVec3d val);
    void swatchChanged(int index, SeVec3d val);
    void swatchAdded(int index, SeVec3d val);
    void swatchRemoved(int index);

private:
    QGridLayout *_gridLayout;
    int _columns;
    bool _indexLabel;
};
#endif

