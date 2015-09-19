#include "noteedit.h"
#include "widget.h"
#include <QMenu>
#include <QTextCursor>
#include <QFile>
#include <QFileInfo>
#include <QImageReader>
#include <QColorDialog>
#include <QMessageBox>
#include <QShortcut>
#include <QWheelEvent>
#include <QPainter>
#include <QDataStream>
#include <QDebug>

noteEdit::noteEdit(QWidget *parent) : QTextEdit(parent)
{

    canDraw=false;
    pressed=false;
    startDraw=true;
    ctrlPress=false;

    this->setMouseTracking(true);

    shortcutBold = new QShortcut(QKeySequence(Qt::ALT+Qt::Key_B),this);
    shortcutItalic = new QShortcut(QKeySequence(Qt::ALT+Qt::Key_I),this);
    shortcutUnderLine = new QShortcut(QKeySequence(Qt::ALT+Qt::Key_U),this);
    shortcutOverLine = new QShortcut(QKeySequence(Qt::ALT+Qt::Key_O),this);
    shortcutStrikeOut = new QShortcut(QKeySequence(Qt::ALT+Qt::Key_T),this);
    shortcutLeft = new QShortcut(QKeySequence(Qt::ALT+Qt::Key_J),this);
    shortcutCenter = new QShortcut(QKeySequence(Qt::ALT+Qt::Key_E),this);
    shortcutRight = new QShortcut(QKeySequence(Qt::ALT+Qt::Key_R),this);

    QObject::connect(shortcutBold, SIGNAL(activated()), this, SLOT(setBold()));
    QObject::connect(shortcutItalic, SIGNAL(activated()), this, SLOT(setItalic()));
    QObject::connect(shortcutUnderLine, SIGNAL(activated()), this, SLOT(setUnderLine()));
    QObject::connect(shortcutOverLine, SIGNAL(activated()), this, SLOT(setOverLine()));
    QObject::connect(shortcutStrikeOut, SIGNAL(activated()), this, SLOT(setStrikeOut()));
    QObject::connect(shortcutLeft, SIGNAL(activated()), this, SLOT(setLeft()));
    QObject::connect(shortcutRight, SIGNAL(activated()), this, SLOT(setRight()));
    QObject::connect(shortcutCenter, SIGNAL(activated()), this, SLOT(setCenter()));

    menu=new QMenu(this);

    actionCopy=new QAction("复制(&C)",this);
    actionCopy->setShortcut(QKeySequence::Copy);

    actionPaste=new QAction("粘贴(&P)",this);
    actionPaste->setShortcut(QKeySequence::Paste);

    actionCut=new QAction("剪切(&T)",this);
    actionCut->setShortcut(QKeySequence::Cut);

    actionDel=new QAction("删除(&D)",this);
    actionDel->setShortcut(QKeySequence::Delete);

    actionFgx1=new QAction(this);
    actionFgx1->setSeparator(true);

    actionAll=new QAction("全选(&A)",this);
    actionAll->setShortcut(QKeySequence::SelectAll);

    actionFgx2=new QAction(this);
    actionFgx2->setSeparator(true);

    menuSetColor=new QMenu(menu);
    menuSetColor->setTitle("设置背景颜色");

    actionYellow=new QAction("黄色",this);

    actionGrey=new QAction("灰色",this);

    actionBlue=new QAction("蓝色",this);

    actionGreen=new QAction("绿色",this);

    actionPink=new QAction("粉红色",this);

    actionCustomColor=new QAction("自定义颜色...",this);

    connect(actionCopy,SIGNAL(triggered(bool)),this,SLOT(copy()));
    connect(actionPaste,SIGNAL(triggered(bool)),this,SLOT(paste()));
    connect(actionCut,SIGNAL(triggered(bool)),this,SLOT(cut()));
    connect(actionDel,SIGNAL(triggered(bool)),this,SLOT(delSelected()));
    connect(actionAll,SIGNAL(triggered(bool)),this,SLOT(selectAll()));
    connect(actionYellow,SIGNAL(triggered(bool)),this,SLOT(colorYellow()));
    connect(actionGrey,SIGNAL(triggered(bool)),this,SLOT(colorGrey()));
    connect(actionBlue,SIGNAL(triggered(bool)),this,SLOT(colorBlue()));
    connect(actionGreen,SIGNAL(triggered(bool)),this,SLOT(colorGreen()));
    connect(actionPink,SIGNAL(triggered(bool)),this,SLOT(colorPink()));
    connect(actionCustomColor,SIGNAL(triggered(bool)),this,SLOT(colorCustom()));

    menu->addAction(actionCopy);
    menu->addAction(actionPaste);
    menu->addAction(actionCut);
    menu->addAction(actionDel);
    menu->addAction(actionFgx1);
    menu->addAction(actionAll);
    menu->addAction(actionFgx2);
    menu->addMenu(menuSetColor);
    menuSetColor->addAction(actionYellow);
    menuSetColor->addAction(actionGrey);
    menuSetColor->addAction(actionBlue);
    menuSetColor->addAction(actionGreen);
    menuSetColor->addAction(actionPink);
    menuSetColor->addAction(actionCustomColor);

    QFile menuQss(":/qss/menu.qss");
    menuQss.open(QFile::ReadOnly);
    menu->setStyleSheet(menuQss.readAll());
    menuQss.close();
}

