#include "widget.h"
#include "ui_widget.h"
#include "aboutdialog.h"
#include <QFile>
#include <QScrollBar>
#include <QTimer>
#include <QCommandLineParser>
#include <QMessageBox>
#include <QMouseEvent>
#include <QTime>
#include <QTextStream>
#include <QDir>
#include <QProcess>
#include <QPainter>
#include <QPen>
#include <QtMath>
#include <QMenu>

#define MSG_HEIGHT 20
#define MSG_FONT_SIZE 9
const QString edit_font_name="微软雅黑";
const QString msg_font_name="微软雅黑";

QString programPath;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    noteName="";

    trueWidth=this->width()-10;
    trueHeight=this->height()-10;

    programPath=qApp->applicationDirPath();

    edit = new noteEdit(this);
    edit->move(trueXToX(0),trueYToY(20));
    edit->resize(200,228);
    QFont font(edit_font_name,11);
    edit->setFont(font);

    isLeftPress = false;
    this->dir = NONE;
    this->setMouseTracking(true);

    edit->setStyleSheet("background-color: rgb(233, 233, 23);"
                        "border:0px;");

    QFile Vscrollbarqss(":/qss/Vscrollbar.qss");
    Vscrollbarqss.open(QFile::ReadOnly);
    edit->verticalScrollBar()->setStyleSheet(Vscrollbarqss.readAll());
    Vscrollbarqss.close();
    QFile Hscrollbarqss(":/qss/Hscrollbar.qss");
    Hscrollbarqss.open(QFile::ReadOnly);
    edit->horizontalScrollBar()->setStyleSheet(Hscrollbarqss.readAll());
    Hscrollbarqss.close();

    closeButton = new flatButton(this);
    closeButton->setPixmap(QPixmap(":/images/button/cross.png"));
    closeButton->setAlignment(Qt::AlignCenter);
    closeButton->move(trueXToX(180),trueYToY(0));
    closeButton->resize(20,20);
    closeButton->setToolTip("关闭");
    closeButton->show();
    connect(closeButton,SIGNAL(clicked()),this,SLOT(closeClicked()));
    connect(closeButton,SIGNAL(mousePressed()),this,SLOT(closeButtonPressed()));
    connect(closeButton,SIGNAL(mouseReleased()),this,SLOT(closeButtonReleased()));

    delButton = new flatButton(this);
    delButton->setPixmap(QPixmap(":/images/button/delete.png"));
    delButton->setAlignment(Qt::AlignCenter);
    delButton->move(trueXToX(160),trueYToY(0));
    delButton->resize(20,20);
    delButton->setToolTip("删除该便笺");
    delButton->show();
    connect(delButton,SIGNAL(clicked()),this,SLOT(delClickedF()));
    connect(delButton,SIGNAL(mousePressed()),this,SLOT(delButtonPressed()));
    connect(delButton,SIGNAL(mouseReleased()),this,SLOT(delButtonReleased()));

    newButton = new flatButton(this);
    newButton->setPixmap(QPixmap(":/images/button/plus.png"));
    newButton->setAlignment(Qt::AlignCenter);
    newButton->move(trueXToX(0),trueYToY(0));
    newButton->resize(20,20);
    newButton->setToolTip("新建便笺");
    newButton->show();
    connect(newButton,SIGNAL(clicked()),this,SLOT(newClicked()));
    connect(newButton,SIGNAL(mousePressed()),this,SLOT(newButtonPressed()));
    connect(newButton,SIGNAL(mouseReleased()),this,SLOT(newButtonReleased()));
    connect(newButton,SIGNAL(rightClicked()),this,SLOT(newRightClicked()));

    moreButton = new flatButton(this);
    moreButton->setPixmap(QPixmap(":/images/button/more_1.png"));
    moreButton->setAlignment(Qt::AlignCenter);
    moreButton->move(trueXToX(20),trueYToY(0));
    moreButton->resize(20,20);
    moreButton->setToolTip("更多");
    moreButton->show();
    connect(moreButton,SIGNAL(clicked()),this,SLOT(moreClicked()));
    connect(moreButton,SIGNAL(mousePressed()),this,SLOT(moreButtonPressed()));
    connect(moreButton,SIGNAL(mouseReleased()),this,SLOT(moreButtonReleased()));

    msgTimer=new QTimer(this);
    delClicked=false;
    connect(msgTimer,SIGNAL(timeout()),this,SLOT(delTimeout()));

    //-n选项代表新建便笺
    //-s -rgb互相不能同在
    //-r -g -b必须同在且必须与-n同在
    QCommandLineOption op1("n");//新建标签 new
    QCommandLineOption op2("s","","noteName");//加载指定便笺 set
    QCommandLineOption op3("r","","colorR");//设置新建便笺RGB的R
    QCommandLineOption op4("g","","colorG");//设置新建便笺RGB的G
    QCommandLineOption op5("b","","colorB");//设置新建便笺RGB的B
    QCommandLineParser parser;

    parser.addOption(op1);
    parser.addOption(op2);
    parser.addOption(op3);
    parser.addOption(op4);
    parser.addOption(op5);
    parser.process(*qApp);
    bool isSet1=parser.isSet(op1);
    bool isSet2=parser.isSet(op2);
    bool isSet3=parser.isSet(op3);
    bool isSet4=parser.isSet(op4);
    bool isSet5=parser.isSet(op5);
    if(isSet1&&isSet2==false&&isSet3==false&&isSet4==false&&isSet5==false)
    {
        this->newNote();
    }
    else if(isSet1==false&&isSet2&&isSet3==false&&isSet4==false&&isSet5==false)
    {
        this->readNote(parser.value(op2));
    }
    else if(isSet1&&isSet2==false&&isSet3&&isSet4&&isSet5)
    {
        this->newNote(QColor(parser.value(op3).toInt(),parser.value(op4).toInt(),parser.value(op5).toInt()));
    }
    else if(isSet1==false&&isSet2==false&&isSet3==false&&isSet4==false&&isSet5==false)
    {
        this->readAllNote();
    }
    else
    {
        QMessageBox::warning(this,"错误","命令行参数不正确",QMessageBox::Ok, QMessageBox::Ok);
        newNote();
    }

    connect(edit,SIGNAL(textChanged()),this,SLOT(editTextChanged()));


    menu=new QMenu(this);

    actionAbout=new QAction("关于",this);
    actionAbout->setShortcut(QKeySequence(Qt::Key_F1));
    actionInstructions=new QAction("使用说明",this);
    actionFgx=new QAction(this);
    actionFgx->setSeparator(true);
    menuDraw=new QMenu(menu);
    menuDraw->setTitle("涂鸦");
    actionDraw=new QAction("启用涂鸦模式",menuDraw);
    actionDraw->setCheckable(true);
    actionClear=new QAction("清空所有涂鸦",menuDraw);

    connect(actionAbout,SIGNAL(triggered(bool)),this,SLOT(moreAboutClicked()));
    connect(actionInstructions,SIGNAL(triggered(bool)),this,SLOT(moreInstructionsClicked()));
    connect(actionDraw,SIGNAL(toggled(bool)),this,SLOT(moreDrawClicked(bool)));
    connect(actionClear,SIGNAL(triggered(bool)),this,SLOT(moreClearClicked()));

    menu->addAction(actionAbout);
    menu->addAction(actionInstructions);
    menu->addAction(actionFgx);
    menu->addMenu(menuDraw);
    menuDraw->addAction(actionDraw);
    menuDraw->addAction(actionClear);

    connect(menu,SIGNAL(aboutToHide()),this,SLOT(changeMore1()));

    QFile menuQss(":/qss/menu.qss");
    menuQss.open(QFile::ReadOnly);
    menu->setStyleSheet(menuQss.readAll());
    menuQss.close();

    connect(this,SIGNAL(moreStateChange()),this,SLOT(setMorePixmap()));

    setAttribute(Qt::WA_TranslucentBackground);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::region(const QPoint &cursorGlobalPoint)
{
    QRect rect = this->rect();
    QPoint tl = mapToGlobal(rect.topLeft());
    QPoint rb = mapToGlobal(rect.bottomRight());

    int x = cursorGlobalPoint.x();
    int y = cursorGlobalPoint.y();

    if(tl.x() + PADDING >= x && tl.x() <= x && tl.y() + PADDING >= y && tl.y() <= y) {
        //左上角
        dir = LT;
        this->setCursor(QCursor(Qt::SizeFDiagCursor));
    } else if(x >= rb.x() - PADDING && x <= rb.x() && y >= rb.y() - PADDING && y <= rb.y()) {
        //右下角
        dir = RB;
        this->setCursor(QCursor(Qt::SizeFDiagCursor));
    } else if(x <= tl.x() + PADDING && x >= tl.x() && y >= rb.y() - PADDING && y <= rb.y()) {
        //左下角
        dir = LB;
        this->setCursor(QCursor(Qt::SizeBDiagCursor));
    } else if(x <= rb.x() && x >= rb.x() - PADDING && y >= tl.y() && y <= tl.y() + PADDING) {
        //右上角
        dir = RT;
        this->setCursor(QCursor(Qt::SizeBDiagCursor));
    } else if(x <= tl.x() + PADDING && x >= tl.x()) {
        //左边
        dir = LEFT;
        this->setCursor(QCursor(Qt::SizeHorCursor));
    } else if( x <= rb.x() && x >= rb.x() - PADDING) {
        //右边
        dir = RIGHT;
        this->setCursor(QCursor(Qt::SizeHorCursor));
    }else if(y >= tl.y() && y <= tl.y() + PADDING){
        //上边
        dir = TOP;
        this->setCursor(QCursor(Qt::SizeVerCursor));
    } else if(y <= rb.y() && y >= rb.y() - PADDING) {
        //下边
        dir = BOTTOM;
        this->setCursor(QCursor(Qt::SizeVerCursor));
    }else {
        //默认
        dir = NONE;
        this->setCursor(QCursor(Qt::ArrowCursor));
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        isLeftPress = false;
        if(dir != NONE)
        {
            this->releaseMouse();
            this->setCursor(QCursor(Qt::ArrowCursor));
        }
    }
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    switch(event->button())
    {
    case Qt::LeftButton:
        isLeftPress = true;
        if(dir != NONE)
        {
            this->mouseGrabber();
        }
        else
        {
            dragPosition = event->globalPos() - this->frameGeometry().topLeft();
        }
        break;
    case Qt::RightButton:
        break;
    default:
        QWidget::mousePressEvent(event);
    }

}

