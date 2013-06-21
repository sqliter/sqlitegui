
#include <QtGui>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>

#include "sqlparse.h"
#include "stackedwidget.h"
#include "showview.h"
#include "viewdesignwidget.h"
#include "showtablewidget.h"
#include "querywidget.h"
#include "tabledesignwidget.h"
#include "indexwidget.h"
#include "triggerwidget.h"
#include "mainwindow.h"
#include <QList>

enum TreeItemType
{
    DBTypeItem=1000,
    DBItem=1001,
    TableTypeItem=1002,
    TableItem=1003,
    ViewTypeItem=1004,
    ViewItem=1005,
    ColumnTypeItem=1006,
    ColumnItem=1007,
    IndexTypeItem=1008,
    IndexItem=1009,
    TriggerTypeItem=1010,
    TriggerItem=1011,
    SysTableTypeItem=1012,
    SysTableItem=1013
};

MainWindow::MainWindow(){    
    init();
}
void MainWindow::treeContextMenu(QPoint p)
{
    QTreeWidgetItem *current=treeDB->itemAt(p);

    if(current==0){
        menuTreeContext->clear();
        menuTreeContext->addAction(createDBAct);
        menuTreeContext->addAction(openDBAct);
        menuTreeContext->addAction(queryAct);
        menuTreeContext->addAction(closeAllDBAct);

        menuTreeContext->exec(QCursor::pos());
        return;
    }

    treeDB->setCurrentItem(current);

    switch(current->type())
    {
    case DBItem:
        {
            menuTreeContext->clear();
            menuTreeContext->addAction(closeDBAct);
            menuTreeContext->addSeparator();
            menuTreeContext->addAction(createTableAct);
            menuTreeContext->addAction(createViewAct);
            menuTreeContext->addAction(createIndexAct);
            //menuTreeContext->addAction(createTriggerAct);
            menuTreeContext->addSeparator();
            menuTreeContext->addAction(refreshAct);
            menuTreeContext->addAction(queryAct);
        }
        break;
    case TableTypeItem:
        {
            menuTreeContext->clear();
            menuTreeContext->addAction(createTableAct);
        }
        break;
    case TableItem:
        {
            menuTreeContext->clear();
            menuTreeContext->addAction(showTableAct);
            menuTreeContext->addAction(alterTableAct);
            menuTreeContext->addAction(createIndexAct);
            //menuTreeContext->addAction(createTriggerAct);
            menuTreeContext->addSeparator();
            menuTreeContext->addAction(removeAct);
            menuTreeContext->addSeparator();
            menuTreeContext->addAction(renameAct);

            menuTreeContext->setDefaultAction(showTableAct);
        }
        break;
    case SysTableTypeItem:
        return;
    case SysTableItem:
        {
            menuTreeContext->clear();
            menuTreeContext->addAction(showTableAct);
        }
        break;
    case ViewTypeItem:
        {
            menuTreeContext->clear();
            menuTreeContext->addAction(createViewAct);
        }
        break;
    case ViewItem:
        {
            menuTreeContext->clear();
            menuTreeContext->addAction(showViewAct);
            menuTreeContext->addAction(alterViewAct);
            //menuTreeContext->addAction(createTriggerAct);
            menuTreeContext->addSeparator();
            menuTreeContext->addAction(removeAct);

            menuTreeContext->setDefaultAction(showViewAct);
        }
        break;
    case IndexTypeItem:
        {
            menuTreeContext->clear();
            menuTreeContext->addAction(createIndexAct);
        }
        break;
    case IndexItem:
        {
            menuTreeContext->clear();
            menuTreeContext->addAction(alterIndexAct);
            menuTreeContext->addSeparator();
            menuTreeContext->addAction(removeAct);
            //menuTreeContext->setDefaultAction(alterIndexAct);
        }
        break;
    case TriggerTypeItem:
        {
            menuTreeContext->clear();
            //menuTreeContext->addAction(createTriggerAct);
        }
        break;
    case TriggerItem:
        {
            menuTreeContext->clear();
            menuTreeContext->addAction(alterTriggerAct);
            //menuTreeContext->addSeparator();
            //menuTreeContext->addAction(removeAct);
            //menuTreeContext->setDefaultAction(alterTriggerAct);
        }
        break;
    default:
        {
            menuTreeContext->clear();
            menuTreeContext->addAction(createDBAct);
            menuTreeContext->addAction(openDBAct);
            menuTreeContext->addAction(queryAct);
            menuTreeContext->addAction(closeAllDBAct);
        }
        break;
    }
    menuTreeContext->exec(QCursor::pos());
}
void MainWindow::treeItemChanged(QTreeWidgetItem *item, int column)
{
    treeDB->disconnect(this,SLOT(treeItemChanged(QTreeWidgetItem*,int)));
    if(item->type()==DBItem){
//        QTreeWidgetItem *item = treeDB->currentItem();
//        QSqlDatabase::database().close();
//        QFileInfo fi(item->toolTip(column));
//        QString path = fi.path();
//        QString newfile=QString("%1/%2.db").arg(path).arg(item->text(0));
//        QFile::rename(item->toolTip(0),newfile);

//        treeDB->disconnect(this,SLOT(treeItemChanged(QTreeWidgetItem*,int)));
//        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
//        db.setDatabaseName(newfile);
//        if(!db.open())return;
    }else if(item->type()==TableItem){
        QString name = item->text(column).trimmed();

        //bool ok;
        //QString new_name= QInputDialog::getText(this,QString("新表名"),
        //                                        QString("新表名"),
        //                                        QLineEdit::Normal,
        //                                        name,&ok);
        if(!name.isEmpty()){
            QTreeWidgetItem *dbitem = treeDB->currentItem()->parent()->parent();
            QString conname = dbitem->data(0,Qt::UserRole).toString();
            QSqlDatabase db = QSqlDatabase::database(conname);
            QString strsql=QString("alter table %1 rename to %2")
                    .arg(old_name).arg(name);
            QSqlQuery q(db);
            if(q.exec(strsql))
            {
                item->setText(column,name);
            }else
                item->setText(column,old_name);
        }
    }
}
void MainWindow::treeItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    connect(treeDB,SIGNAL(itemChanged(QTreeWidgetItem*,int)),
            this,SLOT(treeItemChanged(QTreeWidgetItem*,int)));
    old_name = item->text(column);
    if(item->type()==TableItem || item->type() == SysTableItem){
        showTable();
    }else if(item->type()== ViewItem){
        showView();
    }
}