bool noteEdit::canInsertFromMimeData(const QMimeData *source) const
{
    return true;
}

void noteEdit::insertFromMimeData(const QMimeData *source)
{
    if (source->hasImage())
    {
        static int i = 1;
        QUrl url(QString("dropped_image_%1").arg(i++));
        dropImage(url, qvariant_cast<QImage>(source->imageData()));
    }
    else if (source->hasUrls())
    {
        foreach (QUrl url, source->urls())
        {
            QFileInfo info(url.toLocalFile());
            if (QImageReader::supportedImageFormats().contains(info.suffix().toLower().toLatin1()))
                dropImage(url, QImage(info.filePath()));
            else
                dropTextFile(url);
        }
    }
    else if(source->hasHtml())
    {
        QRegExp regexp("<(?!img|br|p|/p|style|/style|span|/span).*>");
        regexp.setCaseSensitivity(Qt::CaseInsensitive);
        regexp.setMinimal(true);
        QString sourcehtml=source->html();
        sourcehtml=sourcehtml.replace(regexp,"");
        //regexp.setPattern("style=\"[^\"]*\"");
        //regexp.setMinimal(false);
        //sourcehtml=sourcehtml.replace(regexp,"");
        textCursor().insertHtml(sourcehtml);
    }
    else
    {
        QTextEdit::insertFromMimeData(source);
    }
}

void noteEdit::contextMenuEvent(QContextMenuEvent *e)
{
    menu->exec(QCursor::pos());
}

void noteEdit::wheelEvent(QWheelEvent *event)
{
    if(ctrlPress)
    {
        if(event->delta()>=0)
        {
            this->zoomIn();
        }
        else
        {
            this->zoomOut();
        }
    }
    else
    {
        QTextEdit::wheelEvent(event);
    }
}

void noteEdit::keyPressEvent(QKeyEvent *event)
{
    Qt::KeyboardModifiers modifiers=event->modifiers();
    if(modifiers&Qt::ControlModifier)
    {
        ctrlPress=true;
    }
    else
    {
        ctrlPress=false;
    }
    QTextEdit::keyPressEvent(event);
}

void noteEdit::keyReleaseEvent(QKeyEvent *event)
{
    Qt::KeyboardModifiers modifiers=event->modifiers();
    if(modifiers&Qt::ControlModifier)
    {
        ctrlPress=true;
    }
    else
    {
        ctrlPress=false;
    }
    QTextEdit::keyPressEvent(event);
}

void noteEdit::mousePressEvent(QMouseEvent *event)
{
    pressed=true;

    QTextEdit::mousePressEvent(event);
}

void noteEdit::mouseReleaseEvent(QMouseEvent *event)
{
    pressed=false;
    startDraw=true;
    saveDraw();

    QTextEdit::mouseReleaseEvent(event);
}

void noteEdit::mouseMoveEvent(QMouseEvent *event)
{
    if(canDraw&&pressed)
    {
        if(startDraw)
        {
            painterPath.moveTo(event->pos());
            startDraw=false;
        }
        painterPath.lineTo(event->pos());
    }
    update();

    QTextEdit::mouseMoveEvent(event);
}

