#ifndef SHOWTABLEWIDGET_H
#define SHOWTABLEWIDGET_H

#include <QWidget>

class QTableView;
class QSqlTableModel;
class QMenu;
class QToolBar;

class ShowTableObjects;
class QModelIndex;

class ShowTableWidget:public QWidget
{
    Q_OBJECT
public:
    static ShowTableWidget* createObject(const QString &table_name,const QString &con,bool &isExist);
    static void deleteObject(const QString &table_name,const QString &con);
    static void initObjContainer();
    static void delObjContainer();
    static QList<QWidget*>* dbTables(const QString &con);
    static QList<ShowTableWidget*> allTables();

private slots:
    void showContextMenu(QPoint p);
    void appendRow();
    void insertRow();
    void removeSelectedRows();
    void submit();

    void dataChanged(const QModelIndex &topLeft,const QModelIndex &bottomRight);
    void rowsInserted(const QModelIndex &parent, int start, int end);
    void rowsRemoved(const QModelIndex &parent, int start, int end);
protected:
    void closeEvent(QCloseEvent *event);
private:
    ShowTableWidget();

    void init(const QString &table_name,const QString &con);
    void init_model();
    void init_table();
    //void init_actions();
    void init_menu();
    void init_toolbar();
    bool maybeSave();
    bool isDirty;

    QTableView *view;
    QMenu *contextMenu;
    QSqlTableModel *model;

    QString table;
    QString dbname;
    QString con;

    QToolBar *toolbar;
    QAction *removeAct;
    QAction *insertAct;
    QAction *appendAct;
    QAction *submitAct;
    QAction *cancelAct;

    static ShowTableObjects *objs;
};

#endif // SHOWTABLEWIDGET_H
