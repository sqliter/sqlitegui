#ifndef QUERYWIDGET_H
#define QUERYWIDGET_H

#include <QWidget>

//#include "highlighter.h"

class QSqlQuery;

class QTextEdit;
class QTabWidget;
class QTableView;
class Highlighter;
class QSplitter;
class QToolBar;
class QComboBox;
class QueryObjects;
class StackedWidget;
class MainWindow;

class QueryWidget:public QWidget
{
    Q_OBJECT
public:
    static QueryWidget* createObject(const QString &file_name,bool &isExist);
    static void deleteObject(const QString &file_name);
    static void initObjContainer();
    static void delObjContainer();
protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void selectionChanged();
    void documentModified();
    void clipboardChanged();
    void update_combobox();
    void tabCloseRequested(int index);

    void create();
    void open();
    bool save();
    bool saveAs();

    void remove();
    void find();

    void inGrid();
    void inTxt();
    void selectDB(int index);
    void check();
    void execQuery();
    void stopQuery();

private:
    QueryWidget();

    //void newFile();
    bool loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);

    void init_combobox();
    void init_toolbar();
    void init_editor();
    void init_splitter();
    void init(const QString &file_name,bool flag);

    bool hasSelection();

    bool maybeSave();

    void updateActions();

    QString fileName;
    bool isUntitled;

    QComboBox *cbDB;
    QToolBar *toolbar;
    QSplitter *splitter;
    QTextEdit *queryEdit;    
    QTextEdit *resultEdit;
    QTabWidget *resultTab;
    QWidget *findView;

    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;

    QAction *redoAct;
    QAction *undoAct;
    QAction *copyAct;
    QAction *cutAct;
    QAction *pasteAct;
    QAction *delAct;
    QAction *findAct;
    QAction *selectAllAct;

    QAction *resultAct;
    QAction *tblAct;
    QAction *txtAct;

    QAction *runAct;
    QAction *stopAct;
    QAction *checkAct;

    bool isInGrid;

    QSqlQuery *query;
    QString conname;


    Highlighter *highlighter;
    MainWindow *main;
    StackedWidget *stackedParent;

    static QueryObjects *objs;
};
#endif // QUERYWIDGET_H