void noteEdit::saveDraw()
{
    QFile file(qApp->applicationDirPath()+"/notes/"+((Widget*)parentWidget())->noteName+".draw");
    file.open(QIODevice::WriteOnly);
    file.close();
    file.open(QIODevice::WriteOnly);
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out<<(quint32)0xA0B0C0D0;
    out<<(qint32)10200;//1:固定头 0200：版本
    out.setVersion(QDataStream::Qt_5_5);
    out<<painterPath;
    file.write(data);
    file.close();
}

void noteEdit::paintEvent(QPaintEvent *event)
{
    QTextEdit::paintEvent(event);
    QPainter painter(viewport());
    painter.setPen(QPen(QColor(0,0,0)));
    painter.drawPath(painterPath);
    lastPoint=painterPath.currentPosition();
}

void noteEdit::dropImage(const QUrl& url, const QImage& image)
{
    if (!image.isNull())
    {
        document()->addResource(QTextDocument::ImageResource, url, image);
        textCursor().insertImage(url.toString());
    }
}

void noteEdit::dropTextFile(const QUrl& url)
{
    textCursor().insertHtml("<a href=\""+url.toString()+"\">"+url.toString()+"</a><p></p>");
    textCursor().anchor();
}

void noteEdit::setFormat(int format, bool b)
{
    QFont font;
    font=this->currentFont();
    switch(format)
    {
    case 0:
        font.setBold(b);
        break;
    case 1:
        font.setItalic(b);
        break;
    case 2:
        font.setUnderline(b);
        break;
    case 3:
        font.setOverline(b);
        break;
    case 4:
        font.setStrikeOut(b);
        break;
    }
    this->setCurrentFont(font);
}

void noteEdit::delSelected()
{
    this->textCursor().removeSelectedText();
}

void noteEdit::colorYellow()
{
    ((Widget*)parentWidget())->setEditColor(QColor(255,255,205));
}

void noteEdit::colorGrey()
{
    ((Widget*)parentWidget())->setEditColor(QColor(233,233,233));
}

void noteEdit::colorBlue()
{
    ((Widget*)parentWidget())->setEditColor(QColor(155,155,255));
}

void noteEdit::colorGreen()
{
    ((Widget*)parentWidget())->setEditColor(QColor(120,233,120));
}

void noteEdit::colorPink()
{
    ((Widget*)parentWidget())->setEditColor(QColor(255,200,200));
}

void noteEdit::colorCustom()
{
    QColor customColor=QColorDialog::getColor(QColor(255,255,205));
    if(customColor.isValid())
    {
        if(customColor.red()>=100||customColor.green()>=100||customColor.blue()>=100)
        {
            ((Widget*)parentWidget())->setEditColor(customColor);
        }
        else
        {
            QMessageBox::warning(NULL,"逸便笺", "颜色不允许过深！\nRGB至少有1个超过100");
        }
    }
}

void noteEdit::setBold()
{
    if(this->currentFont().bold())
    {
        setFormat(0,false);
    }
    else
    {
        setFormat(0,true);
    }
}

void noteEdit::setItalic()
{
    if(this->currentFont().italic())
    {
        setFormat(1,false);
    }
    else
    {
        setFormat(1,true);
    }
}

void noteEdit::setUnderLine()
{
    if(this->currentFont().underline())
    {
        setFormat(2,false);
    }
    else
    {
        setFormat(2,true);
    }
}

void noteEdit::setOverLine()
{
    if(this->currentFont().overline())
    {
        setFormat(3,false);
    }
    else
    {
        setFormat(3,true);
    }
}

void noteEdit::setStrikeOut()
{
    if(this->currentFont().strikeOut())
    {
        setFormat(4,false);
    }
    else
    {
        setFormat(4,true);
    }
}

void noteEdit::setLeft()
{
    this->setAlignment(Qt::AlignLeft);
}

void noteEdit::setRight()
{
    this->setAlignment(Qt::AlignRight);
}

void noteEdit::setCenter()
{
    this->setAlignment(Qt::AlignCenter);
}
