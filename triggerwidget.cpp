#include <QtGui>
#include <QtSql>
#include "mainwindow.h"
#include "triggerwidget.h"

TriggerDialog::TriggerDialog()
{

}

void TriggerDialog::init_combobox()
{
    cbTable = new QComboBox;
    QSqlDatabase db = QSqlDatabase::database(conname);
    cbTable->addItems(db.tables());
    cbTable->addItems(db.tables(QSql::Views));

    int count = cbTable->count();
    for(int index=0;index<count;index++){
        if(cbTable->itemText(index)==tbname){
            cbTable->setCurrentIndex(index);
            return;
        }
    }
    cbTable->setCurrentIndex(0);
}

void TriggerDialog::init_editor()
{
    sqlEdit = new QTextEdit;

}

void TriggerDialog::init(const QString &con_name,
                       const QString &table_name,
                       const QString &trigger_name)
{
    conname = con_name;
    dbname = QSqlDatabase::database(con_name).databaseName();
    tbname = table_name;
    triggername = trigger_name;
    create_flag = trigger_name.isEmpty();

    QLabel *lblSql = new QLabel(QString("Sql"));
    init_editor();

    QLabel *lblName = new QLabel(QString("����"));
    txtName = new QLineEdit(triggername);

    QLabel *lblTable = new QLabel(QString("�����ͼ"));
    init_combobox();

    QGridLayout *grid = new QGridLayout;

    grid->addWidget(lblName,0,0);
    grid->addWidget(txtName,0,1);

    grid->addWidget(lblTable,1,0);
    grid->addWidget(cbTable,1,1);

    grid->setColumnStretch(1,1);

    btnOk = new QPushButton(QString("ȷ��"));
    btnCancel = new QPushButton(QString("ȡ��"));
    connect(btnOk,SIGNAL(clicked()),this,SLOT(create()));
    connect(btnCancel,SIGNAL(clicked()),this,SLOT(close()));

    QHBoxLayout *hboxButtons = new QHBoxLayout;
    hboxButtons->addStretch(1);
    hboxButtons->addWidget(btnOk);
    hboxButtons->addWidget(btnCancel);

    QVBoxLayout *vboxMain = new QVBoxLayout;
    vboxMain->addLayout(grid);
    vboxMain->addWidget(lblSql);
    vboxMain->addWidget(sqlEdit,1);
    vboxMain->addLayout(hboxButtons);

    setLayout(vboxMain);

    if(create_flag)
        setWindowTitle(QString("����������"));
    else
    {
        setWindowTitle(QString("������%1").arg(triggername));
        cbTable->setEnabled(false);
        sqlEdit->setReadOnly(true);
        txtName->setReadOnly(true);
    }

    resize(700,420);
}

void TriggerDialog::loadTrigger(const QString &sql)
{
    sqlEdit->setText(sql);
}

void TriggerDialog::create()
{
    if(!create_flag){
        close();
        return;
    }

    if(!check())return;

    QString sql = sqlEdit->toPlainText();

    QSqlDatabase db = QSqlDatabase::database(conname);
    QSqlQuery q(db);
    if(q.exec(sql)){
        MainWindow *mw = qobject_cast<MainWindow*>(qApp->activeWindow());
        if(mw!=0)mw->refresh(conname);
    }else{
        QSqlError se = q.lastError();
        QString err = QString(
           "database:%1\ndriver:%2\nerror number:%3"
           "\nerror type:%4\nerror text:%5")
                .arg(se.databaseText())
                .arg(se.driverText())
                .arg(se.number())
                .arg(se.type()).arg(se.text());
        QMessageBox::warning(this,QString("��������"),err);
    }
    close();
}

bool TriggerDialog::check()
{
    return false;
}
