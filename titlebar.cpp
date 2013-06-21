#include <QtGui>
#include "titlebar.h"

TitleBar::TitleBar(){

    init();
    //this->setFixedHeight(20);
    //this->setMinimumWidth(200);
}

void TitleBar::prev(){
    int index = cbTitle->currentIndex();
    if(index>0)
    {
        index --;
        cbTitle->setCurrentIndex(index);
        emit indexChanged(index);
    }
}

void TitleBar::next(){
    int index = cbTitle->currentIndex();
    if(index<cbTitle->count()-1)
    {
        index ++;
        cbTitle->setCurrentIndex(index);
        emit indexChanged(index);
    }
}

void TitleBar::remove()
{
    int index = cbTitle->currentIndex();
    emit title_removed(index);
    cbTitle->removeItem(index);
    //int nindex = cbTitle->currentIndex();
    //if(index!=nindex)
}
void TitleBar::addTitle(const QString &title)
{
    cbTitle->addItem(title);
}

void TitleBar::setCurrent(int index)
{
    cbTitle->setCurrentIndex(index);
}

void TitleBar::init()
{
    prevAct = new QAction(QIcon(":/images/prev.png"), QString("倒退"), this);
    prevAct->setShortcuts(QKeySequence::Back);
    prevAct->setStatusTip(QString("倒退"));
    connect(prevAct,SIGNAL(triggered()),this,SLOT(prev()));

    nextAct = new QAction(QIcon(":/images/next.png"), QString("前进"), this);
    nextAct->setShortcuts(QKeySequence::Forward);
    nextAct->setStatusTip(QString("前进"));
    connect(nextAct,SIGNAL(triggered()),this,SLOT(next()));

    removeAct = new QAction(QIcon(":/images/close.png"), QString("关闭"), this);
    removeAct->setShortcuts(QKeySequence::Close);
    removeAct->setStatusTip(QString("关闭"));

    //connect(removeAct,SIGNAL(triggered()),this,SLOT(remove()));

    //QToolButton *btnPrev = new QToolButton(this);
    //QToolButton *btnNext = new QToolButton(this);
    //QToolButton *btnClose = new QToolButton(this);
    cbTitle = new QComboBox;
    QWidgetAction *titleAct = new QWidgetAction(this);
    titleAct->setDefaultWidget(cbTitle);

    QToolBar *navToolBar = new QToolBar(this);
    navToolBar->addAction(prevAct);
    navToolBar->addAction(nextAct);
    QToolBar *tbar = new QToolBar(this);
    tbar->addAction(titleAct);

    QToolBar *closeToolBar = new QToolBar(this);
    closeToolBar->addAction(removeAct);

    //btnPrev->setDefaultAction(prevAct);
    //btnNext->setDefaultAction(nextAct);
    //btnClose->setDefaultAction(removeAct);
    //addAction(prevAct);
    //addAction(nextAct);


    //cbTitle->setMinimumWidth(300);
//
//    QLabel *lblClose = new QLabel(this);
//    lblClose->setToolTip(QString("关闭"));
//    lblClose->setPixmap(QPixmap(":/images/close.png"));
//    lblClose->setFrameStyle(QFrame::StyledPanel|QFrame::Plain);

    QHBoxLayout *hbox = new QHBoxLayout;

    hbox->addWidget(navToolBar);
    //hbox->addWidget(btnNext);
    hbox->addWidget(tbar,1);
    //hbox->addStretch();
    hbox->addWidget(closeToolBar);
    hbox->setSpacing(0);
    hbox->setMargin(0);

    setLayout(hbox);
//
////    btnPrev->setFlat(true);
////    btnNext->setFlat(true);
////    btnClose->setFlat(true);
//    //setStyleSheet(QString("QPushButton{border:0px;}"));
//    QFile qss(":/stylesheet.qss");
//    qss.open(QFile::ReadOnly);
//    QString s=qss.readAll();
//    setStyleSheet(s);
//    qss.close();
//    qDebug()<<s;
}
