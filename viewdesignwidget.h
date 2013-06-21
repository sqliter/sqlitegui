#ifndef VIEWDESIGNWIDGET_H
#define VIEWDESIGNWIDGET_H

#include <QWidget>

class QPushButton;
class QLineEdit;
class QCheckBox;
class QTextEdit;
class Highlighter;

class ViewDesignObjects;

class ViewDesignWidget:public QWidget
{
    Q_OBJECT
public:
    static ViewDesignWidget* createObject(const QString &view_name,
                                          const QString &con_name,
                                          bool &isExist);
    static void deleteObject(const QString &view_name,
                             const QString &con_name);
    static void initObjContainer();
    static void delObjContainer();
protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void run();

private:
    ViewDesignWidget();

    void init(const QString &view_name,
              const QString &con_name,
              bool flag);

    bool getNameFromSql(const QString &sql,QString &name);

    QLineEdit   *txtName;
    QCheckBox   *cbTemp;

    QPushButton *btnCreate;

    QTextEdit *sqlEdit;
    Highlighter *highlighter;

    bool create_flag;
    QString dbname;
    QString viewname;
    QString conname;

    static ViewDesignObjects *objs;
};
#endif // VIEWDESIGNWIDGET_H
