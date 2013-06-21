/*

class TableKeyHeader:public QHeaderView
{
    Q_OBJECT
public:
    TableKeyHeader(Qt::Orientation ori,QWidget *parent=0):QHeaderView(ori,parent){}
    TableDesignModel *tmodel;
    QTableView *owner;
    void paintSection(QPainter *painter,
                      const QRect &rect, int logicalIndex) const;
};


void TableKeyHeader::paintSection(QPainter *painter,
                                  const QRect &rect,
                                  int logicalIndex) const
{
    //owner->rootIndex();
    if(owner->selectionModel()->isRowSelected(
                logicalIndex,owner->rootIndex()))
        painter->fillRect(rect,
                          QColor(51,204,102,122));
    //QHeaderView::paintSection(painter,rect,logicalIndex);
    //bool pk = tmodel->sqlite_column(logicalIndex)->isPK;
    bool pk = true;

    QRect rc = rect;
    int size=14;
    rc.setRect(rc.left()+(rc.width()-size)/2,rc.top()+(rc.height()-size)/2,size,size);

    if(pk)
        painter->drawPixmap(rc,QPixmap(":/images/key.png"));
    else
        painter->fillRect(rc,painter->background());
    painter->setPen(QColor(153,153,153,153));
    painter->drawLine(rect.bottomLeft(),rect.bottomRight());
}


    //KeyDelegate *kd = new KeyDelegate(hv);
    //hv->setItemDelegate(kd);
    //hv->setItemDelegate(kd);
//    TableKeyHeader *hv = new TableKeyHeader(Qt::Vertical,tbFields);
//    hv->owner = tbFields;
//    hv->setResizeMode(QHeaderView::ResizeToContents);
//    hv->setFixedWidth(0);
//    hv->tmodel = model;
//    tbFields->setVerticalHeader(hv);
//    hv->setContextMenuPolicy(Qt::CustomContextMenu);
//    connect(hv,SIGNAL(customContextMenuRequested(QPoint)),
//            this,SLOT(showContextMenu(QPoint)));
//    connect(hv,SIGNAL(sectionClicked(int)),
//            this,SLOT(sectionClicked(int)));
//    connect(hv,SIGNAL(sectionPressed(int)),
//            tbFields,SLOT(selectRow(int)));

class CheckBox:public QWidget
{
    Q_OBJECT
public:
    CheckBox(QWidget *parent=0);
    void setChecked(bool v);
    bool isChecked();
protected:
    void keyReleaseEvent(QKeyEvent *);
    void mousePressEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);

    bool value;
};


CheckBox::CheckBox(QWidget *parent):QWidget(parent){
    value = false;
}

void CheckBox::setChecked(bool v){
    value = v;
    update();
}

bool CheckBox::isChecked()
{
    return value;
}

void CheckBox::keyReleaseEvent(QKeyEvent *e)
{
    if(e->key()==Qt::Key_Enter || e->key()==Qt::Key_Space){
        value = !value;
        update();
    }
}

void CheckBox::mousePressEvent(QMouseEvent *e){
    value = !value;
    update();
}

void CheckBox::paintEvent(QPaintEvent *e){

    QRect rc = e->rect();
    int size = 16;
    rc.setRect(rc.left()+1+(rc.width()-size)/2,
               rc.top()+1+(rc.height()-size)/2,
               size,size);

    QPainter painter(this);
    if(value)
        painter.drawPixmap(rc,QPixmap(":/images/check.png"));
    else
        painter.drawPixmap(rc,QPixmap(":/images/uncheck.png"));
}

QWidget* CheckDelegate::createEditor(QWidget *parent,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
    CheckBox *editor = new CheckBox(parent);
    editor->setGeometry(option.rect);
    editor->show();
    return editor;
}

void CheckDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    CheckBox *ed = qobject_cast<CheckBox*>(editor);
    ed->setChecked(index.data(Qt::EditRole).toBool());
}

void CheckDelegate::setModelData(QWidget *editor,
                                QAbstractItemModel *model,
                                const QModelIndex &index) const
{
    CheckBox *ed=qobject_cast<CheckBox*>(editor);
    model->setData(index,QVariant(ed->isChecked()),Qt::EditRole);
}

void CheckDelegate::updateEditorGeometry(QWidget *editor,
                                         const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}

5月31日，Check表达式不好弄。目前看，最好的办法是不要去增加Check约束了。或者
不要增加关于整个表的Check了。

6月1日：13:23
多窗口之间的同步更新，一直是一个问题。
主要有dbtree,contentscontainer,page三者之间的同步。要解决好这个问题啊，其实，
理想的解决方案啊，是应该设立一个共同的数据类。使用观察者模式来解决这个问题。
不过眼下显然这样做是来不及了。
在这里，将dbtree的模型类作为数据类的基类。是一个现在看来比较好的解决办法。
不过真正的解决之道应该是，设计一个包含多个数据库的数据模型类集合。

21:53,添加了四列，生日，家乡，所在地，地址，都不允许空，同时设了默认值，但是没有成功。
这个问题需要改正。
在解决了这个问题之后。在创建一个列很多很多的表。我看可以参考佳缘这样的网站。看看效果
如何。实际上，表的修改所涉及到的主要是解析。如果可以创建复杂的表。再对该表进行修改。

21：57
现在可以解决一个修改表名的功能。

    QTreeWidgetItem *tableitem =new QTreeWidgetItem(dbitem,TableTypeItem);
    tableitem->setText(0,QString("表"));
    tableitem->setIcon(0,QIcon(":/images/node.png"));
    tableitem->setExpanded(true);

    QStringList tables = db.tables(QSql::AllTables);
    QTreeWidgetItem *item=0,*column=0,*columns=0;
    for(int i=0;i<tables.size();++i)
    {
        item = new QTreeWidgetItem(tableitem,TableItem);
        item->setText(0,tables.at(i));
        item->setIcon(0,QIcon(":/images/table.png"));
        item->setFlags(dbitem->flags()|Qt::ItemIsEditable);

        columns = new QTreeWidgetItem(item,ColumnTypeItem);
        columns->setText(0,QString("列"));
        columns->setIcon(0,QIcon(":/images/node.png"));

        QSqlRecord record = db.record(tables.at(i));
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
        columns->setText(0,QString("列"));

        QSqlRecord record = db.record(tables.at(i));
        for(int j=0;j<record.count();++j)
        {
            column = new QTreeWidgetItem(columns,ColumnItem);
            column->setText(0,record.fieldName(j));
        }
    }

    QTreeWidgetItem *tableitem =new QTreeWidgetItem(dbitem,TableTypeItem);
    tableitem->setText(0,QString("表"));
    tableitem->setIcon(0,QIcon(":/images/node.png"));
    tableitem->setExpanded(true);

    QStringList tables = db.tables(QSql::AllTables);
    QTreeWidgetItem *item=0,*column=0,*columns=0;
    for(int i=0;i<tables.size();++i)
    {
        item = new QTreeWidgetItem(tableitem,TableItem);
        item->setText(0,tables.at(i));
        item->setIcon(0,QIcon(":/images/table.png"));
        item->setFlags(dbitem->flags()|Qt::ItemIsEditable);

        columns = new QTreeWidgetItem(item,ColumnTypeItem);
        columns->setText(0,QString("列"));
        columns->setIcon(0,QIcon(":/images/node.png"));

        QSqlRecord record = db.record(tables.at(i));
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
        columns->setText(0,QString("列"));

        QSqlRecord record = db.record(tables.at(i));
        for(int j=0;j<record.count();++j)
        {
            column = new QTreeWidgetItem(columns,ColumnItem);
            column->setText(0,record.fieldName(j));
        }
    }

*/
