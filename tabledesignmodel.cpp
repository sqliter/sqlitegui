#include <QtSql>
#include "sqlparse.h"
#include "tabledesignmodel.h"

TableDesignModel::TableDesignModel(QObject *parent)
    :QAbstractTableModel(parent){

    dirty = false;
    emit(dirtyChanged(dirty));
    table = 0;
    pkey = new SqlitePKey;
    uq = new SqliteUnique;

    headernames<<QString("");
    headernames<<QString("列名");
    headernames<<QString("类型");
    headernames<<QString("长度");
    headernames<<QString("允许空");
    //headernames<<QString("主键");
    headernames<<QString("唯一性");
    headernames<<QString("默认值");
    headernames<<QString("Check");

    colwidths<<18<<100<<100<<100<<50<<50<<100<<100;

    int row_count = 10;
    for(int i=0;i<row_count;i++){
        SqliteColumn *col = new SqliteColumn;
        col->notnull = false;
        col->state = ColInvalid;
        datas.push_back(col);
    }
}

bool TableDesignModel::isDirty()
{
    return dirty;
}

void TableDesignModel::createTable(QString tbname, QString constr){
    table_name = tbname;
    con = constr;
}

void TableDesignModel::openTable(QString tbname, QString constr,int &err)
{
    err = 0;
    table_name = tbname;
    con = constr;

    QSqlDatabase db = QSqlDatabase::database(constr);
    QString sqlstr=QString("select sql from sqlite_master where type='table' and name='%1'")
            .arg(tbname);
    QSqlQuery query(db);
    query.exec(sqlstr);

    query.first();
    sql_src = query.value(0).toString();
    if(sql_src.isEmpty())
    {
        err = 2;
        return;
    }

    SqlParse parser;
    if(!parser.parse(sql_src))
    {
        err = 1;
        return;
    }

    table = new SqliteTable;
    table->name = parser.table.name;

    //复制解析出来的列
    int row=0;
    SqliteColumn *datascol=0;
    SqliteColumn *tbcol=0;

    int count = parser.table.column_list.count();

    resize(count);

    foreach(SqliteColumn *col,parser.table.column_list){
        //复制到datas
        datascol = datas[row];
        *datascol = *col;
        datascol->notnull = !col->notnull;
        datascol->state = ColValid;

        row ++;

        //复制到table
        tbcol = new SqliteColumn;
        *tbcol = *col;
        datascol->notnull = !col->notnull;
        tbcol->state = ColValid;

        table->column_list.push_back(tbcol);
    }

    //复制解析出来的主键
    table->pkey.conflict = parser.table.pkey.conflict;
    this->pkey->conflict = table->pkey.conflict;

    foreach(QString str,parser.table.pkey.column_list){
        table->pkey.column_list<<str;
        pkey->column_list<<str;
    }

    //复制解析出来的唯一性约束
    table->uq.conflict = parser.table.uq.conflict;
    this->uq->conflict = table->uq.conflict;

    foreach(QString str,parser.table.uq.column_list){
        table->uq.column_list<<str;
        uq->column_list<<str;
    }
    //复制Check约束
    table->check_expr = parser.table.check_expr;
    this->check_expr = table->check_expr;
}

bool TableDesignModel::saveTable(QString &error){
    QSqlDatabase db = QSqlDatabase::database(con);
    db.transaction();
    if(table==0){
        //新建表
        QString strsql = create_sql();
        QSqlQuery q(db);
        bool r = q.exec(strsql);
        if(!r){
            QSqlError se=q.lastError();
            format_sql_error(&se,error);
            db.rollback();
            return r;
        }
    }else{
        //修改表
        foreach(SqliteColumn *col,datas){
            if(col->state == ColAdded){
                //构造添加列字符串，添加列
                int err=0;
                QString strsql = add_col_sql(col,&err);
                if(err==0){
                    QSqlQuery q(db);
                    bool r = q.exec(strsql);
                    if(!r){
                        QSqlError se=q.lastError();
                        format_sql_error(&se,error);
                        db.rollback();
                        return r;
                    }
                }else{
                    error = QString("字段定义了非空约束，必须有一个默认值");
                    db.rollback();
                    return false;
                }
            }
        }
    }
    bool ret = db.commit();
    if(ret){
        foreach(SqliteColumn *col,datas){
            col->state = ColValid;
        }

        dirty = false;
        emit(dirtyChanged(dirty));
    }else{
        QSqlError se = db.lastError();
        format_sql_error(&(se),error);
    }
    return ret;
}

