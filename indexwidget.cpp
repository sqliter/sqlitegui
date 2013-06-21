#include <QtGui>
#include <QtSql>

#include "sqlparse.h"
#include "indexwidget.h"
#include "delegate.h"
#include "mainwindow.h"


IndexDialog::IndexDialog()
{

}

void IndexDialog::init_combobox()
{
    cbTable = new QComboBox;
    QSqlDatabase db = QSqlDatabase::database(conname);
    cbTable->addItems(db.tables());
    connect(cbTable,SIGNAL(currentIndexChanged(QString)),
            this,SLOT(tableChanged(QString)));

    int count = cbTable->count();
    for(int index=0;index<count;index++){
        if(cbTable->itemText(index)==tbname){
            cbTable->setCurrentIndex(index);
            return;
        }
    }
    cbTable->setCurrentIndex(0);
}

void IndexDialog::init_table()
{
    tbColumnList = new QTableWidget;
    tbColumnList->setColumnCount(4);
    tbColumnList->verticalHeader()->setHidden(true);
    QStringList headernames;
    headernames<<QString("")<<QString("字段")
              <<QString("文本Collate")<<QString("排序ASC/DESC");
    tbColumnList->setHorizontalHeaderLabels(headernames);

    CheckDelegate *cd = new CheckDelegate(tbColumnList);
    cd->owner = tbColumnList;
    tbColumnList->setItemDelegateForColumn(0,cd);
    TypeDelegate *td = new TypeDelegate(tbColumnList);
    td->owner = tbColumnList;
    td->item_id = 1;
    tbColumnList->setItemDelegateForColumn(3,td);

    QSqlDatabase db = QSqlDatabase::database(conname);
    if(tbname.isEmpty()){
        QStringList tbs = db.tables();
        if(tbs.count()>0)
            tbname = tbs.at(0);
    }
    QSqlRecord record = db.record(tbname);
    int count = record.count();
    tbColumnList->setRowCount(count);

    for(int i=0;i<count;i++){
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setData(Qt::EditRole,false);
        item->setFlags(Qt::ItemIsEnabled);
        tbColumnList->setItem(i,0,item);

        item = new QTableWidgetItem;
        item->setText(record.fieldName(i));
        tbColumnList->setItem(i,1,item);

        item = new QTableWidgetItem;
        item->setText(QString());
        tbColumnList->setItem(i,2,item);

        item = new QTableWidgetItem;
        item->setText(QString());
        tbColumnList->setItem(i,3,item);
    }

    connect(tbColumnList,SIGNAL(cellClicked(int,int)),
            this,SLOT(cellClicked(int,int)));
}

void IndexDialog::init(const QString &con_name,
                       const QString &table_name,
                       const QString &index_name)
{
    conname = con_name;
    dbname = QSqlDatabase::database(con_name).databaseName();
    tbname = table_name;
    indexname = index_name;
    create_flag = index_name.isEmpty();

    QLabel *lblColList = new QLabel(QString("勾选并设置作为索引的字段"));
    init_table();

    QLabel *lblName = new QLabel(QString("名称"));
    txtName = new QLineEdit(indexname);

    QLabel *lblUnique = new QLabel(QString("是否唯一"));
    ckUnique = new QCheckBox;
    ckUnique->setCheckable(create_flag);
    ckUnique->setChecked(false);

    QLabel *lblTable = new QLabel(QString("表"));
    init_combobox();

    QGridLayout *grid = new QGridLayout;

    grid->addWidget(lblName,0,0);
    grid->addWidget(txtName,0,1);

    grid->addWidget(lblUnique,1,0);
    grid->addWidget(ckUnique,1,1);

    grid->addWidget(lblTable,2,0);
    grid->addWidget(cbTable,2,1);

    grid->setColumnStretch(1,1);

    btnOk = new QPushButton(QString("确定"));
    btnCancel = new QPushButton(QString("取消"));
    connect(btnOk,SIGNAL(clicked()),this,SLOT(create()));
    connect(btnCancel,SIGNAL(clicked()),this,SLOT(close()));

    QHBoxLayout *hboxButtons = new QHBoxLayout;
    hboxButtons->addStretch(1);
    hboxButtons->addWidget(btnOk);
    hboxButtons->addWidget(btnCancel);

    QVBoxLayout *vboxMain = new QVBoxLayout;
    vboxMain->addLayout(grid);
    vboxMain->addWidget(lblColList);
    vboxMain->addWidget(tbColumnList,1);
    vboxMain->addLayout(hboxButtons);

    setLayout(vboxMain);

    if(create_flag)
        setWindowTitle(QString("创建索引"));
    else
    {
        setWindowTitle(QString("索引%1").arg(indexname));
        txtName->setReadOnly(true);
        ckUnique->setEnabled(false);
        cbTable->setEnabled(false);
        tbColumnList->setEnabled(false);
    }

    resize(700,420);
}