void MainWindow::remove()
{
    QTreeWidgetItem *item = treeDB->currentItem();
    if(item->type()==TableItem){
        removeTable();
    }else if(item->type() == ViewItem){
        remove_view();
    }else if(item->type() == IndexItem){
        remove_index();
    }
}
void MainWindow::rename()
{
    connect(treeDB,SIGNAL(itemChanged(QTreeWidgetItem*,int)),
            this,SLOT(treeItemChanged(QTreeWidgetItem*,int)));
    old_name = treeDB->currentItem()->text(0);
    treeDB->editItem(treeDB->currentItem());
}

void MainWindow::createDB()
{
    QString fileName = QFileDialog::getSaveFileName(this, QString("创建新数据库"),
                                    QString(),
                                    QString("SQLite3数据库名 (*.db)"));
    if(fileName.isEmpty())return;

    QFileInfo fi(fileName);
    QString conname=QString("Sqlite3-%1").arg(fi.baseName());

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE",conname);
    db.setDatabaseName(fileName);
    if(!db.open()){
        QSqlError se = db.lastError();
        QMessageBox::warning(this,QString("创建数据库错误"),se.text());
        return;
    }

    QTreeWidgetItem *dbitem = new QTreeWidgetItem(treeDB,DBItem);
    dbitem->setText(0,fi.baseName());
    dbitem->setIcon(0,QIcon(":/images/db.png"));
    dbitem->setToolTip(0,fileName);
    dbitem->setData(0,Qt::UserRole,db.connectionName());
    //dbitem->setFlags(dbitem->flags()|Qt::ItemIsEditable);
    dbitem->setExpanded(true);
    toplevels.append(dbitem);

    refresh(dbitem);

    emit connectionChanged();
}