QString TableDesignModel::create_sql()
{
    QString strsql = QString("create table %1(").arg(table_name);

    int count = datas.count();
    for(int i=0;i<count;i++){
        SqliteColumn *col = datas.value(i);
        if(col->name.isEmpty())continue;
        strsql.append(col->name);
        if(!col->type.isEmpty()){
            strsql.append(" ");
            strsql.append(col->type);

            if(col->length>0){
                strsql.append("(");
                strsql.append(QString("%1").arg(col->length));
                if(col->length2>0){
                    strsql.append(",");
                    strsql.append(QString("%1").arg(col->length2));
                }
                strsql.append(")");
            }
        }

        if(col->isPK){
            if(pkey->column_list.count()>1){
                //pk_rows.append(i);
            }else{
                strsql.append(QString(" primary key"));
                if(!col->pksort_order.isEmpty()
                        && col->pksort_order!=QString("")){
                    strsql.append(QString(" %1").arg(col->pksort_order));
                }
                if(!col->pkconflict.isEmpty()
                        && col->pkconflict!=QString("")){
                    strsql.append(QString(" on conflict %1").arg(col->pkconflict));
                }
                if(col->auto_inc){
                    strsql.append(" autoincrement");
                }
            }
        }
        if(!col->notnull){
            strsql.append(QString(" not null"));
            if(!col->notnullconflict.isEmpty()){
                strsql.append(QString(" on conflict %1").arg(col->notnullconflict));
            }
        }
        if(!col->defaultValue.isEmpty()){
            strsql.append(QString(" default %1").arg(col->defaultValue));
        }
        if(!col->check.isEmpty()){
            strsql.append(QString(" check(%1)").arg(col->check));
        }
        if(!col->conlate.isEmpty()){
            strsql.append(QString(" collate %1").arg(col->conlate));
        }
        if(col->isUnique){
            if(uq->column_list.count()<=1){
                strsql.append(QString(" unique"));
                if(!col->unique_conflict.isEmpty()){
                    strsql.append(QString(" on conflict %1")
                                  .arg(col->unique_conflict));
                }
            }
        }
        strsql.append(QChar(','));
    }

    if(pkey->column_list.count()>1)
    {
        strsql.append(QString(" primary key ("));
        count = pkey->column_list.count();
        for(int i=0;i<count-1;i++){
            strsql.append(QString("%1,").arg(pkey->column_list.at(i)));
        }
        strsql.append(QString("%1)").arg(pkey->column_list.at(count-1)));
        if(!pkey->conflict.isEmpty()){
            strsql.append(QString(" on conflict %1").arg(pkey->conflict));
        }
        strsql.append(QChar(','));
    }
    if(uq->column_list.count()>1)
    {
        strsql.append(QString(" unique ("));
        count = uq->column_list.count();
        for(int i=0;i<count-1;i++){
            strsql.append(QString("%1,").arg(uq->column_list.at(i)));
        }
        strsql.append(QString("%1)").arg(uq->column_list.at(count-1)));
        if(!uq->conflict.isEmpty()){
            strsql.append(QString(" on conflict %1").arg(uq->conflict));
        }
        strsql.append(QChar(','));
    }

    if(!check_expr.isEmpty()){
        strsql.append(QString(" check(%1),").arg(check_expr));
    }

    strsql = strsql.left(strsql.length()-1);

    strsql.append(QChar(')'));
    qDebug()<<strsql;
    return strsql;
}

