#include <QtGui>
#include <QtSql>
#include "sqlparse.h"
#include "tabledesignmodel.h"
#include "delegate.h"
#include "tabledesignwidget.h"
#include "mainwindow.h"
#include "stackedwidget.h"

struct TableDesignObjects{
    QMap<QPair<QString,QString>,TableDesignWidget*> widgets;
};

TableDesignObjects* TableDesignWidget::objs=0;
TableDesignWidget* TableDesignWidget::createObject(const QString &table_name,
                                               const QString &constr,
                                               bool &isExist)
{
    static int n=1;
    bool flag = false;
    QString tbname = table_name;
    if(table_name.isEmpty()){
        tbname = QString("未命名%1").arg(n);
        n++;
        flag = true;
    }
    QPair<QString,QString> pair;

    pair.first = tbname;
    pair.second = constr;

    if(objs->widgets.contains(pair))
    {
        isExist = true;
        return objs->widgets[pair];
    }else{
        isExist = false;
        TableDesignWidget *widget = new TableDesignWidget;
        widget->init(tbname,constr,flag);
        objs->widgets.insert(pair,widget);
        return widget;
    }
}
void TableDesignWidget::deleteObject(const QString &table_name, const QString &constr)
{
    QPair<QString,QString> pair;

    pair.first = table_name;
    pair.second = constr;

    objs->widgets.remove(pair);
}
void TableDesignWidget::initObjContainer()
{
    objs = new TableDesignObjects;
}

void TableDesignWidget::delObjContainer()
{
    objs->widgets.clear();

    delete objs;
}

QList<QWidget*>* TableDesignWidget::dbTables(const QString &constr)
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

        TableDesignWidget *widget = objs->widgets.find(pair).value();
        ret->append(widget);
    }
    return ret;

    //QMap<QPair<QString,QString>,ShowTableWidget*>;
}
TableDesignWidget::TableDesignWidget()
{

}

void TableDesignWidget::init(const QString &table_name,
                             const QString &constr,
                             bool _create_flag)
{
    create_flag = _create_flag;
    tbname = table_name;
    con = constr;
    dbname = QSqlDatabase::database(con).databaseName();

    main = new QSplitter(this);

    init_model();
    init_fields_table();

    init_menu();
    init_toolbar();

    init_field_widget();

    connect(model,SIGNAL(dirtyChanged(bool)),
            saveAct,SLOT(setEnabled(bool)));

    main->addWidget(tbFields);
    main->addWidget(tabField);
    main->setOrientation(Qt::Vertical);
    main->setStretchFactor(0,1);
    main->setStretchFactor(1,0);
    if(tbname.isEmpty())
        tbname = QString("未命名");
    setWindowTitle(QString("表%1").arg(tbname));

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(toolbar);
    vbox->addWidget(main);
    vbox->setMargin(0);
    vbox->setSpacing(0);

    this->setLayout(vbox);
}

void TableDesignWidget::init_model()
{
    model = new TableDesignModel;
    if(create_flag){
        model->createTable(tbname,con);
    }else{
        int err=0;
        model->openTable(tbname,con,err);
        if(err>0){
            QMessageBox::warning(this,
                                 QString("解析出错"),
      QString("解析表结构出现错误，修改表的后果无法预料，\n最好结束修改，您可以将该错误报告软件开发者!"));
        }
    }
    model->pk(pk_rows);

    current_column = 0;
    connect(model,SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this,SLOT(dataChanged(QModelIndex,QModelIndex)));
}

