#ifndef CANVAS_H
#define CANVAS_H

#include <QLabel>
#include <QMouseEvent>

QT_BEGIN_NAMESPACE

class Canvas : public QLabel
{
    Q_OBJECT
public:
    Canvas(QWidget *parent);
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void setImage(QImage image);
signals:
    void mousePressed(int x, int y);
    void mouseMoved(int x, int y);
};

QT_END_NAMESPACE

#endif //CANVAS_H