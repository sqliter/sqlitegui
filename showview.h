#ifndef SHOWVIEW_H
#define SHOWVIEW_H

#include <QTableView>

class QSqlQueryModel;

class ShowViewObjects;
class QModelIndex;

class ShowViewWidget:public QTableView
{
    Q_OBJECT
public:
    static ShowViewWidget* createObject(const QString &view_name,
                                        const QString &con,bool &isExist);
    static void deleteObject(const QString &view_name,
                             const QString &con);
    static void initObjContainer();
    static void delObjContainer();
    static QList<QWidget*> dbViews(const QString &con);
    static QList<ShowViewWidget*> allTables();
protected:
    void closeEvent(QCloseEvent *event);
private:
    ShowViewWidget();

    void init(const QString &table_name,const QString &con);
    void init_model();
    void init_table();

    QSqlQueryModel *model;

    QString view;
    QString dbname;
    QString con;

    static ShowViewObjects *objs;
};
#endif // SHOWVIEW_H
