#ifndef NOTEEDIT_H
#define NOTEEDIT_H

#include <QTextEdit>
#include <QMimeData>
#include <QShortcut>

class noteEdit : public QTextEdit
{
    Q_OBJECT
public:
    noteEdit(QWidget *parent = 0);
    bool canInsertFromMimeData(const QMimeData * source) const;
    void insertFromMimeData(const QMimeData * source);
    void contextMenuEvent(QContextMenuEvent *e);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void saveDraw();
    bool ctrlPress;
    bool canDraw;
    bool pressed;
    bool startDraw;
    QPainterPath painterPath;
    QPointF lastPoint;
private:
    void dropImage(const QUrl& url, const QImage& image);
    void dropTextFile(const QUrl& url);
    void setFormat(int format,bool b);//0:粗体 1:斜体 2:下划线 3:上划线 4:删除线
    void paintEvent(QPaintEvent *event);
    QShortcut *shortcutBold;
    QShortcut *shortcutItalic;
    QShortcut *shortcutUnderLine;
    QShortcut *shortcutOverLine;
    QShortcut *shortcutStrikeOut;
    QShortcut *shortcutLeft;
    QShortcut *shortcutRight;
    QShortcut *shortcutCenter;
    QMenu *menu;
    QAction *actionCopy;
    QAction *actionPaste;
    QAction *actionCut;
    QAction *actionDel;
    QAction *actionFgx1;
    QAction *actionAll;
    QAction *actionFgx2;
    QMenu *menuSetColor;
    QAction *actionYellow;
    QAction *actionGrey;
    QAction *actionBlue;
    QAction *actionGreen;
    QAction *actionPink;
    QAction *actionCustomColor;
private slots:
    void delSelected();
    void colorYellow();
    void colorGrey();
    void colorBlue();
    void colorGreen();
    void colorPink();
    void colorCustom();

    void setBold();
    void setItalic();
    void setUnderLine();
    void setOverLine();
    void setStrikeOut();

    void setLeft();
    void setRight();
    void setCenter();
};

#endif // NOTEEDIT_H
