#include <QtGui>

#include "stackedwidget.h"

StackedWidget::StackedWidget()
{
    init();
}

void StackedWidget::back()
{
    int index = cbTitle->currentIndex();
    if(index>0)
    {
        index --;
        cbTitle->setCurrentIndex(index);
        contentwidget->setCurrentIndex(index);
        updateActions();
    }
}

void StackedWidget::forward()
{
    int index = cbTitle->currentIndex();
    if(index<cbTitle->count()-1)
    {
        index ++;
        cbTitle->setCurrentIndex(index);
        contentwidget->setCurrentIndex(index);
        updateActions();
    }
}

bool StackedWidget::closepage()
{
    QWidget *widget = contentwidget->currentWidget();
    if(widget!=0){
        if(widget->close()){
            int index = contentwidget->indexOf(widget);
            cbTitle->removeItem(index);
            contentwidget->removeWidget(widget);
            delete widget;
            updateActions();
            return true;
        }
        else
            return false;
    }
    return true;
}

void StackedWidget::current_changed(int index)
{
}

void StackedWidget::addWidget(QWidget *widget){
    int index = contentwidget->addWidget(widget);
    cbTitle->addItem(widget->windowTitle());
    contentwidget->setCurrentIndex(index);
    cbTitle->setCurrentIndex(index);
    updateActions();
}

bool StackedWidget::removeWidget(QWidget *widget)
{
    if(widget->close()){
        int index = contentwidget->indexOf(widget);
        cbTitle->removeItem(index);
        contentwidget->removeWidget(widget);
        delete widget;
        updateActions();
        return true;
    }else
        return false;
}

void StackedWidget::showWidget(QWidget *widget)
{
    int index = contentwidget->indexOf(widget);
    if(index>=0){
        contentwidget->setCurrentIndex(index);
        cbTitle->setCurrentIndex(index);
        updateActions();
    }
}
void  StackedWidget::removeAll()
{

}

void StackedWidget::setWidgetTitle(QWidget *widget, const QString &title)
{
    int index = contentwidget->indexOf(widget);
    if(index>=0){
        cbTitle->setItemText(index,title);
    }
}

void StackedWidget::init()
{
    init_titlebar();
    this->setFrameStyle(QFrame::Panel|QFrame::Sunken);

    contentwidget = new QStackedWidget(this);
    contentwidget->setFrameStyle(QFrame::NoFrame);
    contentwidget->setStyleSheet("QStackedWidget{background-color:white}");

    connect(cbTitle,SIGNAL(currentIndexChanged(int)),
            contentwidget,SLOT(setCurrentIndex(int)));
    connect(cbTitle,SIGNAL(currentIndexChanged(int)),
            this,SLOT(current_changed(int)));

    QVBoxLayout *vbox = new QVBoxLayout;

    vbox->addWidget(titlebar);
    vbox->addWidget(contentwidget);

    vbox->setMargin(0);
    vbox->setSpacing(1);

    setLayout(vbox);
}

void StackedWidget::init_titlebar()
{
    titlebar = new QFrame(this);
    titlebar->setMinimumHeight(24);

    backAct = new QAction(QIcon(":/images/back.png"),
                          QString("倒退(&S)"),this);
    backAct->setShortcuts(QKeySequence::Back);
    backAct->setToolTip(QString("倒退"));
    connect(backAct,SIGNAL(triggered()), this,SLOT(back()));

    forwardAct = new QAction(QIcon(":/images/forward.png"),
                                      QString("前进"),this);
    forwardAct->setToolTip(QString("前进"));
    connect(forwardAct,SIGNAL(triggered()),this,SLOT(forward()));

    QToolBar *navTools = new QToolBar(titlebar);
    navTools->setMaximumHeight(22);
    navTools->addAction(backAct);
    navTools->addAction(forwardAct);

    closeAct = new QAction(QIcon(":/images/close.png"),
                                    QString("关闭"),this);
    connect(closeAct,SIGNAL(triggered()),this,SLOT(closepage()));

    cbTitle = new QComboBox(titlebar);
    cbTitle->setMinimumHeight(24);

    QToolBar *closeTool = new QToolBar(titlebar);
    closeTool->setMaximumHeight(22);
    closeTool->addAction(closeAct);

    hbox_titlebar = new QHBoxLayout;

    hbox_titlebar->addWidget(navTools);
    hbox_titlebar->addWidget(cbTitle,1);
    hbox_titlebar->addWidget(closeTool);
    hbox_titlebar->setSpacing(5);
    hbox_titlebar->setMargin(0);

    titlebar->setLayout(hbox_titlebar);

    titlebar->setFrameStyle(QFrame::Panel|QFrame::Raised);

    QFile qss(":/titlebar.qss");
    qss.open(QFile::ReadOnly);
    QString s=qss.readAll();
    titlebar->setStyleSheet(s);
    qss.close();

    backAct->setEnabled(false);
    forwardAct->setEnabled(false);
    closeAct->setEnabled(false);
}

void StackedWidget::updateActions()
{
    int index = cbTitle->currentIndex();
    int count = cbTitle->count();

    backAct->setEnabled(index>0 && index <count);
    forwardAct->setEnabled(index<count-1 && index>=0);

    closeAct->setEnabled(count>0);
}
