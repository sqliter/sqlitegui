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

5��31�գ�Check���ʽ����Ū��Ŀǰ������õİ취�ǲ�Ҫȥ����CheckԼ���ˡ�����
��Ҫ���ӹ����������Check�ˡ�

6��1�գ�13:23
�ര��֮���ͬ�����£�һֱ��һ�����⡣
��Ҫ��dbtree,contentscontainer,page����֮���ͬ����Ҫ�����������Ⱑ����ʵ��
����Ľ������������Ӧ������һ����ͬ�������ࡣʹ�ù۲���ģʽ�����������⡣
����������Ȼ���������������ˡ�
�������dbtree��ģ������Ϊ������Ļ��ࡣ��һ�����ڿ����ȽϺõĽ���취��
���������Ľ��֮��Ӧ���ǣ����һ������������ݿ������ģ���༯�ϡ�

21:53,��������У����գ����磬���ڵأ���ַ����������գ�ͬʱ����Ĭ��ֵ������û�гɹ���
���������Ҫ������
�ڽ�����������֮���ڴ���һ���кܶ�ܶ�ı��ҿ����Բο���Ե��������վ������Ч��
��Ρ�ʵ���ϣ�����޸����漰������Ҫ�ǽ�����������Դ������ӵı��ٶԸñ�����޸ġ�

21��57
���ڿ��Խ��һ���޸ı����Ĺ��ܡ�

    QTreeWidgetItem *tableitem =new QTreeWidgetItem(dbitem,TableTypeItem);
    tableitem->setText(0,QString("��"));
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
        columns->setText(0,QString("��"));
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
    viewitem->setText(0,QString("��ͼ"));
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
        columns->setText(0,QString("��"));

        QSqlRecord record = db.record(tables.at(i));
        for(int j=0;j<record.count();++j)
        {
            column = new QTreeWidgetItem(columns,ColumnItem);
            column->setText(0,record.fieldName(j));
        }
    }

    QTreeWidgetItem *tableitem =new QTreeWidgetItem(dbitem,TableTypeItem);
    tableitem->setText(0,QString("��"));
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
        columns->setText(0,QString("��"));
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
    viewitem->setText(0,QString("��ͼ"));
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
        columns->setText(0,QString("��"));

        QSqlRecord record = db.record(tables.at(i));
        for(int j=0;j<record.count();++j)
        {
            column = new QTreeWidgetItem(columns,ColumnItem);
            column->setText(0,record.fieldName(j));
        }
    }

*/