void MainWindow::openDB(){
    QString fileName = QFileDialog::getOpenFileName(this, QString("打开数据库"),
                                    QString(),
                                    QString("SQLite3数据库 (*.db)"));
    if(fileName.isEmpty())return;

    QFileInfo fi(fileName);
    QString conname = QString("Sqlite3-%1").arg(fi.baseName());
    if(QSqlDatabase::contains(conname)){
        conname = QString("Sqlite3DB-%1").arg(fileName);
    }
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE",conname);
    db.setDatabaseName(fileName);
    if(!db.open()){
        QSqlError se = db.lastError();
        QMessageBox::warning(this,QString("打开数据库错误"),se.text());
        return;
    }

    QTreeWidgetItem *dbitem = new QTreeWidgetItem(treeDB,DBItem);
    dbitem->setText(0,fi.baseName());
    dbitem->setIcon(0,QIcon(":/images/db.png"));
    dbitem->setToolTip(0,fileName);
    dbitem->setData(0,Qt::UserRole,QVariant(db.connectionName()));
    //dbitem->setFlags(dbitem->flags()|Qt::ItemIsEditable);
    dbitem->setExpanded(true);
    toplevels.append(dbitem);

    refresh(dbitem);

    emit connectionChanged();

}
void MainWindow::closeDB(){
    QTreeWidgetItem *item = treeDB->currentItem();
    QString constr = item->data(0,Qt::UserRole).toString();
    //关闭所有打开的表
    QList<QWidget*> *widgets = ShowTableWidget::dbTables(constr);

    QWidget *widget=0;
    for(int i=0;i<widgets->count();i++){
        widget = widgets->at(i);
        if(!stackedMain->removeWidget(widget))
        {
            return;
        }
    }
    widgets->clear();
    delete widgets;
    //关闭所有正在设计的表
    QList<QWidget*> *tdwidgets = TableDesignWidget::dbTables(constr);
    foreach(QWidget* w,*tdwidgets){
        if(!stackedMain->removeWidget(w))
            return;
    }
    tdwidgets->clear();
    delete tdwidgets;

    {
        QSqlDatabase db = QSqlDatabase::database(constr);
        db.close();
    }
    QSqlDatabase::removeDatabase(constr);

    int index = treeDB->indexOfTopLevelItem(item);
    treeDB->takeTopLevelItem(index);
    toplevels.removeAll(item);
    delete item;
    emit connectionChanged();
}
void MainWindow::closeAllDB()
{
    //int count = treeDB->topLevelItemCount();
    QList<ShowTableWidget*> tables = ShowTableWidget::allTables();

    foreach(ShowTableWidget *widget,tables){
        stackedMain->removeWidget(widget);
    }

    foreach(QTreeWidgetItem *item,toplevels){
        //QTreeWidgetItem *item = treeDB->topLevelItem(i);
        qDebug()<<item->toolTip(0);
        QString constr = item->data(0,Qt::UserRole).toString();
        {
            QSqlDatabase db = QSqlDatabase::database(constr);
            db.close();
        }
        QSqlDatabase::removeDatabase(constr);
        int index = treeDB->indexOfTopLevelItem(item);
        treeDB->takeTopLevelItem(index);
        delete item;
    }
    toplevels.clear();
    emit connectionChanged();
}

void MainWindow::refresh()
{
    QTreeWidgetItem *item = treeDB->currentItem();
    if(item->type()==DBItem){
        refresh(item);
    }
}

void MainWindow::query(){
    bool isExist;
    QueryWidget *widget = QueryWidget::createObject(QString(),isExist);

    stackedMain->addWidget(widget);
}

void MainWindow::createTable()
{
    QTreeWidgetItem *dbitem=treeDB->currentItem();
    if(dbitem->type()==DBItem){
    }else if(dbitem->type()==TableTypeItem){
        dbitem = dbitem->parent();
    }
    QString conname = dbitem->data(0,Qt::UserRole).toString();
    QString name=QString();

    bool isExist = false;
    TableDesignWidget *widget = TableDesignWidget::createObject(name,conname,isExist);
    if(isExist){
        stackedMain->showWidget(widget);
    }else{
        stackedMain->addWidget(widget);
    }
}

void MainWindow::createView()
{
    QTreeWidgetItem *dbitem=treeDB->currentItem();
    if(dbitem->type()==DBItem){
    }else if(dbitem->type()==ViewTypeItem){
        dbitem = dbitem->parent();
    }
    QString conname = dbitem->data(0,Qt::UserRole).toString();
    QString name=QString();

    bool isExist = false;
    ViewDesignWidget *widget = ViewDesignWidget::createObject(name,conname,isExist);
    if(isExist){
        stackedMain->showWidget(widget);
    }else{
        stackedMain->addWidget(widget);
    }
}

