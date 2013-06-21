#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QTreeWidget;
class QTreeWidgetItem;
class QSplitter;
class StackedWidget;

class MainWindow:public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();

    void addTable(const QString &constr,const QString &tbname);
    void refresh(const QString &constr);

    StackedWidget* mdiArea();
public slots:
    void refresh();
signals:
    void connectionChanged();
private slots:
    void treeContextMenu(QPoint p);
    void treeItemChanged(QTreeWidgetItem *item,int column);
    void treeItemDoubleClicked(QTreeWidgetItem *item ,int column);

    void createDB();
    void openDB();
    void closeDB();
    void closeAllDB();

    void query();

    void remove();
    void rename();

    void createTable();
    void createView();
    void createIndex();
    void createTrigger();

    void editTable();
    void renameTable();
    void removeTable();

    void editView();
    void editIndex();
    void editTrigger();

    void showTable();
    void showView();

    void about();
private:
    void init();
    void init_actions();
    void init_menu();
    void init_toolbar();
    void init_tree();

    void refresh(QTreeWidgetItem *dbitem);
    void remove_view();
    void remove_index();
    void load_table_indexs(QTreeWidgetItem *tbitem,
                           const QString &con_name);
    void load_trigger(QTreeWidgetItem *dbitem);

    QTreeWidget *treeDB;
    QList<QTreeWidgetItem*> toplevels;
    QSplitter *splitterMain;
    StackedWidget *stackedMain;

    QMenu *menuDB;
    QMenu *menuHelp;
    QMenu *menuTreeContext;

    QAction *saveAct;
    QAction *removeAct;
    QAction *renameAct;
    QAction *refreshAct;
    QAction *exitAct;
    QAction *aboutAct;

    QAction *createDBAct;
    QAction *openDBAct;
    QAction *closeDBAct;
    QAction *closeAllDBAct;
    QAction *queryAct;

    QAction *createTableAct;
    QAction *createViewAct;
    QAction *createIndexAct;
    QAction *createTriggerAct;

    QAction *showTableAct;
    QAction *alterTableAct;

    QAction *showViewAct;
    QAction *alterViewAct;

    QAction *alterIndexAct;

    QAction *alterTriggerAct;

    QString old_name;

};

#endif // MAINWINDOW_H
