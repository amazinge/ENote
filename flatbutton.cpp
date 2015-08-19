#include "flatbutton.h"
#include <QMouseEvent>

flatButton::flatButton(QWidget *parent) : QLabel(parent)
{
    pressed=false;
    this->setMouseTracking(true);
}

void flatButton::mousePressEvent(QMouseEvent *e)
{
    pressed=true;
    emit mousePressed();
}

void flatButton::mouseReleaseEvent(QMouseEvent *e)
{
    emit mouseReleased();
    QRect rect=this->geometry();
    QPoint mousePos=this->mapFromGlobal(QCursor::pos());
    if(pressed && (abs(mousePos.x()))<=rect.size().width() &&
            (abs(mousePos.y()))<=rect.size().height())
    {
        if(e->button()==Qt::LeftButton)
        {
            emit clicked();
        }
        else if(e->button()==Qt::RightButton)
        {
            emit rightClicked();
        }
    }
    pressed=false;
}

void flatButton::mouseMoveEvent(QMouseEvent *e)
{
    emit mouseMoved();
}