void Widget::setEditColor(QColor color)
{
    QString red=QString::number(color.red());
    QString green=QString::number(color.green());
    QString blue=QString::number(color.blue());
    this->setStyleSheet("background-color: rgb("+red+","+green+","+blue+");");
    edit->setStyleSheet("background-color: rgb("+red+","+green+","+blue+");"
                        "border:0px;");
    editColor=color;
    QFile nCfg(programPath+"/notes/"+noteName+".color");
    nCfg.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&nCfg);
    out << QString("%1,%2,%3").arg(red,green,blue);
    nCfg.close();
}

void Widget::setEditColor(QString color)
{
    this->setStyleSheet("background-color: rgb("+color+");");
    edit->setStyleSheet("background-color: rgb("+color+");"
                        "border:0px;");
    QStringList strList=color.split(",");
    QString red1=strList.at(0);
    QString green1=strList.at(1);
    QString blue1=strList.at(2);
    int red=red1.toInt();
    int green=green1.toInt();
    int blue=blue1.toInt();
    editColor=QColor(red,green,blue);
    QFile nCfg(programPath+"/notes/"+noteName+".color");
    nCfg.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&nCfg);
    out << color;
    nCfg.close();
}

void Widget::readNote(QString noteNamea)
{
    noteName=noteNamea;
    QFile noteFile(programPath+"/notes/"+noteName);
    noteFile.open(QFile::ReadOnly);
    QTextStream in(&noteFile);
    QString noteHtml=removeHtml(in.readAll());
    edit->setHtml(noteHtml);
    noteFile.close();

    QFile noteColorFile(programPath+"/notes/"+noteName+".color");
    noteColorFile.open(QFile::ReadOnly);
    QTextStream colorIn(&noteColorFile);
    setEditColor(colorIn.readAll());
    noteColorFile.close();

    QFile drawFile(programPath+"/notes/"+noteName+".draw");
    drawFile.open(QIODevice::ReadOnly);
    QDataStream drawFileIn(&drawFile);
    quint32 m;
    quint32 v;
    drawFileIn>>m;
    drawFileIn>>v;
    if(m!=0xA0B0C0D0)
    {
        QMessageBox::warning(this,"警告","涂鸦内容损坏");
    }
    else if(v!=10200)
    {
        QMessageBox::warning(this,"警告","涂鸦内容无法读取，请使用正确版本读取");
    }
    else
    {
        drawFileIn>>edit->painterPath;
        edit->update();
    }
}
void Widget::readAllNote()
{
    QDir dir(programPath+"/notes");
    dir.setFilter(QDir::Files | QDir::Hidden |QDir::NoSymLinks);
    dir.setSorting(QDir::Size|QDir::Reversed);
    QFileInfoList fileInfo = dir.entryInfoList();
    int c=fileInfo.count();
    foreach (QFileInfo info, fileInfo)
    {
        QFileInfo tempInfo1(programPath+"/notes/"+info.fileName()+".color");
        QFileInfo tempInfo2(programPath+"/notes/"+info.fileName()+".draw");
        if(tempInfo1.isFile()&&tempInfo2.isFile())
        {
            QProcess::startDetached("\""+qApp->applicationFilePath()+"\" -s "+info.fileName());
        }
        else
        {
            c--;
        }
    }
    if(c<=0)
    {
        newNote();
    }
    else
    {
        QTimer::singleShot(0, qApp, SLOT(quit()));
    }
}