void MainWindow::createIndex()
{
    QString tbname;
    QTreeWidgetItem *dbitem=treeDB->currentItem();
    if(dbitem->type()==DBItem){
        tbname = QString();
    }else if(dbitem->type()==TableItem){
        tbname = dbitem->text(0);
        dbitem = dbitem->parent()->parent();
    }else if(dbitem->type()==IndexTypeItem){
        tbname = dbitem->parent()->text(0);
        dbitem = dbitem->parent()->parent()->parent();
    }else
    {
        return;
    }
    QString conname = dbitem->data(0,Qt::UserRole).toString();
    QString name=QString();

     IndexDialog dlgIndex;
     dlgIndex.init(conname,tbname,name);
     dlgIndex.exec();
}

void MainWindow::createTrigger()
{
    QString tbname;
    QString conname;
    QString name=QString();

    QTreeWidgetItem *dbitem=treeDB->currentItem();
    if(dbitem->type()==DBItem){
        tbname = QString();
    }else if(dbitem->type()==TableItem){
        tbname = dbitem->text(0);
        dbitem = dbitem->parent()->parent();
    }else if(dbitem->type()==ViewItem){
        tbname = dbitem->text(0);
        dbitem = dbitem->parent()->parent();
    }
    else if(dbitem->type()==TriggerTypeItem){
        tbname = dbitem->parent()->text(0);
        dbitem = dbitem->parent()->parent()->parent();
    }else
    {
        return;
    }
    conname = dbitem->data(0,Qt::UserRole).toString();

    TriggerDialog dlgTrigger;
    dlgTrigger.init(conname,tbname,name);
    dlgTrigger.exec();
}

void MainWindow::showTable(){
    QTreeWidgetItem *dbitem = treeDB->currentItem()->parent()->parent();
    QString conname = dbitem->data(0,Qt::UserRole).toString();
    QString name = treeDB->currentItem()->text(0).trimmed();

    bool isExist = false;
    ShowTableWidget *widget = ShowTableWidget::createObject(name,conname,isExist);
    if(isExist){
        stackedMain->showWidget(widget);
    }else{
        stackedMain->addWidget(widget);
    }
}

void MainWindow::showView(){
    QTreeWidgetItem *dbitem = treeDB->currentItem()->parent()->parent();
    QString conname = dbitem->data(0,Qt::UserRole).toString();
    QString name = treeDB->currentItem()->text(0).trimmed();

    bool isExist = false;
    ShowViewWidget *widget =
            ShowViewWidget::createObject(name,conname,isExist);
    if(isExist){
        stackedMain->showWidget(widget);
    }else{
        stackedMain->addWidget(widget);
    }
}

void MainWindow::editTable(){
    QTreeWidgetItem *dbitem = treeDB->currentItem()->parent()->parent();
    QString conname = dbitem->data(0,Qt::UserRole).toString();
    QString name = treeDB->currentItem()->text(0).trimmed();
    //stackedMain->openWidget(QString("edittable"),name);
    bool isExist = false;
    TableDesignWidget *widget = TableDesignWidget::createObject(name,conname,isExist);
    if(isExist){
        stackedMain->showWidget(widget);
    }else{
        stackedMain->addWidget(widget);
    }
}

void MainWindow::renameTable()
{
    QTreeWidgetItem *item = treeDB->currentItem();
    QString name = item->text(0).trimmed();

    bool ok;
    QString new_name= QInputDialog::getText(this,QString("新表名"),
                                            QString("新表名"),
                                            QLineEdit::Normal,
                                            name,&ok);
    if(ok && !new_name.isEmpty()){
        QTreeWidgetItem *dbitem = treeDB->currentItem()->parent()->parent();
        QString conname = dbitem->data(0,Qt::UserRole).toString();
        QSqlDatabase db = QSqlDatabase::database(conname);
        QString strsql=QString("alter table %1 rename to %2")
                .arg(name).arg(new_name);
        QSqlQuery q(db);
        if(q.exec(strsql))
        {
            item->setText(0,new_name);
        }
    }
}

void MainWindow::removeTable()
{
    QTreeWidgetItem *item = treeDB->currentItem();
    QString name = item->text(0).trimmed();

//    bool ok;
//    QString new_name= QInputDialog::getText(this,QString("新表名"),
//                                            QString("新表名"),
//                                            QLineEdit::Normal,
//                                            name,&ok);
    if(item->type() == TableItem){
        QTreeWidgetItem *dbitem = item->parent()->parent();
        QString conname = dbitem->data(0,Qt::UserRole).toString();
        QSqlDatabase db = QSqlDatabase::database(conname);
        QString strsql=QString("drop table if exists %1").arg(name);
        QSqlQuery q(db);
        if(q.exec(strsql))
        {
            QTreeWidgetItem *parent = item->parent();
            parent->removeChild(item);
            delete item;
        }else{

        }
    }
}

