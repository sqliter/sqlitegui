#ifndef SQLPARSE_H
#define SQLPARSE_H

#include <QString>
#include <QVector>
#include <QMap>

class SqlScanner;

enum SqliteConflict
{
    Undefine,
    Rollback,
    Abort,
    Fail,
    Ignore,
    Replace
};

struct SqliteNotNullConstraint
{
    bool value;
    SqliteConflict conflict;
};

struct SqliteColumn
{
    SqliteColumn();

    SqliteColumn& operator=(const SqliteColumn &other);

    QString name;

    QString type;
    int length;
    int length2;

    bool notnull;
    QString notnullconflict;

    bool isPK;
    bool auto_inc;
    QString pksort_order;
    QString pkconflict;

    bool isUnique;
    QString unique_conflict;

    QString defaultValue;

    QString check;

    QString conlate;

    int state;
};

struct SqlitePKey{
    QVector<QString> column_list;
    QString conflict;
};
struct SqliteUnique{
    QVector<QString> column_list;
    QString conflict;
};

struct SqliteIndexColumn
{
    QString name;
    QString collate;
    QString sort;
};

struct SqliteIndex{
    QString name;
    QString tbname;
    bool unique;
    QList<SqliteIndexColumn*> collist;
};

struct SqliteTable
{
    QString name;
    QVector<SqliteColumn*> column_list;
    SqlitePKey pkey;
    SqliteUnique uq;
    QString check_expr;

};

enum ParseState
{
  Prepair,
  ParseColumnList,
  ParsePKey,
  ParsePKeyCol,
  ParsePKeyColEnd,
  ParseUnique,
  ParseUniqueCol,
  ParseUniqueColEnd,
  ParseCheck,
  ParseCheckEnd,
  ParseEnd
};

class SqlParse
{
public:

    bool parse(const QString &sql);

    bool parse_index(const QString &src,
                     const QString &dbname,
                     SqliteIndex &index);

    SqliteTable table;
private:
    void reset();
    bool parse_table(SqlScanner *scanner);
    bool parse_column(SqlScanner *scanner,SqliteColumn *col);
    bool parse_conflict(SqlScanner *scanner,QString &conflict);
    //SqliteColumn* findBy(const QString &col_name);

    ParseState state;
};

#endif // SQLPARSE_H
