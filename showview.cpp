#include <QtGui>
#include <QtSql>

#include "showview.h"
struct ShowViewObjects{
    QMap<QPair<QString,QString>,ShowViewWidget*> widgets;
};

ShowViewObjects* ShowViewWidget::objs=0;
ShowViewWidget* ShowViewWidget::createObject(const QString &view_name,
                                               const QString &constr,
                                               bool &isExist)
{
    QPair<QString,QString> pair;

    pair.first = view_name;
    pair.second = constr;

    if(objs->widgets.contains(pair))
    {
        isExist = true;
        return objs->widgets[pair];
    }else{
        isExist = false;
        ShowViewWidget *widget = new ShowViewWidget;
        widget->init(view_name,constr);
        objs->widgets.insert(pair,widget);
        return widget;
    }
}
void ShowViewWidget::deleteObject(const QString &table_name, const QString &constr)
{
    QPair<QString,QString> pair;

    pair.first = table_name;
    pair.second = constr;

    objs->widgets.remove(pair);
}
void ShowViewWidget::initObjContainer()
{
    objs = new ShowViewObjects;
}

void ShowViewWidget::delObjContainer()
{
    objs->widgets.clear();

    delete objs;
}

QList<QWidget*> ShowViewWidget::dbViews(const QString &constr)
{
    QList<QPair<QString,QString> > keys = objs->widgets.keys();
    QPair<QString,QString> pair;
    QStringList tables;

    foreach(pair,keys){
        if(pair.second==constr){
            tables.append(pair.first);
        }
    }

    QList<QWidget*> ret;
    foreach(QString table,tables){
        pair.first = table;
        pair.second = constr;

        ShowViewWidget *widget = objs->widgets.find(pair).value();
        ret.append(widget);
    }
    return ret;
}

QList<ShowViewWidget*> ShowViewWidget::allTables()
{
    return objs->widgets.values();
}

ShowViewWidget::ShowViewWidget()
{
}

void ShowViewWidget::init(const QString &view_name,
                          const QString &constr)
{
    view = view_name;
    con = constr;
    dbname = QSqlDatabase::database(con).databaseName();

    init_model();
    init_table();

    setWindowTitle(QString("йсм╪%1.%2").arg(dbname).arg(view));
}
void ShowViewWidget::init_table(){
    setModel(model);
    resizeRowsToContents();
}

void ShowViewWidget::init_model(){

    QSqlDatabase db = QSqlDatabase::database(con);
    model = new QSqlQueryModel;
    model->setQuery(QString("SELECT * FROM %1").arg(view),db);

    QSqlRecord record = db.record(view);
    for(int i=0;i<record.count();++i)
    {
        model->setHeaderData(i,Qt::Horizontal,record.fieldName(i));
    }
}

void ShowViewWidget::closeEvent(QCloseEvent *event)
{
    event->accept();
    deleteObject(view,con);
}

