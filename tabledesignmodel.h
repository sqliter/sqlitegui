#ifndef TABLEDESIGNMODEL_H
#define TABLEDESIGNMODEL_H

#include <QStringList>
#include <QAbstractTableModel>
class SqliteTable;
class SqliteColumn;
class SqlitePKey;
class SqliteUnique;
class QSqlError;

enum ColumnState
{
    ColInvalid,
    ColValid,
    ColAdded,
    ColRemoved,
    ColModified
};

class TableDesignModel:public QAbstractTableModel
{
    Q_OBJECT
public:
    TableDesignModel(QObject *parent=0);

    void createTable(QString tbname,QString constr);
    void openTable(QString tbname,QString constr,int &err);
    bool saveTable(QString &error);
    void setTableName(QString tbname);
    bool isDirty();

    SqliteColumn* column(int row);
    int colwidth(int col);

    void pk(QList<int> &rows);
    const QString& pkconflict();
    void setPk(QList<int> rows);
    void cancelPk(QList<int> rows);

    const QString& uqconflict();
    void setUnique(const QList<int> &rows,const QList<int> &old_rows);
    void cancelUnique(const QList<int> &rows);

    const QString& check();
    void setCheck(const QString &chk_exp);

    int rowCount(const QModelIndex &parent= QModelIndex()) const;
    int columnCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());
signals:
    void dirtyChanged(bool is_dirty);
private:
    void format_sql_error(QSqlError *se,QString &err);
    void resize(int count);

    QString create_sql();
    QString add_col_sql(SqliteColumn *col,int *err);

    QString con;
    QString sql_src;
    SqliteTable *table;

    QList<SqliteColumn*> datas;
    QString table_name;
    SqlitePKey *pkey;
    SqliteUnique *uq;
    QString check_expr;

    QList<int> colwidths;

    bool dirty;
    QStringList headernames;
};

#endif // TABLEDESIGNMODEL_H
