
#include <QtGui>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlRecord>

#include "mainwindow.h"
#include "highlighter.h"
#include "querywidget.h"
#include "stackedwidget.h"

struct QueryObjects{
    QMap<QString,QueryWidget*> widgets;
};

QueryObjects* QueryWidget::objs=0;

QueryWidget* QueryWidget::createObject(const QString &file_name,bool &isExist)
{
    static int n=1;
    bool flag = false;
    QString fname = file_name;
    if(file_name.isEmpty()){
        fname = QString("查询%1.sql").arg(n);
        n++;
        flag = true;
    }

    if(objs->widgets.contains(fname))
    {
        isExist = true;
        return objs->widgets[fname];
    }else{
        isExist = false;
        QueryWidget *widget = new QueryWidget;
        widget->init(fname,flag);
        objs->widgets.insert(fname,widget);
        return widget;
    }
}
void QueryWidget::deleteObject(const QString &file_name)
{
    objs->widgets.remove(file_name);
}
void QueryWidget::initObjContainer()
{
    objs = new QueryObjects;
}

void QueryWidget::delObjContainer()
{
    objs->widgets.clear();

    delete objs;
}

QueryWidget::QueryWidget()
{
}

void QueryWidget::init(const QString &file_name,bool flag)
{
    init_combobox();
    init_editor();
    init_toolbar();
    init_splitter();

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(toolbar);
    vbox->addWidget(splitter,1);
    vbox->setMargin(0);
    vbox->setSpacing(0);

    this->setLayout(vbox);

    isUntitled = flag;
    if(flag){
        fileName = file_name;
        setWindowTitle(fileName);
        connect(queryEdit->document(), SIGNAL(contentsChanged()),
                this, SLOT(documentModified()));
    }else{
        loadFile(file_name);
    }
    main = (MainWindow*)QApplication::activeWindow();
    stackedParent = main->mdiArea();

    connect(main,SIGNAL(connectionChanged()),this,SLOT(update_combobox()));

    connect(queryEdit,SIGNAL(redoAvailable(bool)),
            redoAct,SLOT(setEnabled(bool)));
    connect(queryEdit,SIGNAL(undoAvailable(bool)),
            undoAct,SLOT(setEnabled(bool)));

    connect(queryEdit,SIGNAL(selectionChanged()),
            this,SLOT(selectionChanged()));
    QClipboard *clipboard = QApplication::clipboard();
    connect(clipboard,SIGNAL(dataChanged()),
            this,SLOT(clipboardChanged()));

}
void QueryWidget::init_combobox()
{
    cbDB = new QComboBox;
    connect(cbDB,SIGNAL(currentIndexChanged(int)),this,SLOT(selectDB(int)));

    QStringList conlist = QSqlDatabase::connectionNames();
    foreach(QString con,conlist){
        QSqlDatabase db = QSqlDatabase::database(con);
        QFileInfo fi(db.databaseName());
        cbDB->addItem(fi.baseName(),QVariant(con));
    }
    cbDB->setCurrentIndex(0);
}

void QueryWidget::init_editor()
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);

    queryEdit = new QTextEdit;
    queryEdit->setFont(font);

    highlighter = new Highlighter(queryEdit->document());
}
void QueryWidget::init_splitter()
{
    splitter =new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);

    resultEdit = new QTextEdit;
    resultTab = new QTabWidget;

    resultTab->setTabsClosable(true);

    splitter->addWidget(queryEdit);
    splitter->addWidget(resultTab);

    findView = new QWidget;

    query = 0;
    isInGrid = true;

    //resultTab->addTab(resultSplitter,QString("网格"));
    resultTab->addTab(resultEdit,QString("消息"));
    connect(resultTab,SIGNAL(tabCloseRequested(int)),
            this,SLOT(tabCloseRequested(int)));
}

