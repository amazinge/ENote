#include "widget.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator *tran=new QTranslator(&a);
    tran->load(":/other/qt_zh_CN.qm");
    a.installTranslator(tran);
    Widget w;
    w.setWindowOpacity(1);
    w.setWindowFlags(Qt::FramelessWindowHint);
    w.setMouseTracking(true);
    w.show();

    return a.exec();
}
