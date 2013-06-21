#include <QtGui>
#include <QMenu>
#include <QtSql>

#include "showtablewidget.h"
#include <QSqlTableModel>
#include <QSqlRecord>
struct ShowTableObjects{
    QMap<QPair<QString,QString>,ShowTableWidget*> widgets;
};

ShowTableObjects* ShowTableWidget::objs=0;
ShowTableWidget* ShowTableWidget::createObject(const QString &table_name,
                                               const QString &constr,
                                               bool &isExist)
{
    QPair<QString,QString> pair;

    pair.first = table_name;
    pair.second = constr;

    if(objs->widgets.contains(pair))
    {
        isExist = true;
        return objs->widgets[pair];
    }else{
        isExist = false;
        ShowTableWidget *widget = new ShowTableWidget;
        widget->init(table_name,constr);
        objs->widgets.insert(pair,widget);
        return widget;
    }
}
void ShowTableWidget::deleteObject(const QString &table_name, const QString &constr)
{
    QPair<QString,QString> pair;

    pair.first = table_name;
    pair.second = constr;

    objs->widgets.remove(pair);
}
void ShowTableWidget::initObjContainer()
{
    objs = new ShowTableObjects;
}

void ShowTableWidget::delObjContainer()
{
    objs->widgets.clear();

    delete objs;
}

QList<QWidget*>* ShowTableWidget::dbTables(const QString &constr)
{
    QList<QPair<QString,QString> > keys = objs->widgets.keys();
    QPair<QString,QString> pair;
    QStringList tables;

    foreach(pair,keys){
        //keys.at(i);
        if(pair.second==constr){
            tables.append(pair.first);
        }
    }

    QList<QWidget*> *ret = new QList<QWidget*>();
    foreach(QString table,tables){
        pair.first = table;
        pair.second = constr;

        ShowTableWidget *widget = objs->widgets.find(pair).value();
        ret->append(widget);
    }
    return ret;

    //QMap<QPair<QString,QString>,ShowTableWidget*>;
}

QList<ShowTableWidget*> ShowTableWidget::allTables()
{
    return objs->widgets.values();
}

ShowTableWidget::ShowTableWidget()
{
}

void ShowTableWidget::init(const QString &table_name,const QString &constr)
{
    isDirty = false;
    table = table_name;
    con = constr;
    dbname = QSqlDatabase::database(con).databaseName();

    init_model();
    init_table();
    init_menu();
    init_toolbar();

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(toolbar);
    vbox->addWidget(view,1);
    vbox->setMargin(0);
    vbox->setSpacing(0);

    this->setLayout(vbox);

    setWindowTitle(QString("数据表%1.%2").arg(dbname).arg(table));
}
void ShowTableWidget::init_table(){
    view = new QTableView(this);
    view->setModel(model);
    view->resizeRowsToContents();
    view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(view,SIGNAL(customContextMenuRequested(QPoint)),
            this,SLOT(showContextMenu(QPoint)));

}

void ShowTableWidget::init_model(){

    QSqlDatabase db = QSqlDatabase::database(con);
    model = new QSqlTableModel(this,db);
    model->setTable(table);

    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

    QSqlRecord record = db.record(table);
    for(int i=0;i<record.count();++i)
    {
        model->setHeaderData(i,Qt::Horizontal,record.fieldName(i));
    }
}

void ShowTableWidget::init_menu(){
    contextMenu = new QMenu(view);
    appendAct = contextMenu->addAction(QIcon(":/images/new.png"),
                                       QString("追加行"),this,SLOT(appendRow()));
    insertAct = contextMenu->addAction(QIcon(":/images/insert.png"),
                                       QString("插入行"),this,SLOT(insertRow()));
    removeAct = contextMenu->addAction(QIcon(":/images/remove.png"),
                           QString("删除所选行"),this,SLOT(removeSelectedRows()));

    contextMenu->addSeparator();
    submitAct = contextMenu->addAction(QIcon(":/images/save.png"),
            QString("提交更新"),this,SLOT(submit()));
    cancelAct = contextMenu->addAction(QIcon(":/images/cancel.png"),
                                       QString("取消更新"),model,SLOT(revertAll()));
}

void ShowTableWidget::init_toolbar()
{
    toolbar = new QToolBar(this);
    toolbar->addAction(appendAct);
    toolbar->addAction(insertAct);
    toolbar->addAction(removeAct);
    toolbar->addAction(submitAct);
    toolbar->addAction(cancelAct);
    toolbar->setMaximumHeight(24);
}

void ShowTableWidget::appendRow()
{
    model->insertRow(model->rowCount());
}

void ShowTableWidget::submit()
{
    bool ret = model->submitAll();
    if(!ret){
        QSqlError se = model->lastError();

        QString err = QString("database:%1\ndriver:%2\nerror number:%3\nerror type:%4\nerror text:%5")
                .arg(se.databaseText())
                .arg(se.driverText())
                .arg(se.number())
                .arg(se.type()).arg(se.text());
        QMessageBox::warning(this,QString("数据保存错误"),
                             err);
    }
}

void ShowTableWidget::insertRow()
{
    QModelIndexList list = view->selectionModel()->selectedIndexes();

    if(list.count()>0){
        int i= list.at(0).row();
        model->insertRow(i);
    }else{
        model->insertRow(view->currentIndex().row());
    }
}

void ShowTableWidget::removeSelectedRows()
{
    QModelIndexList list = view->selectionModel()->selectedRows();
    model->removeRows(list.at(0).row(),list.count());
//    for(int i=0;i<list.count();i++){
//        int row  = list.at(i).row();
//        model->removeRow(row);
//    }
    //QMessageBox::information(this,QString("debug"),
      //      QString("删除所选行"));

}

void ShowTableWidget::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    isDirty = true;
}

void ShowTableWidget::rowsInserted(const QModelIndex &parent, int start, int end)
{
    isDirty = true;
}

void ShowTableWidget::rowsRemoved(const QModelIndex &parent, int start, int end)
{
    isDirty = true;
}

void ShowTableWidget::showContextMenu(QPoint p)
{
    contextMenu->exec(QCursor::pos());
}

bool ShowTableWidget::maybeSave()
{
    if(isDirty){
        QMessageBox::StandardButton ret;
        QString db = QSqlDatabase::database(con).databaseName();

        ret = QMessageBox::warning(this, QString("警告"),
                     QString("'%1.%2' 已经更改.是否保存？").arg(db).arg(table),
                     QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return model->submitAll();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}
void ShowTableWidget::closeEvent(QCloseEvent *event)
{
    if(maybeSave()){
        event->accept();
        deleteObject(table,con);
    }else{
        event->ignore();
    }
}