void TableDesignWidget::init_menu(){
    contextMenu = new QMenu(tbFields);

    appendAct = contextMenu->addAction(QIcon(":/images/new.png"),
                                       QString("追加行"),this,SLOT(appendRow()));
    insertAct = contextMenu->addAction(QIcon(":/images/insert.png"),
                                       QString("插入行"),this,SLOT(insertRow()));
    removeAct = contextMenu->addAction(QIcon(":/images/remove.png"),
                           QString("删除所选行"),this,SLOT(removeSelectedRows()));

    contextMenu->addSeparator();
    saveAct = contextMenu->addAction(QIcon(":/images/save.png"),
            QString("保存"),this,SLOT(save()));

    pkAct = contextMenu->addAction(QIcon(":/images/key.png"),
                                       QString("主键"),this,SLOT(setPK()));
    uniqueAct = contextMenu->addAction(QIcon(":/images/star.png"),
                QString("唯一性(Unique)"),this,SLOT(setUnique()));

}

void TableDesignWidget::init_toolbar()
{
    checkAct = new QAction(QIcon(":/images/yes.png"),QString("Check"),this);
    checkAct->setToolTip(QString("定义表Check约束"));
    connect(checkAct,SIGNAL(triggered()),this,SLOT(setCheck()));

    toolbar = new QToolBar(this);
    toolbar->setMaximumHeight(24);

    toolbar->addAction(saveAct);
    toolbar->addSeparator();
    toolbar->addAction(appendAct);
    toolbar->addAction(insertAct);
    toolbar->addAction(removeAct);
    if(create_flag){
        toolbar->addSeparator();
        toolbar->addAction(pkAct);
        toolbar->addAction(uniqueAct);
        toolbar->addAction(checkAct);
    }
    saveAct->setEnabled(create_flag);
    pkAct->setVisible(create_flag);
    uniqueAct->setVisible(create_flag);
}

void TableDesignWidget::init_fields_table()
{
    tbFields = new QTableView(main);
    tbFields->setModel(model);
    //tbFields->setSelectionBehavior(QAbstractItemView::SelectRows);

    tbFields->resizeRowsToContents();

    for(int i=0;i<model->columnCount();i++){
        tbFields->setColumnWidth(i,model->colwidth(i));
    }
    tbFields->horizontalHeader()->setResizeMode(0,QHeaderView::Fixed);

    TextDelegate *td = new TextDelegate(tbFields);
    td->owner = tbFields;
    tbFields->setItemDelegate(td);

    KeyDelegate *kd = new KeyDelegate(tbFields);
    kd->owner = tbFields;
    tbFields->setItemDelegateForColumn(0,kd);

    TypeDelegate *tyd = new TypeDelegate(tbFields);
    tyd->owner = tbFields;
    tbFields->setItemDelegateForColumn(2,tyd);

    CheckDelegate *cd=new CheckDelegate(tbFields);
    cd->owner = tbFields;
    tbFields->setItemDelegateForColumn(4,cd);
    tbFields->setItemDelegateForColumn(5,cd);

    QHeaderView *hv = tbFields->verticalHeader();
    hv->setFixedWidth(0);

    QItemSelectionModel *sm = tbFields->selectionModel();
    connect(sm,SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this,SLOT(currentRowChanged(QModelIndex,QModelIndex)));
    connect(sm,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this,SLOT(selectionChanged(QItemSelection,QItemSelection)));

    tbFields->setTabKeyNavigation(true);
    connect(tbFields,SIGNAL(clicked(QModelIndex)),
            this,SLOT(itemClicked(QModelIndex)));

    tbFields->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tbFields,SIGNAL(customContextMenuRequested(QPoint)),
            this,SLOT(showContextMenu(QPoint)));
}