void IndexDialog::loadIndex(const QStringList &sl)
{
    int count = sl.count();
    int row_count = tbColumnList->rowCount();
    QTableWidgetItem *item=0;

    for(int i=0;i<row_count;i++){
        item = tbColumnList->item(i,1);
        for(int j=0;j<count;j+=3){
            if(sl.at(j) == item->text()){
                tbColumnList->item(i,0)->setData(Qt::EditRole,true);
                tbColumnList->item(i,2)->setText(sl.at(j+1));
                tbColumnList->item(i,3)->setText(sl.at(j+2));
            }
        }
    }

}

void IndexDialog::create()
{
    if(!create_flag){
        close();
        return;
    }
    QString unique;
    if(ckUnique->isChecked())
    {
        unique = QString("unique");
    }
    else
    {
        unique = QString();
    }
    QString iname = txtName->text();
    QString collist;

    int row_count = tbColumnList->rowCount();
    int selected_count=0;
    for(int i=0;i<row_count;i++){
        QTableWidgetItem *item = tbColumnList->item(i,0);
        bool isSelected = item->data(Qt::EditRole).toBool();
        if(isSelected){
            QString col;
            if(tbColumnList->item(i,2)->text().isEmpty()){
                col = QString("%1 %3")
                        .arg(tbColumnList->item(i,1)->text())
                        .arg(tbColumnList->item(i,3)->text());
            }else{
                col = QString("%1 Collate%2 %3")
                    .arg(tbColumnList->item(i,1)->text())
                    .arg(tbColumnList->item(i,2)->text())
                    .arg(tbColumnList->item(i,3)->text());
            }
            collist.append(col.trimmed());
            collist.append(",");
            selected_count ++;
        }
    }
    if(selected_count>0){
        collist = collist.left(collist.length()-1);
    }

    QString sql = QString("create %1 index if not exists %2 on %3(%4)")
            .arg(unique).arg(iname).arg(tbname).arg(collist);

    qDebug()<<sql;
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
        QMessageBox::warning(this,QString("创建错误"),err);
    }
    close();
}

void IndexDialog::tableChanged(const QString &tb)
{
    tbname = tb;
    tbColumnList->clearContents();

    QSqlDatabase db = QSqlDatabase::database(conname);
    QSqlRecord record = db.record(tbname);
    int count = record.count();
    tbColumnList->setRowCount(count);

    for(int i=0;i<count;i++){
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setData(Qt::EditRole,false);
        item->setFlags(Qt::ItemIsEnabled);
        tbColumnList->setItem(i,0,item);

        item = new QTableWidgetItem;
        item->setText(record.fieldName(i));
        tbColumnList->setItem(i,1,item);

        item = new QTableWidgetItem;
        item->setText(QString());
        tbColumnList->setItem(i,2,item);

        item = new QTableWidgetItem;
        item->setText(QString());
        tbColumnList->setItem(i,3,item);
    }
}

void IndexDialog::cellClicked(int row,int column){
    if(column == 0){
        if(create_flag){
            QTableWidgetItem *item = tbColumnList->item(row,column);
            bool checked = item->data(Qt::EditRole).toBool();
            item->setData(Qt::EditRole,!checked);
        }
    }
}
