#ifndef TABLEDESIGNWIDGET_H
#define TABLEDESIGNWIDGET_H

#include <QWidget>
#include <QHeaderView>

class QMenu;
class QTableWidget;
class QTableView;
class QTabWidget;
class QLineEdit;
class QCheckBox;
class QComboBox;
class QSplitter;
class QPushButton;
class QToolBar;
class TableDesignModel;
class TableDesignObjects;
class SqliteColumn;

#include "stackedpage.h"

class TableDesignWidget:public QWidget
{
    Q_OBJECT
public:
    static TableDesignWidget* createObject(const QString &table_name,const QString &con,bool &isExist);
    static void deleteObject(const QString &table_name,const QString &con);
    static void initObjContainer();
    static void delObjContainer();
    static QList<QWidget*>* dbTables(const QString &con);
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void currentRowChanged(const QModelIndex &current,
                           const QModelIndex &previous);
    void selectionChanged(const QItemSelection &selected,
                          const QItemSelection &deselected);
    void dataChanged(const QModelIndex &topLeft,
                     const QModelIndex &bottomRight);
    void itemClicked(const QModelIndex &index);
    void showContextMenu(QPoint p);
    void appendRow();
    void insertRow();
    void removeSelectedRows();
    void save();
    void setPK();
    void setUnique();
    void setCheck();

private:
    TableDesignWidget();

    void init(const QString &table_name,const QString &con,bool create_flag);
    void init_model();
    void init_menu();
    void init_toolbar();
    void init_fields_table();
    void init_field_widget();

    bool maybeSave();

    void setComboText(QComboBox *cb,const QString &text);
    void updateWidgetEnables(const QModelIndex &current);

    QSplitter *main;

    QMenu *contextMenu;

    QToolBar *toolbar;

    QTableView *tbFields;
    //QTableWidget *tbFields;
    TableDesignModel *model;
    QString table_define_sql;

    QTabWidget *tabField;
    QWidget *field_widget;

    QCheckBox *chkAutoInc;
    QComboBox *cbPkConflict;
    QComboBox *cbNotNullConflict;
    QComboBox *cbUniqueConflict;
    QLineEdit *txtCollate;
    QLineEdit *txtPkSortOrder;

    QString tbname;
    QString dbname;
    QString con;

    QAction *saveAct;
    QAction *pkAct;
    QAction *uniqueAct;
    QAction *checkAct;
    QAction *removeAct;
    QAction *insertAct;
    QAction *appendAct;

    bool create_flag;
    bool save_ok;

    QList<int> selected_rows;
    QList<int> pk_rows;
    QList<int> uq_rows;

    bool pk_same;
    bool uq_same;

    SqliteColumn *current_column;

    static TableDesignObjects *objs;
};

#endif // TABLEDESIGNWIDGET_H