void TableDesignWidget::init_field_widget()
{
    QStringList conflicts;
    conflicts<<QString()<<QString("Rollback")<<QString("Abort")
            <<QString("Fail")<<QString("Ignore")<<QString("Replace");

    tabField = new QTabWidget;

    field_widget = new QWidget(tabField);
    tabField->addTab(field_widget,QString("列"));

    chkAutoInc = new QCheckBox(QString());
    cbPkConflict = new QComboBox;
    cbPkConflict->addItems(conflicts);
    cbNotNullConflict = new QComboBox;
    cbNotNullConflict->addItems(conflicts);
    cbUniqueConflict = new QComboBox;
    cbUniqueConflict->addItems(conflicts);
    txtCollate = new QLineEdit(QString());
    txtPkSortOrder = new QLineEdit;

    QFormLayout *formlayout = new QFormLayout(field_widget);

    formlayout->addRow(QString("自动增量"),chkAutoInc);
    formlayout->addRow(QString("主键排序"),txtPkSortOrder);
    formlayout->addRow(QString("主键约束冲突"),cbPkConflict);
    formlayout->addRow(QString("空约束冲突"),cbNotNullConflict);
    formlayout->addRow(QString("唯一性约束冲突"),cbUniqueConflict);
    formlayout->addRow(QString("Collate约束"),txtCollate);

    field_widget->setLayout(formlayout);

    chkAutoInc->setEnabled(false);
    cbPkConflict->setEnabled(false);
    txtPkSortOrder->setEnabled(false);
    cbNotNullConflict->setEnabled(false);
    cbUniqueConflict->setEnabled(false);
    txtCollate->setEnabled(false);
}

void TableDesignWidget::selectionChanged(const QItemSelection &selected,
                                         const QItemSelection &deselected)
{

    QModelIndexList list = tbFields->selectionModel()->selectedIndexes();

    selected_rows.clear();

    int count = list.count();

    if(count>0){
        for(int i=0;i<count;++i){
            int row = list.value(i).row();
            QString name = model->column(row)->name;
            if(!name.isEmpty() && name!=QString("")){
                if(!selected_rows.contains(row))
                {
                    selected_rows.append(row);
                }
            }
        }
    }else{
        int row = tbFields->currentIndex().row();

        QString name = model->column(row)->name;
        if(!name.isEmpty() && name!=QString("")){
            selected_rows.append(row);
        }
    }

    int selected_count = selected_rows.count();

    if(selected_count>0 && !create_flag){
        bool flag = true;
        for(int i=0;i<selected_count;i++){
            SqliteColumn *col=model->column(selected_rows[i]);
            if(col->state==ColValid){
                flag = false;
                break;
            }
        }
        removeAct->setEnabled(flag);
        insertAct->setEnabled(flag);
    }
    if(!create_flag)return;
    if(selected_count>0){
        pkAct->setEnabled(true);
        uniqueAct->setEnabled(true);

        pk_same = false;
        if(selected_count == pk_rows.count()){
            pk_same = true;
            for(int i=0;i<selected_count;++i){
                if(selected_rows[i]!=pk_rows[i])
                {
                    pk_same = false;
                    break;
                }
            }
        }

        uq_same = false;
        if(selected_count == uq_rows.count()){
            uq_same = true;
            for(int i=0;i<selected_count;++i){
                if(selected_rows[i] != uq_rows[i])
                {
                    uq_same = false;
                    break;
                }
            }
        }

        if(pk_same){
            pkAct->setText(QString("取消主键设置"));
        }else
        {
            pkAct->setText(QString("设置主键"));
        }

        if(uq_same){
            uniqueAct->setText(QString("取消唯一性设置"));
        }else
        {
            uniqueAct->setText(QString("设置唯一性"));
        }

    }else{
        pkAct->setEnabled(false);
        uniqueAct->setEnabled(false);
    }
}