QString TableDesignModel::add_col_sql(SqliteColumn *col,int *err)
{
    if(col->name.isEmpty())
        return QString();

    QString strsql = QString("alter table %1 add column ").arg(table_name);

    strsql.append(col->name);

    if(!col->type.isEmpty()){
        strsql.append(QString(" %1").arg(col->type));

        if(col->length>0){
            strsql.append("(");
            strsql.append(QString("%1").arg(col->length));
            if(col->length2>0){
                strsql.append(",");
                strsql.append(QString("%1").arg(col->length2));
            }
            strsql.append(")");
        }
    }

    if(!col->notnull){
        strsql.append(QString(" not null"));
        if(!col->notnullconflict.isEmpty()){
            strsql.append(QString(" %1").arg(col->notnullconflict));
        }

        if(col->defaultValue.isEmpty()){
            *err = 1;
            return QString();
        }

    }

    if(!col->defaultValue.isEmpty()){
        strsql.append(QString(" default %1").arg(col->defaultValue));
    }

    if(!col->check.isEmpty()){
        strsql.append(QString(" check(%1)").arg(col->check));
    }
    if(!col->conlate.isEmpty()){
        strsql.append(QString(" collate %1").arg(col->conlate));
    }
    qDebug()<<strsql;
    return strsql;
}

void TableDesignModel::setTableName(QString tbname)
{
    table_name = tbname;
}

int TableDesignModel::colwidth(int col){
    return colwidths.at(col);
}

SqliteColumn* TableDesignModel::column(int row)
{
    return datas.value(row);
}
const QString& TableDesignModel::pkconflict()
{
    return pkey->conflict;
}

void TableDesignModel::pk(QList<int> &rows){
    int count = datas.count();
    for(int i=0;i<count;i++){
        if(datas.value(i)->isPK){
            rows.append(i);
        }
    }
}

void TableDesignModel::setPk(QList<int> rows)
{
    for(int i=0;i<datas.count();++i){
        datas.value(i)->isPK = false;
    }
    pkey->column_list.clear();
    pkey->conflict.clear();

    int count = rows.count();
    for(int i=0;i<count;++i){
        datas.value(rows[i])->isPK = true;
        if(count>1){
            QString colname = datas.value(rows[i])->name;
            pkey->column_list.append(colname);
        }
    }
    dirty = true;
    emit(dirtyChanged(dirty));
}

void TableDesignModel::cancelPk(QList<int> rows)
{
    int count = rows.count();
    for(int i=0;i<count;++i){
        datas.value(rows[i])->isPK = false;
    }
    if(count>1){
        pkey->column_list.clear();
        pkey->conflict.clear();
    }
    dirty = true;
    emit(dirtyChanged(dirty));
}

const QString& TableDesignModel::uqconflict()
{
    return uq->conflict;
}

void TableDesignModel::setUnique(const QList<int> &rows,const QList<int> &old_rows)
{
    int count = old_rows.count();
    for(int i=0;i<count;++i){
        datas.value(old_rows[i])->isUnique = false;
    }
    if(count>1){
        uq->column_list.clear();
        uq->conflict.clear();
    }

    count = rows.count();
    for(int i=0;i<count;++i){
        datas.value(rows[i])->isUnique = true;
        if(count>1){
            QString colname = datas.value(rows[i])->name;
            uq->column_list.append(colname);
        }
    }
    dirty = true;
    emit(dirtyChanged(dirty));
}

void TableDesignModel::cancelUnique(const QList<int> &rows)
{
    int count = rows.count();
    for(int i=0;i<count;++i){
        datas.value(rows[i])->isUnique = false;
    }
    if(count>1){
        uq->column_list.clear();
        uq->conflict.clear();
    }
    dirty = true;
    emit(dirtyChanged(dirty));
}

const QString& TableDesignModel::check()
{
    return check_expr;
}

void TableDesignModel::setCheck(const QString &chk_exp)
{
    this->check_expr = chk_exp;
    dirty = true;
    emit(dirtyChanged(dirty));
}

int TableDesignModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return datas.size();
}

int TableDesignModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return headernames.size();
}

