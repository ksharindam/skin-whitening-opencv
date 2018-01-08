#include "canvas.h"

Canvas:: Canvas(QWidget *parent) : QLabel(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setMouseTracking(true);
}

void
Canvas:: mousePressEvent(QMouseEvent *ev)
{
    emit mousePressed(ev->x(), ev->y());
}

void
Canvas:: mouseMoveEvent(QMouseEvent *ev)
{
    emit mouseMoved(ev->x(), ev->y());
}

void
Canvas:: setImage(QImage image)
{
    setPixmap(QPixmap::fromImage(image));
}