void MainWindow::editView(){
    QTreeWidgetItem *dbitem = treeDB->currentItem()->parent()->parent();
    QString conname = dbitem->data(0,Qt::UserRole).toString();
    QString name = treeDB->currentItem()->text(0).trimmed();

    bool isExist = false;
    ViewDesignWidget *widget =
            ViewDesignWidget::createObject(name,conname,isExist);
    if(isExist){
        stackedMain->showWidget(widget);
    }else{
        stackedMain->addWidget(widget);
    }
}

void MainWindow::editIndex(){
    QTreeWidgetItem *item = treeDB->currentItem();
    QString conname = item->data(0,33).toString();
    QString name = item->text(0).trimmed();
    QString tbname = item->data(0,32).toString();
    QStringList sl = item->data(0,34).toStringList();

    IndexDialog dlgIndex;
    dlgIndex.init(conname,tbname,name);
    dlgIndex.loadIndex(sl);
    dlgIndex.exec();
}

void MainWindow::editTrigger(){
    QTreeWidgetItem *item = treeDB->currentItem();
    QString conname = item->data(0,33).toString();
    QString name = item->text(0).trimmed();
    QString tbname = item->data(0,32).toString();
    QStringList sl = item->data(0,34).toStringList();

    IndexDialog dlgIndex;
    dlgIndex.init(conname,tbname,name);
    dlgIndex.loadIndex(sl);
    dlgIndex.exec();
}
void MainWindow::about(){

    QMessageBox::about(this,QString("关于SQLiteGUI"),
            QString("The <b>SQLiteGUI %1</b>"
              "<p>作者：<a href='mailto:raymondliu@163.com'>liuqb</a></p>"
              "<P>如有问题请联系作者Email:raymondliu@163.com</p>")
                       .arg(qApp->applicationVersion()));
}

void MainWindow::init_menu(){

    menuDB = menuBar()->addMenu(QString("数据库"));

    menuDB->addAction(createDBAct);
    menuDB->addAction(openDBAct);
    menuDB->addAction(closeDBAct);
    menuDB->addAction(closeAllDBAct);
    menuDB->addSeparator();
    menuDB->addAction(queryAct);
    menuDB->addSeparator();
    menuDB->addAction(exitAct);

    menuHelp = menuBar()->addMenu(QString("帮助"));
    menuHelp->addAction(aboutAct);
}

void MainWindow::init_toolbar()
{
    QToolBar *toolBar = addToolBar(QString("DB"));
    toolBar->addAction(createDBAct);
    toolBar->addAction(openDBAct);
    toolBar->addAction(closeDBAct);
    toolBar->addSeparator();
    toolBar->addAction(queryAct);
}