void Widget::newNote(QColor color)
{
    noteName=QString::number(random());
    QFileInfo fi(programPath+"/notes");
    QDir dir;
    if(!fi.isDir()){dir.mkpath(programPath+"/notes");}
    fi.setFile(programPath+"/notes/"+noteName);
    while(fi.isFile())
    {
        noteName=QString::number(random());
        fi.setFile(programPath+"/notes/"+noteName);
    }
    QFile nFile(programPath+"/notes/"+noteName);
    nFile.open(QIODevice::WriteOnly | QIODevice::Text);
    nFile.close();
    setEditColor(color);
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint gloPoint = event->globalPos();
    QRect rect = this->rect();
    QPoint tl = mapToGlobal(rect.topLeft());
    QPoint rb = mapToGlobal(rect.bottomRight());
    if(!isLeftPress)
    {
        this->region(gloPoint);
    }
    else
    {
        if(dir != NONE)
        {
            QRect rMove(tl, rb);
            switch(dir)
            {
            case LEFT:
                if(rb.x() - gloPoint.x() <= this->minimumWidth())
                    rMove.setX(tl.x());
                else
                    rMove.setX(gloPoint.x());
                break;
            case RIGHT:
                rMove.setWidth(gloPoint.x() - tl.x());
                break;
            case TOP:
                if(rb.y() - gloPoint.y() <= this->minimumHeight())
                    rMove.setY(tl.y());
                else
                    rMove.setY(gloPoint.y());
                break;
            case BOTTOM:
                rMove.setHeight(gloPoint.y() - tl.y());
                break;
            case LT:
                if(rb.x() - gloPoint.x() <= this->minimumWidth())
                    rMove.setX(tl.x());
                else
                    rMove.setX(gloPoint.x());
                if(rb.y() - gloPoint.y() <= this->minimumHeight())
                    rMove.setY(tl.y());
                else
                    rMove.setY(gloPoint.y());
                break;
            case RT:
                rMove.setWidth(gloPoint.x() - tl.x());
                rMove.setY(gloPoint.y());
                break;
            case LB:
                rMove.setX(gloPoint.x());
                rMove.setHeight(gloPoint.y() - tl.y());
                break;
            case RB:
                rMove.setWidth(gloPoint.x() - tl.x());
                rMove.setHeight(gloPoint.y() - tl.y());
                break;
            default:
                break;
            }
            if(rMove.width()>90 && rMove.height()>50){this->setGeometry(rMove);}

        }
        else
        {
            move(event->globalPos() - dragPosition);
            event->accept();
        }
    }
    QWidget::mouseMoveEvent(event);
}