void TableDesignWidget::currentRowChanged(const QModelIndex &current,
                                          const QModelIndex &previous)
{
    //保存field_widget中的值到前面的行中。
    if(previous.isValid()){
        SqliteColumn *pre_column = model->column(previous.row());
        pre_column->auto_inc = chkAutoInc->isChecked();
        pre_column->pkconflict = cbPkConflict->currentText();
        pre_column->pksort_order = txtPkSortOrder->text();
        pre_column->notnullconflict = cbNotNullConflict->currentText();
        pre_column->unique_conflict = cbUniqueConflict->currentText();
        pre_column->conlate =txtCollate->text();
    }

    //使用当前项更新widget

    if(current.isValid()){
        current_column = model->column(current.row());

        updateWidgetEnables(current);

        chkAutoInc->setChecked(current_column->auto_inc);
        txtPkSortOrder->setText(current_column->pksort_order);
        if(pk_rows.count()==1)
            setComboText(cbPkConflict,current_column->pkconflict);
        else
            setComboText(cbPkConflict,model->pkconflict());

        setComboText(cbNotNullConflict,current_column->notnullconflict);
        if(uq_rows.count()>1)
            setComboText(cbUniqueConflict,model->uqconflict());
        else
            setComboText(cbUniqueConflict,current_column->unique_conflict);

        txtCollate->setText(current_column->conlate);
    }
}

void TableDesignWidget::dataChanged(const QModelIndex &topLeft,
                                    const QModelIndex &bottomRight)
{
    if(topLeft.isValid()){
        updateWidgetEnables(topLeft);
    }
}

void TableDesignWidget::itemClicked(const QModelIndex &index){
    if(index.column() == 0){
        tbFields->selectRow(index.row());
    }
    else if(index.column()==4){
        bool checked = index.data().toBool();
        model->setData(index,QVariant(!checked));
    }else if(index.column()==5){
        if(create_flag){
            bool checked = index.data().toBool();
            model->setData(index,QVariant(!checked));
        }
    }
}

void TableDesignWidget::showContextMenu(QPoint p)
{
    contextMenu->exec(QCursor::pos());
}

void TableDesignWidget::appendRow()
{
    model->insertRows(model->rowCount(),10);
}

void TableDesignWidget::insertRow()
{
    QModelIndexList list = tbFields->selectionModel()->selectedIndexes();

    if(list.count()>0){
        int i= list.at(0).row();
        model->insertRow(i);
    }else{
        model->insertRow(tbFields->currentIndex().row());
    }
}

void TableDesignWidget::removeSelectedRows()
{
//    QModelIndexList list = tbFields->selectionModel()->selectedRows();
//    if(list.count()>0){
//        model->removeRows(list.at(0).row(),list.count());
//    }
    int count = selected_rows.count();
    for(int i=0;i<count;i++){
        model->removeRow(selected_rows[i]);
    }
}

void TableDesignWidget::save()
{
    if(create_flag){
        bool ok;
        tbname = QInputDialog::getText(
                    this,QString("保存表"),
                    QString("请输入表名："),QLineEdit::Normal,
                    tbname,&ok);
        if (ok){
            model->setTableName(tbname);
            QString msg;
            save_ok = model->saveTable(msg);
            if(save_ok){
                create_flag = false;
                MainWindow *mw = (MainWindow*)QApplication::activeWindow();
                mw->addTable(con,tbname);
                StackedWidget *sw = mw->mdiArea();
                sw->setWidgetTitle(this,QString("表%1").arg(tbname));
            }else{
                QMessageBox::warning(this,QString("保存错误"),msg);
            }
        }
    }else{
        QString msg;
        save_ok = model->saveTable(msg);
        if(!save_ok){
            QMessageBox::warning(this,QString("保存错误"),msg);
        }else{
            MainWindow *mw = (MainWindow*)QApplication::activeWindow();
            mw->addTable(con,tbname);
        }
    }

}