void MainWindow::init_actions()
{
    saveAct = new QAction(QIcon(":/images/save.png"), QString("保存(&S)"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(QString("保存到磁盘"));

    removeAct = new QAction(QIcon(":/images/remove.png"), QString("删除"), this);
    removeAct->setShortcuts(QKeySequence::Delete);
    removeAct->setStatusTip(QString("删除"));
    connect(removeAct,SIGNAL(triggered()),this,SLOT(remove()));

    renameAct = new QAction(QString("重命名"),this);
    connect(renameAct,SIGNAL(triggered()),this,SLOT(rename()));

    refreshAct = new QAction(QIcon(":/images/refresh.png"), QString("刷新"), this);
    refreshAct->setShortcuts(QKeySequence::Refresh);
    refreshAct->setStatusTip(QString("刷新"));
    connect(refreshAct,SIGNAL(triggered()),this,SLOT(refresh()));

    exitAct = new QAction(QString("退出(&x)"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(QString("退出应用程序"));
    connect(exitAct,SIGNAL(triggered()),this,SLOT(close()));

    aboutAct = new QAction(QString("关于"),this);
    aboutAct->setStatusTip(QString("关于SqliteGUI"));
    connect(aboutAct,SIGNAL(triggered()),this,SLOT(about()));

    createDBAct = new QAction(QIcon(":/images/new.png"), QString("创建数据库"), this);
    createDBAct->setShortcuts(QKeySequence::New);
    createDBAct->setStatusTip(QString("创建一个新SQLite3数据库"));
    connect(createDBAct,SIGNAL(triggered()),this,SLOT(createDB()));

    openDBAct = new QAction(QIcon(":/images/open.png"), QString("打开数据库"), this);
    openDBAct->setShortcuts(QKeySequence::Open);
    openDBAct->setStatusTip(QString("打开SQLite3数据库"));
    connect(openDBAct,SIGNAL(triggered()),this,SLOT(openDB()));

    closeDBAct = new QAction(QIcon(":/images/closedb.png"),QString("关闭数据库"),this);
    closeDBAct->setShortcut(QKeySequence::Close);
    connect(closeDBAct,SIGNAL(triggered()),this,SLOT(closeDB()));

    closeAllDBAct = new QAction(QString("关闭所有数据库"),this);
    connect(closeAllDBAct,SIGNAL(triggered()),this,SLOT(closeAllDB()));

    queryAct = new QAction(QIcon(":/images/query.png"),QString("查询"),this);
    queryAct->setShortcut(QKeySequence(QString("F8")));
    connect(queryAct,SIGNAL(triggered()),this,SLOT(query()));

    createTableAct = new QAction(QString("创建表"),this);
    connect(createTableAct,SIGNAL(triggered()),
            this,SLOT(createTable()));

    createViewAct = new QAction(QString("创建视图"),this);
    connect(createViewAct,SIGNAL(triggered()),
            this,SLOT(createView()));

    createIndexAct = new QAction(QString("创建索引"),this);
    connect(createIndexAct,SIGNAL(triggered()),
            this,SLOT(createIndex()));

    createTriggerAct = new QAction(QString("创建触发器"),this);
    connect(createTriggerAct,SIGNAL(triggered()),
            this,SLOT(createTrigger()));

    showTableAct = new QAction(QString("打开表"),this);
    connect(showTableAct,SIGNAL(triggered()),this,SLOT(showTable()));

    alterTableAct = new QAction(QString("修改表"),this);
    connect(alterTableAct,SIGNAL(triggered()),this,SLOT(editTable()));

    showViewAct = new QAction(QString("显示视图数据"),this);
    connect(showViewAct,SIGNAL(triggered()),
            this,SLOT(showView()));

    alterViewAct = new QAction(QString("查看视图定义"),this);
    connect(alterViewAct,SIGNAL(triggered()),
            this,SLOT(editView()));

    alterIndexAct = new QAction(QString("查看索引定义"),this);
    connect(alterIndexAct,SIGNAL(triggered()),
            this,SLOT(editIndex()));

    alterTriggerAct = new QAction(QString("查看触发器定义"),this);
    connect(alterTriggerAct,SIGNAL(triggered()),
            this,SLOT(editTrigger()));

}

void MainWindow::init_tree(){
    treeDB = new QTreeWidget;
    treeDB->setHeaderLabel(QString("所有数据库"));

    treeDB->setContextMenuPolicy(Qt::CustomContextMenu);
    treeDB->setExpandsOnDoubleClick(true);
    treeDB->setEditTriggers(QAbstractItemView::EditKeyPressed);
    connect(treeDB,SIGNAL(customContextMenuRequested(QPoint)),
            this,SLOT(treeContextMenu(QPoint)));
    connect(treeDB,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this,SLOT(treeItemDoubleClicked(QTreeWidgetItem*,int)));


    menuTreeContext = new QMenu(treeDB);
}

void MainWindow::init(){

    ShowViewWidget::initObjContainer();
    ViewDesignWidget::initObjContainer();
    ShowTableWidget::initObjContainer();
    QueryWidget::initObjContainer();
    TableDesignWidget::initObjContainer();

    init_actions();
    init_menu();
    //init_toolbar();
    init_tree();

    splitterMain = new QSplitter(this);
    stackedMain = new StackedWidget;

    splitterMain->addWidget(treeDB);
    splitterMain->addWidget(stackedMain);

    setCentralWidget(splitterMain);

    resize(800,400);

    QList<int> list;
    list<<200<<600;

    splitterMain->setSizes(list);

    setWindowIcon(QIcon(":/images/app.png"));
}

void MainWindow::addTable(const QString &constr, const QString &tbname)
{
    foreach(QTreeWidgetItem *dbitem,toplevels){
        if(constr == dbitem->data(0,Qt::UserRole).toString())
        {
            refresh(dbitem);
            break;
        }
    }
}
void MainWindow::refresh(const QString &constr)
{
    foreach(QTreeWidgetItem *dbitem,toplevels)
    {
        if(constr == dbitem->data(0,Qt::UserRole).toString())
        {
            refresh(dbitem);
            break;
        }
    }
}

void MainWindow::refresh(QTreeWidgetItem *dbitem)
{
    QFileInfo fi(dbitem->toolTip(0));
    dbitem->setText(0,fi.baseName());
    QList<QTreeWidgetItem*> children = dbitem->takeChildren();

    int count= children.count();
    QTreeWidgetItem *child=0;
    for(int i=0;i<count;i++){
        child = children.value(i);
        delete child;
    }
    children.clear();

    QString constr =dbitem->data(0,Qt::UserRole).toString();
    QSqlDatabase db = QSqlDatabase::database(constr);

    QTreeWidgetItem *tableitem =new QTreeWidgetItem(dbitem,TableTypeItem);
    tableitem->setText(0,QString("表"));
    tableitem->setIcon(0,QIcon(":/images/node.png"));
    tableitem->setExpanded(true);

    QStringList tables = db.tables(QSql::Tables);
    QTreeWidgetItem *item=0,*column=0,*columns=0;
    for(int i=0;i<tables.size();++i)
    {
        item = new QTreeWidgetItem(tableitem,TableItem);
        item->setText(0,tables.at(i));
        item->setIcon(0,QIcon(":/images/table.png"));
        item->setFlags(dbitem->flags()|Qt::ItemIsEditable);

        columns = new QTreeWidgetItem(item,ColumnTypeItem);
        columns->setText(0,QString("字段"));
        columns->setIcon(0,QIcon(":/images/node.png"));

        QSqlRecord record = db.record(tables.at(i));
        for(int j=0;j<record.count();++j)
        {
            column = new QTreeWidgetItem(columns,ColumnItem);
            column->setText(0,record.fieldName(j));
            column->setIcon(0,QIcon(":/images/column.png"));
        }
        load_table_indexs(item,constr);
    }

    QTreeWidgetItem *systableitem =new QTreeWidgetItem(dbitem,SysTableTypeItem);
    systableitem->setText(0,QString("系统表"));
    systableitem->setIcon(0,QIcon(":/images/node.png"));
    systableitem->setExpanded(true);

    QStringList systables = db.tables(QSql::SystemTables);
    item=0;
    column=0;
    columns=0;
    for(int i=0;i<systables.size();++i)
    {
        item = new QTreeWidgetItem(systableitem,SysTableItem);
        item->setText(0,systables.at(i));
        item->setIcon(0,QIcon(":/images/table.png"));

        columns = new QTreeWidgetItem(item,ColumnTypeItem);
        columns->setText(0,QString("字段"));
        columns->setIcon(0,QIcon(":/images/node.png"));

        QSqlRecord record = db.record(systables.at(i));
        for(int j=0;j<record.count();++j)
        {
            column = new QTreeWidgetItem(columns,ColumnItem);
            column->setText(0,record.fieldName(j));
            column->setIcon(0,QIcon(":/images/column.png"));
        }
    }

    QTreeWidgetItem *viewitem = new QTreeWidgetItem(dbitem,ViewTypeItem);
    viewitem->setText(0,QString("视图"));
    viewitem->setIcon(0,QIcon(":/images/node.png"));
    viewitem->setExpanded(true);

    QStringList views = db.tables(QSql::Views);

    item=0;
    column=0;
    columns=0;

    for(int i=0;i<views.size();++i)
    {
        item = new QTreeWidgetItem(viewitem,ViewItem);
        item->setText(0,views.at(i));
        item->setIcon(0,QIcon(":/images/view.png"));

        columns = new QTreeWidgetItem(item,ColumnTypeItem);
        columns->setText(0,QString("字段"));
        columns->setIcon(0,QIcon(":/images/node.png"));

        QSqlRecord record = db.record(views.at(i));
        for(int j=0;j<record.count();++j)
        {
            column = new QTreeWidgetItem(columns,ColumnItem);
            column->setText(0,record.fieldName(j));
            column->setIcon(0,QIcon(":/images/column.png"));
        }
    }

    //    QTreeWidgetItem *indexitem =new QTreeWidgetItem(dbitem,TableTypeItem);
    //    indexitem->setText(0,QString("索引"));
    //
    //    QTreeWidgetItem *triggleitem = new QTreeWidgetItem(dbitem,ViewTypeItem);
    //    triggleitem->setText(0,QString("触发器"));
}

void MainWindow::remove_view()
{
    QTreeWidgetItem *item = treeDB->currentItem();
    if(item->type() == ViewItem){
        QString name = item->text(0).trimmed();

        QTreeWidgetItem *dbitem = item->parent()->parent();
        QString conname = dbitem->data(0,Qt::UserRole).toString();
        QSqlDatabase db = QSqlDatabase::database(conname);
        QString strsql=QString("drop view %1").arg(name);
        QSqlQuery q(db);
        if(q.exec(strsql))
        {
            QTreeWidgetItem *parent = item->parent();
            parent->removeChild(item);
            delete item;
        }else{

        }
    }
}

void MainWindow::remove_index()
{
    QTreeWidgetItem *item = treeDB->currentItem();
    if(item->type() == IndexItem){
        QString name = item->text(0).trimmed();
        //QString tbname = item->data(0,32).toString();
        QString conname = item->data(0,33).toString();
        QSqlDatabase db = QSqlDatabase::database(conname);
        QString strsql=QString("drop index if exists %1").arg(name);
        QSqlQuery q(db);
        if(q.exec(strsql))
        {
            QTreeWidgetItem *parent = item->parent();
            parent->removeChild(item);
            delete item;
        }else{

        }
    }
}
void MainWindow::load_table_indexs(QTreeWidgetItem *tbitem,
                                   const QString &con_name)
{
    QSqlDatabase db = QSqlDatabase::database(con_name);
    QString table_name = tbitem->text(0);

    QString sql = QString("select sql,name from sqlite_master where"
                          " type == 'index' and tbl_name == '%1'")
            .arg(table_name);
    QSqlQuery query(db);

    if(!query.exec(sql))return;

    QTreeWidgetItem *indextypeitem =
            new QTreeWidgetItem(tbitem,IndexTypeItem);
    indextypeitem->setText(0,QString("索引"));
    indextypeitem->setIcon(0,QIcon(":/images/node.png"));

    while(query.next()){
        SqliteIndex index;
        QString src = query.value(0).toString();
        if(src.isEmpty()){
            if(query.value(0).isValid()){
                QTreeWidgetItem *indexitem =
                    new QTreeWidgetItem(indextypeitem,IndexItem);
                indexitem->setText(0,query.value(1).toString());
                indexitem->setData(0,Qt::UserRole,table_name);
                indexitem->setData(0,33,con_name);
                indexitem->setData(0,34,QStringList());
                indexitem->setIcon(0,QIcon(":/images/index.png"));
            }
            continue;
        }

        SqlParse parser;
        if(parser.parse_index(src,db.databaseName(),index))
        {
            if(table_name != index.tbname)return;
            QTreeWidgetItem *indexitem =
                    new QTreeWidgetItem(indextypeitem,IndexItem);
            indexitem->setText(0,index.name);
            indexitem->setToolTip(0,src);
            indexitem->setData(0,Qt::UserRole,index.tbname);
            indexitem->setData(0,33,con_name);
            indexitem->setIcon(0,QIcon(":/images/index.png"));

            QStringList sl;
            int count = index.collist.count();
            for(int i=0;i<count;i++){
                QTreeWidgetItem *column =
                        new QTreeWidgetItem(indexitem,ColumnItem);
                column->setText(0,index.collist.value(i)->name);
                column->setIcon(0,QIcon(":/images/column.png"));
                sl<<index.collist.value(i)->name;
                sl<<index.collist.value(i)->collate;
                sl<<index.collist.value(i)->sort;
            }
            indexitem->setData(0,34,sl);
        }
    }
}

void MainWindow::load_trigger(QTreeWidgetItem *dbitem)
{
    QString con_name = dbitem->data(0,32).toString();
    QSqlDatabase db = QSqlDatabase::database(con_name);

    QString sql = QString("select * from sqlite_master where"
                          " type == 'trigger'" );
    QSqlQuery query(db);

    if(!query.exec(sql))return;

    QTreeWidgetItem *triggertypeitem =
            new QTreeWidgetItem(dbitem,TriggerTypeItem);
    triggertypeitem->setText(0,QString("触发器"));
    triggertypeitem->setIcon(0,QIcon(":/images/node.png"));

    while(query.next()){

    }
}

StackedWidget* MainWindow::mdiArea()
{
    return this->stackedMain;
}