void Widget::resizeEvent(QResizeEvent *event)
{
    trueWidth=event->size().width()-10;
    trueHeight=event->size().height()-10;
    edit->resize(trueWidth-2,trueHeight-20);
    closeButton->move(trueXToX(trueWidth-20),trueYToY(0));
    delButton->move(trueXToX(trueWidth-40),trueYToY(0));
    if(delClicked)
    {
        delMsg->move(trueXToX(1),trueYToY(trueHeight-MSG_HEIGHT-1));
        delMsg->resize(trueWidth-2,MSG_HEIGHT);
    }
}

void Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(editColor);
    painter.fillRect(trueXToX(0)+1,trueYToY(0),trueWidth,trueHeight+1,editColor);
    for(int i=0;i<=4;i++)
    {
        painter.setPen(QColor(150,150,150,51*i+51));
        painter.drawRect(i,i,this->width()-i*2,this->height()-i*2);
    }
}

void Widget::delClickedF()
{
    if (delClicked==false)
    {
        delMsg=new QLabel("确定要删除该便笺吗？再按一次删除",this);
        delClicked=true;
        delMsg->move(trueXToX(1),trueYToY(trueHeight-MSG_HEIGHT-1));
        delMsg->resize(trueWidth-2,MSG_HEIGHT);
        QFont msg_font(msg_font_name,MSG_FONT_SIZE);
        delMsg->setFont(msg_font);
        delMsg->setAlignment(Qt::AlignCenter);
        delMsg->show();
        for(int i=1;i<=5;i++)
        {
            delMsg->setStyleSheet("background-color: rgba(255, 255, 255, "+QString::number(i*10)+"%);"
                                  "color: rgba(0, 0, 0,"+QString::number(i*10)+"%);");
            sleep(100);
        }
        msgTimer->start(3000);
    }
    else
    {
        QFile::remove(programPath+"/notes/"+noteName);
        QFile::remove(programPath+"/notes/"+noteName+".color");
        QFile::remove(programPath+"/notes/"+noteName+".draw");
        this->close();
    }
}

