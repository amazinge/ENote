#ifndef FLATBUTTON_H
#define FLATBUTTON_H

#include <QLabel>

class flatButton : public QLabel
{
    Q_OBJECT
public:
    flatButton(QWidget *parent = 0);
    bool pressed;
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);

signals:
    void clicked();
    void rightClicked();
    void mousePressed();
    void mouseReleased();
    void mouseMoved();

public slots:
};

#endif // FLATBUTTON_H
