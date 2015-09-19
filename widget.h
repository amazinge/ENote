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
    void setEditColor(QColor color = QColor(250,250,175));//用QColor设置便笺颜色
    void setEditColor(QString color = "250,250,175");//用以逗号分割rgb的字符串设置便笺颜色
    QString noteName;//便笺名

private:
    Ui::Widget *ui;
    noteEdit *edit;//编辑框
    flatButton *closeButton;//关闭按钮
    flatButton *delButton;//删除按钮
    flatButton *newButton;//新建按钮
    flatButton *moreButton;//更多按钮
    bool delClicked;//删除按钮是否被按下
    QTimer *msgTimer;//消息显示计时器
    QLabel *delMsg;//“删除”消息
    QColor editColor;//便笺颜色
    int trueWidth;//除阴影外的宽度
    int trueHeight;//除阴影外的高度
    bool moreClickedBool;//更多按钮是否被按下
    QMenu *menu;//“更多”按钮的下拉菜单
    QAction *actionAbout;//关于
    QAction *actionInstructions;//使用说明
    QAction *actionFgx;//分割线
    QMenu *menuDraw;//涂鸦菜单
    QAction *actionDraw;//涂鸦
    QAction *actionClear;//清空
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
    bool isLeftPress;//左键是否按下
    QPoint dragPosition;
    Direction dir;
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