void Widget::delRightClicked()
{

}

void Widget::delTimeout()
{
    for(int i=5;i>=1;i--)
    {
        delMsg->setStyleSheet("background-color: rgba(255, 255, 255, "+QString::number(i*10)+"%);"
                              "color: rgba(0, 0, 0,"+QString::number(i*10)+"%);");
        sleep(100);
    }
    delClicked=false;
    delMsg->close();
    msgTimer->stop();
}

void Widget::editTextChanged()
{
    QString outText=edit->toHtml();
    QFile nFile(programPath+"/notes/"+noteName);
    nFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&nFile);
    out<<outText;
    out.flush();
    nFile.close();
}

void Widget::closeClicked()
{
    this->close();
}

void Widget::closeButtonPressed()
{
    QString red=QString::number(qAbs(editColor.red()-10));
    QString green=QString::number(qAbs(editColor.green()-10));
    QString blue=QString::number(qAbs(editColor.blue()-10));
    closeButton->setStyleSheet("QLabel{border:2px solid rgb("+red+","+green+","+blue+");}");
}

void Widget::closeButtonReleased()
{
    QString red=QString::number(editColor.red());
    QString green=QString::number(editColor.green());
    QString blue=QString::number(editColor.blue());
    closeButton->setStyleSheet("QLabel{border:0px solid rgb("+red+","+green+","+blue+");}");
}

void Widget::delButtonPressed()
{
    QString red=QString::number(qAbs(editColor.red()-10));
    QString green=QString::number(qAbs(editColor.green()-10));
    QString blue=QString::number(qAbs(editColor.blue()-10));
    delButton->setStyleSheet("QLabel{border:2px solid rgb("+red+","+green+","+blue+");}");
}

void Widget::delButtonReleased()
{
    QString red=QString::number(editColor.red());
    QString green=QString::number(editColor.green());
    QString blue=QString::number(editColor.blue());
    delButton->setStyleSheet("QLabel{border:0px solid rgb("+red+","+green+","+blue+");}");
}

void Widget::newClicked()
{
    QString red=QString::number(editColor.red());
    QString green=QString::number(editColor.green());
    QString blue=QString::number(editColor.blue());
    QProcess::startDetached("\""+qApp->applicationFilePath()+"\" -n -r "+red+" -g "+green+" -b "+blue);
}

void Widget::newRightClicked()
{
}

void Widget::newButtonPressed()
{
    QString red=QString::number(qAbs(editColor.red()-10));
    QString green=QString::number(qAbs(editColor.green()-10));
    QString blue=QString::number(qAbs(editColor.blue()-10));
    newButton->setStyleSheet("QLabel{border:2px solid rgb("+red+","+green+","+blue+");}");
}

