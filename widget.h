#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#define PADDING 2
enum Direction { TOP, BOTTOM, LEFT, RIGHT, LT, LB, RT, RB, NONE };

#include "flatbutton.h"
#include "noteedit.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void setEditColor(QColor color = QColor(250,250,175));
    void setEditColor(QString color = "250,250,175");
    QString removeHtml(QString html);
    QString noteName;

private:
    Ui::Widget *ui;
    noteEdit *edit;
    flatButton *closeButton;
    flatButton *delButton;
    flatButton *newButton;
    flatButton *moreButton;
    bool delClicked;
    QTimer *msgTimer;
    QLabel *delMsg;
    QColor editColor;
    int trueWidth;
    int trueHeight;
    bool moreClickedBool;
    QMenu *menu;
    QAction *actionAbout;
    QAction *actionInstructions;
    QAction *actionFgx;
    QMenu *menuDraw;
    QAction *actionDraw;
    QAction *actionClear;
    void region(const QPoint &cursorGlobalPoint);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);
    void readNote(QString noteNamea);
    void readAllNote();
    void newNote(QColor color = QColor(255,255,205));
    void sleep(unsigned int msec);
    int trueXToX(int x);
    int trueYToY(int y);
    int random();
    bool isLeftPress;
    QPoint dragPosition;
    Direction dir;
    QString programPath;
protected:
signals:
    void moreStateChange();
private slots:
    void delTimeout();
    void editTextChanged();

    void closeClicked();
    void closeButtonPressed();
    void closeButtonReleased();

    void delClickedF();
    void delRightClicked();
    void delButtonPressed();
    void delButtonReleased();

    void newClicked();
    void newRightClicked();
    void newButtonPressed();
    void newButtonReleased();

    void moreClicked();
    void moreButtonPressed();
    void moreButtonReleased();

    void moreAboutClicked();
    void moreInstructionsClicked();
    void moreDrawClicked(bool b);
    void moreClearClicked();

    void setMorePixmap();
    void changeMore1();
};

#endif // WIDGET_H