void TableDesignWidget::setPK()
{
    if(pk_same){
        model->cancelPk(pk_rows);
        pk_rows.clear();
        pk_same = false;
        pkAct->setText(QString("设置主键"));
    }else
    {
        pk_rows.clear();
        int selected_count = selected_rows.count();
        for(int i=0;i<selected_count;++i){
            pk_rows.append(selected_rows[i]);
        }
        model->setPk(pk_rows);
        pk_same = true;
        pkAct->setText(QString("取消主键设置"));
    }
    //不要删除这两行代码
    tbFields->setColumnHidden(0,true);
    tbFields->setColumnHidden(0,false);
    //不要删除这两行代码
    QModelIndex current = tbFields->currentIndex();

    SqliteColumn *cur_column = model->column(current.row());
    cbPkConflict->setEnabled(cur_column->isPK);
    if(pk_rows.count()>1)
    {
        txtPkSortOrder->setEnabled(false);
        chkAutoInc->setEnabled(false);
    }
    else{
        txtPkSortOrder->setEnabled(cur_column->isPK);
        bool chk=(cur_column->type==QString("int")
                  ||cur_column->type==QString("integer"));
        chk = chk && cur_column->isPK;
        chkAutoInc->setEnabled(chk);
    }
}

void TableDesignWidget::setUnique()
{
    if(uq_same){
        model->cancelUnique(uq_rows);
        uq_same = false;
        uniqueAct->setText(QString("设置唯一性"));

    }else
    {
        model->setUnique(selected_rows,uq_rows);
        uq_rows.clear();
        int selected_count = selected_rows.count();
        for(int i=0;i<selected_count;++i){
            uq_rows.append(selected_rows[i]);
        }
        uq_same = true;
        uniqueAct->setText(QString("取消唯一性设置"));
    }
    //不要删除这两行代码
    tbFields->setColumnHidden(5,true);
    tbFields->setColumnHidden(5,false);
    //不要删除这两行代码
    QModelIndex current = tbFields->currentIndex();

    SqliteColumn *cur_column = model->column(current.row());
    cbUniqueConflict->setEnabled(cur_column->isUnique);
}

void TableDesignWidget::setCheck()
{
    bool ok;
    QString check_exp = QInputDialog::getText(
                this,QString("定义表Check约束"),
                QString("表Check约束"),QLineEdit::Normal,
                model->check(),&ok);
    if (ok){
        //qDebug()<<check_exp;
        model->setCheck(check_exp);
    }
}

void TableDesignWidget::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
        deleteObject(tbname,con);
    } else {
        event->ignore();
    }
}

bool TableDesignWidget::maybeSave()
{
    if(model->isDirty()){
        QMessageBox::StandardButton ret;

        ret = QMessageBox::warning(this, QString("警告"),
                     QString("'%1.%2' 已经更改.是否保存？").arg(dbname).arg(tbname),
                     QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel);
        if (ret == QMessageBox::Save){
            save();
            return save_ok;
        }
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void TableDesignWidget::setComboText(QComboBox *cb, const QString &text)
{
    for(int i=0;i<cb->count();++i){
        if(text==cb->itemText(i)){
            cb->setCurrentIndex(i);
            return;
        }
    }
}

void TableDesignWidget::updateWidgetEnables(const QModelIndex &current)
{
    if(current.isValid()){
        SqliteColumn *cur_column = model->column(current.row());
        if(cur_column->state != ColValid && !cur_column->name.isEmpty()){
            cbPkConflict->setEnabled(cur_column->isPK);
            if(pk_rows.count()>1)
            {
                txtPkSortOrder->setEnabled(false);
                chkAutoInc->setEnabled(false);
            }
            else{
                txtPkSortOrder->setEnabled(cur_column->isPK);
                bool chk=(cur_column->type==QString("int")
                          ||cur_column->type==QString("integer"));
                chk = chk && cur_column->isPK;
                chkAutoInc->setEnabled(chk);
            }

            cbNotNullConflict->setEnabled(!cur_column->notnull);
            cbUniqueConflict->setEnabled(cur_column->isUnique);
            txtCollate->setEnabled(true);
        }else{
            chkAutoInc->setEnabled(false);
            cbPkConflict->setEnabled(false);
            txtPkSortOrder->setEnabled(false);
            cbNotNullConflict->setEnabled(false);
            cbUniqueConflict->setEnabled(false);
            txtCollate->setEnabled(false);
        }
    }
}