void Widget::newButtonReleased()
{
    QString red=QString::number(editColor.red());
    QString green=QString::number(editColor.green());
    QString blue=QString::number(editColor.blue());
    newButton->setStyleSheet("QLabel{border:0px solid rgb("+red+","+green+","+blue+");}");
}

void Widget::moreClicked()
{
    moreClickedBool=true;
    emit moreStateChange();
    menu->exec(this->mapToGlobal(QPoint(moreButton->x(),moreButton->height()+5)));
}

void Widget::moreButtonPressed()
{
    QString red=QString::number(qAbs(editColor.red()-10));
    QString green=QString::number(qAbs(editColor.green()-10));
    QString blue=QString::number(qAbs(editColor.blue()-10));
    moreButton->setStyleSheet("QLabel{border:2px solid rgb("+red+","+green+","+blue+");}");
}

void Widget::moreButtonReleased()
{
    QString red=QString::number(editColor.red());
    QString green=QString::number(editColor.green());
    QString blue=QString::number(editColor.blue());
    moreButton->setStyleSheet("QLabel{border:0px solid rgb("+red+","+green+","+blue+");}");
}

void Widget::moreAboutClicked()
{
    aboutDialog *dialog=new aboutDialog(this);
    dialog->show();
}

void Widget::moreInstructionsClicked()
{
    QFileInfo fileInfoi(programPath+"/instructions");
    QFileInfo fileInfoc(programPath+"/instructions.color");
    QFileInfo fileInfod(programPath+"/instructions.draw");
    if(!fileInfoi.isFile()||!fileInfoc.isFile()||!fileInfod.isFile())
    {
        QFile file(":/other/instructions");
        file.open(QFile::ReadOnly);
        QString iString=file.readAll();
        file.close();

        file.setFileName(":/other/instructions.color");
        file.open(QFile::ReadOnly);
        QString cString=file.readAll();
        file.close();

        file.setFileName(":/other/instructions.draw");
        file.open(QFile::ReadOnly);
        QString dString=file.readAll();
        file.close();

        QFile iFile(programPath+"/instructions");
        iFile.open(QIODevice::WriteOnly);
        iFile.close();
        iFile.open(QIODevice::WriteOnly);
        QTextStream iOut(&iFile);
        iOut<<iString;
        iFile.close();

        QFile cFile(programPath+"/instructions.color");
        cFile.open(QIODevice::WriteOnly);
        cFile.close();
        cFile.open(QIODevice::WriteOnly);
        QTextStream cOut(&cFile);
        cOut<<cString;
        cFile.close();

        QFile dFile(programPath+"/instructions.draw");
        dFile.open(QIODevice::WriteOnly);
        dFile.close();
        dFile.open(QIODevice::WriteOnly);
        QTextStream dOut(&dFile);
        dOut<<dString;
        dFile.close();
    }
    QProcess::startDetached("\""+qApp->applicationFilePath()+"\" -s ../instructions");
}

void Widget::moreDrawClicked(bool b)
{
    edit->canDraw=b;
}

void Widget::moreClearClicked()
{
    edit->painterPath=QPainterPath();
    edit->saveDraw();
    edit->update();
}

void Widget::setMorePixmap()
{
    if(moreClickedBool)
    {
        moreButton->setPixmap(QPixmap(":/images/button/more_2.png"));
    }
    else
    {
        moreButton->setPixmap(QPixmap(":/images/button/more_1.png"));
    }
}

void Widget::changeMore1()
{
    moreClickedBool=false;
    emit moreStateChange();
}

int Widget::random()
{
    QTime t;
    t=QTime::currentTime();
    qsrand(t.msec()+t.second()*1000);
    return qrand();
}

QString Widget::removeHtml(QString html)
{
    QRegExp regexp("<(?!img|br|p|/p|style|/style|span|/span).*>");
    regexp.setCaseSensitivity(Qt::CaseInsensitive);
    regexp.setMinimal(true);
    QString ret=html;
    ret=ret.replace(regexp,"");
    return ret;
}

void Widget::sleep(unsigned int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

int Widget::trueXToX(int x)
{
    return x+5;
}

int Widget::trueYToY(int y)
{
    return y+5;
}