QVariant TableDesignModel::data(const QModelIndex &index, int role) const
{
    if(index.isValid()){
        int col = index.column();
        int row = index.row();
        if(col == 0){
            return QVariant(datas.value(row)->isPK);
        }
        else if(col == 1){
            return QVariant(datas.at(row)->name);
        }else if(col==2){
            return QVariant(datas.at(row)->type);
        }else if(col==3){
            QString len;
            if(datas.at(row)->length>0){
                len.append(QString("%1").arg(datas.at(row)->length));
                if(datas.at(row)->length2>0){
                    len.append(QString(",%1").arg(
                                   datas.at(row)->length2));
                }
            }
            return QVariant(len);
        }else if(col == 4){
            return QVariant(datas.at(row)->notnull);
        }else if(col == 5){
            return QVariant(datas.at(row)->isUnique);
        }else if(col == 6){
            return QVariant(datas.at(row)->defaultValue);
        }else if(col == 7){
            return QVariant(datas.at(row)->check);
        }
        else
            return QVariant();
    }else{
        return QVariant();
    }
}

QVariant TableDesignModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        return headernames.at(section);
    }

    return QVariant();
}

bool TableDesignModel::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; row++) {
        SqliteColumn *col = new SqliteColumn;
        col->state = ColInvalid;
        datas.insert(position,col);
    }

    endInsertRows();
    dirty = true;
    emit(dirtyChanged(dirty));
    return true;
}

bool TableDesignModel::removeRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    bool chan=false;
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; ++row) {
        SqliteColumn *column = datas.at(position);
        if(column->state != ColValid){
            datas.removeAt(position);
            delete column;
            chan = true;
        }
    }

    endRemoveRows();
    if(chan){
        dirty = true;
        emit(dirtyChanged(dirty));
    }
    return chan;
}

bool TableDesignModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
     //if (index.isValid() && role == Qt::EditRole) {
    if(index.isValid()){
        int row = index.row();
        int col = index.column();

        SqliteColumn *column = datas.at(row);

        if(column->state != ColValid){
            if(col == 0){
                column->isPK = value.toBool();
            }else if(col==1){
                if(column->state == ColInvalid){
                    column->state = ColAdded;
                }
                column->name = value.toString();
            }else if(col==2){
                column->type = value.toString();
            }else if(col==3){
                QString len = value.toString();
                QStringList lens= len.split(QChar(','));
                column->length = lens.at(0).toInt();
                if(lens.size()>1){
                    column->length2 = lens.at(1).toInt();
                }
            }
            else if(col==4)
                column->notnull = value.toBool();
            else if(col == 5)
                column->isUnique = value.toBool();
            else if(col == 6){
                column->defaultValue = value.toString();
            }
            else if(col == 7)
                column->check = value.toString();

            emit(dataChanged(index, index));

            dirty = true;
            emit(dirtyChanged(dirty));
        }


//        if(column->state==ColValid)
//            column->state = ColModified;


        return true;
    }else{
        return false;
    }
}

Qt::ItemFlags TableDesignModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    int row = index.row();
    int col = index.column();

    SqliteColumn *column = datas.at(row);

    if(column->state != ColValid){
        if(col==4||col==5)
            return QAbstractTableModel::flags(index);
        else
            return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
    }else
        return QAbstractTableModel::flags(index);
}

void TableDesignModel::format_sql_error(QSqlError *se,QString &err)
{
    err = QString("database:%1\ndriver:%2\nerror number:%3\nerror type:%4\nerror text:%5")
            .arg(se->databaseText())
            .arg(se->driverText())
            .arg(se->number())
            .arg(se->type()).arg(se->text());
}

void TableDesignModel::resize(int count)
{
    SqliteColumn *datascol=0;
    int datas_count = datas.count();
    while(count>=datas_count){
        int added_count = 10;
        for(int i=0;i<added_count;i++){
            datascol = new SqliteColumn;
            datascol->notnull = false;
            datascol->state = ColInvalid;
            datas.push_back(datascol);
        }
        datas_count += added_count;
    }
}
