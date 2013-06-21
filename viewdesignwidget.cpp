#include <QtGui>
#include <QtSql>
#include "mainwindow.h"
#include "stackedwidget.h"
#include "viewdesignwidget.h"
enum GetState
{
    GetBegin,
    GetCreate,
    GetTemp,
    GetView,
    GetName,
    GetEnd
};

struct ViewDesignObjects{
    QMap<QPair<QString,QString>,ViewDesignWidget*> widgets;
};

ViewDesignObjects* ViewDesignWidget::objs=0;
ViewDesignWidget* ViewDesignWidget::createObject(const QString &view_name,
                                               const QString &con_name,
                                               bool &isExist)
{

    static int n=1;
    bool flag = false;
    QString vname = view_name;
    if(view_name.isEmpty()){
        vname = QString("未命名%1").arg(n);
        n++;
        flag = true;
    }
    QPair<QString,QString> pair;

    pair.first = vname;
    pair.second = con_name;

    if(objs->widgets.contains(pair))
    {
        isExist = true;
        return objs->widgets[pair];
    }else{
        isExist = false;
        ViewDesignWidget *widget = new ViewDesignWidget;
        widget->init(vname,con_name,flag);
        objs->widgets.insert(pair,widget);
        return widget;
    }
}
void ViewDesignWidget::deleteObject(const QString &table_name, const QString &constr)
{
    QPair<QString,QString> pair;

    pair.first = table_name;
    pair.second = constr;

    objs->widgets.remove(pair);
}
void ViewDesignWidget::initObjContainer()
{
    objs = new ViewDesignObjects;
}

void ViewDesignWidget::delObjContainer()
{
    objs->widgets.clear();

    delete objs;
}


ViewDesignWidget::ViewDesignWidget()
{
}

void ViewDesignWidget::init(const QString &view_name,
                             const QString &con_name,
                             bool _create_flag)
{
    QSqlDatabase db = QSqlDatabase::database(con_name);
    create_flag = _create_flag;
    viewname = view_name;
    conname = con_name;
    dbname = db.databaseName();

    txtName = new QLineEdit;
    txtName->setText(viewname);
    cbTemp = new QCheckBox;

    QFormLayout *layout = new QFormLayout;

    layout->addRow(QString("视图名"),txtName);
    layout->addRow(QString("临时"),cbTemp);

    btnCreate = new QPushButton(QString("创建"),this);
    //btnCreate->setEnabled(create_flag);
    connect(btnCreate,SIGNAL(clicked()),
            this,SLOT(run()));

    QLabel *lblSql = new QLabel(QString("SQL"));

    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addWidget(lblSql);
    hbox->addStretch(1);
    hbox->addWidget(btnCreate);

    sqlEdit = new QTextEdit;
    //sqlEdit->setEnabled(create_flag);
    QString sql = QString("select sql from sqlite_master where name='%1'")
            .arg(viewname);
    QSqlQuery query(db);
    if(query.exec(sql)){
        query.first();
        sqlEdit->setText(query.value(0).toString());
    }

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addLayout(layout);
    vbox->addLayout(hbox);
    vbox->addWidget(sqlEdit,1);

    this->setLayout(vbox);

    setWindowTitle(QString("视图%1.%2").arg(dbname).arg(viewname));

    setEnabled(create_flag);
}

void ViewDesignWidget::run()
{
    QString sql = sqlEdit->toPlainText();
    if(sql.isEmpty())return;

    QSqlDatabase db = QSqlDatabase::database(conname);
    QSqlQuery query(db);

    if(query.exec(sql)){
        create_flag = false;
        setEnabled(create_flag);
        bool ok = getNameFromSql(sql,viewname);
        if(ok){
            txtName->setText(viewname);
        }
        MainWindow *mw = (MainWindow*)QApplication::activeWindow();
        mw->refresh(conname);
        StackedWidget *sw = mw->mdiArea();
        sw->setWidgetTitle(
                    this,QString("视图%1.%2").arg(dbname).arg(viewname));

    }else{
        QSqlError se = query.lastError();
        QString err = QString(
           "database:%1\ndriver:%2\nerror number:%3"
           "\nerror type:%4\nerror text:%5")
                .arg(se.databaseText())
                .arg(se.driverText())
                .arg(se.number())
                .arg(se.type()).arg(se.text());
        QMessageBox::warning(this,QString("创建错误"),err);
    }
}


void ViewDesignWidget::closeEvent(QCloseEvent *event)
{
    event->accept();
    deleteObject(viewname,conname);
}

bool ViewDesignWidget::getNameFromSql(const QString &sql,QString &name)
{
    QStringList list = sql.split(" ", QString::SkipEmptyParts);
    int state=GetBegin;
    int size = list.size();
    int index = 0;
    bool error = false;

    while(index < size){
        QString word = list.value(index).toLower();
        if(state == GetBegin){
            if(word == QString("create")){
                state = GetCreate;
            }else{
                error = true;
                break;
            }
        }else if(state == GetCreate){
            if(word == QString("temp")){
                state = GetTemp;
            }else if(word == QString("temporary")){
                state = GetTemp;
            }else if(word == QString("view")){
                state = GetView;
            }else{
                error = true;
                break;
            }
        }else if(state == GetView){
            state = GetName;
            name = word;
            break;
        }
        index ++;
    }

    if(error)return false;

    QString str = QString("select name from sqlite_master"
                          "where type == 'view' and name == '%1'").arg(name);
    QSqlDatabase db = QSqlDatabase::database(conname);
    QSqlQuery query(db);
    if(query.exec(str)){
        query.first();
        if(!query.value(0).toString().isEmpty())
            return true;
        else
            return false;

    }
    return !error;
}