void QueryWidget::init_toolbar()
{
    newAct = new QAction(QIcon(":/images/newquery.png"),
                                  QString("新建查询(&N)"),this);
    //newAct->setShortcuts(QKeySequence::New);
    newAct->setToolTip(QString("新建查询"));
    connect(newAct,SIGNAL(triggered()),this,SLOT(create()));

    openAct = new QAction(QIcon(":/images/open.png"),
                                   QString("打开查询(&O)"),this);
    openAct->setToolTip(QString("打开查询"));
    connect(openAct,SIGNAL(triggered()),this,SLOT(open()));

    saveAct = new QAction(QIcon(":/images/save.png"), QString("保存(&S)"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setToolTip(QString("保存到磁盘"));
    connect(saveAct,SIGNAL(triggered()),this,SLOT(save()));

    copyAct = new QAction(QIcon(":/images/copy.png"), QString("复制)"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setToolTip(QString("复制"));
    connect(copyAct,SIGNAL(triggered()),queryEdit,SLOT(copy()));

    cutAct = new QAction(QIcon(":/images/cut.png"), QString("剪切)"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setToolTip(QString("剪切"));
    connect(cutAct,SIGNAL(triggered()),queryEdit,SLOT(cut()));

    pasteAct = new QAction(QIcon(":/images/paste.png"), QString("粘贴)"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setToolTip(QString("粘贴"));
    connect(pasteAct,SIGNAL(triggered()),queryEdit,SLOT(paste()));

    delAct = new QAction(QIcon(":/images/remove.png"), QString("删除)"), this);
    delAct->setShortcuts(QKeySequence::Delete);
    delAct->setToolTip(QString("删除"));
    connect(delAct,SIGNAL(triggered()),this,SLOT(remove()));

    findAct = new QAction(QIcon(":/images/find.png"), QString("查找"), this);
    findAct->setShortcuts(QKeySequence::Find);
    findAct->setToolTip(QString("查找"));
    connect(findAct,SIGNAL(triggered()),this,SLOT(find()));

    redoAct = new QAction(QIcon(":/images/redo.png"), QString("重做"), this);
    redoAct->setShortcuts(QKeySequence::Redo);
    redoAct->setToolTip(QString("重做"));
    connect(redoAct,SIGNAL(triggered()),queryEdit,SLOT(redo()));

    undoAct = new QAction(QIcon(":/images/undo.png"), QString("撤销"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setToolTip(QString("撤销"));
    connect(undoAct,SIGNAL(triggered()),queryEdit,SLOT(undo()));

    resultAct = new QAction(QIcon(":/images/result.png"), QString("结果"), this);
    resultAct->setToolTip(QString("结果显示方式"));

    tblAct = new QAction(QString("以网格形式显示"),this);
    tblAct->setCheckable(true);
    tblAct->setChecked(true);
    connect(tblAct,SIGNAL(triggered()),this,SLOT(inGrid()));

    txtAct = new QAction(QString("以文本形式显示"),this);
    txtAct->setCheckable(true);
    connect(txtAct,SIGNAL(triggered()),this,SLOT(inTxt()));

    QMenu *resultMenu = new QMenu(this);
    resultMenu->addAction(tblAct);
    resultMenu->addAction(txtAct);

    resultAct->setMenu(resultMenu);

    checkAct = new QAction(QIcon(":images/yes.png"),QString("分析"),this);
    checkAct->setToolTip(QString("分析查询"));
    checkAct->setShortcut(QKeySequence("F8"));
    connect(checkAct,SIGNAL(triggered()),this,SLOT(check()));

    runAct = new QAction(QIcon(":/images/run.png"),
                                  QString("执行)"),this);
    runAct->setShortcut(QKeySequence("F5"));
    runAct->setToolTip(QString("执行查询"));
    connect(runAct,SIGNAL(triggered()),this,SLOT(execQuery()));

    stopAct = new QAction(QIcon(":/images/stop.png"),QString("停止"),this);
    stopAct->setToolTip(QString("停止"));
    stopAct->setShortcut(QKeySequence("F6"));
    stopAct->setEnabled(false);
    connect(stopAct,SIGNAL(triggered()),this,SLOT(stopQuery()));

    toolbar = new QToolBar(this);
    toolbar->setMaximumHeight(24);

    toolbar->addAction(newAct);
    toolbar->addAction(openAct);
    toolbar->addAction(saveAct);
    toolbar->addSeparator();
    toolbar->addAction(copyAct);
    toolbar->addAction(cutAct);
    toolbar->addAction(pasteAct);
    toolbar->addAction(delAct);
    toolbar->addAction(redoAct);
    toolbar->addAction(undoAct);
    toolbar->addAction(findAct);

    toolbar->addSeparator();
    //toolbar->addAction(resultAct);
    toolbar->addWidget(cbDB);
    toolbar->addAction(checkAct);
    toolbar->addAction(runAct);
    toolbar->addAction(stopAct);
}

void QueryWidget::update_combobox()
{
    cbDB->clear();
    QStringList conlist = QSqlDatabase::connectionNames();
    foreach(QString con,conlist){
        QSqlDatabase db = QSqlDatabase::database(con);
        QFileInfo fi(db.databaseName());
        cbDB->addItem(fi.baseName(),QVariant(con));
    }
    cbDB->setCurrentIndex(0);
}

void QueryWidget::tabCloseRequested(int index)
{
    resultTab->removeTab(index);
    if(resultTab->count()==0){
        resultTab->setVisible(false);
    }
}

void QueryWidget::selectionChanged()
{
    bool selected = queryEdit->textCursor().hasSelection();
    copyAct->setEnabled(selected);
    cutAct->setEnabled(selected);
    delAct->setEnabled(selected);
}

void QueryWidget::clipboardChanged()
{
    pasteAct->setEnabled(queryEdit->canPaste());
}

void QueryWidget::documentModified()
{
    bool modified = queryEdit->document()->isModified();
    setWindowModified(modified);
    saveAct->setEnabled(modified);
}

void QueryWidget::create()
{
    bool isExist;
    QueryWidget *widget = createObject(QString(),isExist);
    stackedParent->addWidget(widget);
}

void QueryWidget::open(){
    QString file_name = QFileDialog::getOpenFileName(
                             this,
                             QString("打开查询文件"),
                             QString("."),
                             QString("Sqlite查询文件(*.sql)"));

    if (file_name.isEmpty())
        return ;

    bool isExist;
    QueryWidget *widget = createObject(file_name,isExist);
    if(isExist){
        stackedParent->showWidget(widget);
    }else
        stackedParent->addWidget(widget);
}

bool QueryWidget::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(fileName);
    }
}
void QueryWidget::remove()
{
    queryEdit->textCursor().removeSelectedText();
}

void QueryWidget::find()
{
    if(-1==resultTab->indexOf(findView)){
        resultTab->addTab(findView,QString("查找"));
    }
    if(resultTab->isHidden()){
        resultTab->show();
    }
    resultTab->setCurrentWidget(findView);
}
void QueryWidget::inGrid()
{
    isInGrid = true;
    txtAct->setChecked(false);
    resultTab->clear();
    //resultTab->addTab(resultSplitter,QString("网格"));
    resultTab->addTab(resultEdit,QString("消息"));
}

void QueryWidget::inTxt()
{
    isInGrid = false;
    tblAct->setChecked(false);
    resultTab->clear();
    resultTab->addTab(resultEdit,QString("结果"));
}

void QueryWidget::selectDB(int index)
{
    conname = cbDB->itemData(index).toString();
}

void QueryWidget::check()
{

}

void QueryWidget::execQuery()
{
    query = new QSqlQuery(QSqlDatabase::database(conname));
    QString result;

    QString runText;
    if(queryEdit->textCursor().hasSelection()){
        runText = queryEdit->textCursor().selectedText();
    }
    else
    {
        runText = queryEdit->toPlainText();
    }
    QStringList queryList = runText.split(';');
    QString str;

    foreach(str,queryList)
    {
        if(str.isEmpty())continue;
        if(query->exec(str))
        {
            if(query->isSelect())
            {
                int colNum=0;
                int rowNum=0;

                QString record;
                while(query->next())
                {
                    rowNum ++;
                    colNum = query->record().count();
                    record.append(QString("%1\t").arg(rowNum));
                    for(int i=0;i<colNum;i++)
                    {
                        record.append(QString("%1\t").arg(query->value(i).toString()));
                    }
                    record.append("\n");
                }

                QString header;
                if(rowNum>0){
                    header.append(QString("行\t"));
                    query->first();
                    colNum = query->record().count();
                    for(int i=0;i<colNum;++i)
                    {
                        header.append(QString("%1\t").arg(query->record().fieldName(i)));
                    }
                    header.append("\n");
                }

                result.append(header);
                result.append(record);
                result.append(QString("共查询 %1 行\n").arg(rowNum));
            }
            else
            {
                main->refresh(conname);
                result.append(QString("共影响 %1 行\n").arg(query->numRowsAffected()));
            }
        }
        else
        {
            //QSqlError error = query->lastError();
            result.append(query->lastError().text());
            result.append('\n');
            //resultEdit->setText(query->lastError().text());
        }
    }
    resultEdit->setText(result);
    if(-1==resultTab->indexOf(resultEdit)){
        resultTab->insertTab(0,resultEdit,QString("结果"));
    }
    if(resultTab->isHidden()){
        resultTab->show();
    }
    resultTab->setCurrentWidget(resultEdit);
}

void QueryWidget::stopQuery()
{
    if(0!=query)
    {
        if(query->isActive())
        {
            query->clear();
        }
    }
}

//void QueryWidget::newFile()
//{
//    static int sequenceNumber = 1;
//    isUntitled = true;

//    fileName = QString("查询%1.sql").arg(sequenceNumber++);
//    setWindowTitle(fileName+"[*]");
//    connect(queryEdit->document(), SIGNAL(contentsChanged()),
//            this, SLOT(documentModified()));
//}

bool QueryWidget::loadFile(const QString &file_name)
{
    QFile file(file_name);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, QString("查询"),
                             QString("不能读取文件 %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    queryEdit->setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    fileName = file_name;
    isUntitled = false;
    queryEdit->document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(fileName + "[*]");

    connect(queryEdit->document(), SIGNAL(contentsChanged()),
            this, SLOT(documentModified()));

    return true;
}
bool QueryWidget::saveAs()
{
    QString file_name = QFileDialog::getSaveFileName(this, QString("另存为."),
                                    fileName,
                                    QString("SQL查询(*.sql)"));
    if (file_name.isEmpty())
        return false;

    if(saveFile(file_name))
    {
        fileName = file_name;
        stackedParent->setWidgetTitle(this,fileName);
        isUntitled = false;
        return true;
    }
    else
        return false;
}

bool QueryWidget::saveFile(const QString &file_name)
{
    QFile file(file_name);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, QString("保存文件错误"),
                             QString("不能保存文件 %1:\n%2.")
                             .arg(file_name)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << queryEdit->toPlainText();
    QApplication::restoreOverrideCursor();

    queryEdit->document()->setModified(false);

    return true;
}

bool QueryWidget::hasSelection()
{
    return queryEdit->textCursor().hasSelection();
}

void QueryWidget::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

bool QueryWidget::maybeSave()
{
    if (queryEdit->document()->isModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, QString("警告"),
                     QString("'%1' 已经更改.是否保存？").arg(QFileInfo(fileName).baseName()),
                     QMessageBox::Save | QMessageBox::Discard
                     | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}
