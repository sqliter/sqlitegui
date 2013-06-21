#ifndef INDEXWIDGET_H
#define INDEXWIDGET_H

#include <QDialog>

class QLineEdit;
class QCheckBox;
class QComboBox;
class QTableWidget;
class QPushButton;
class SqliteIndex;

class IndexDialog:public QDialog
{
    Q_OBJECT
public:
    IndexDialog();
    void init(const QString &con_name,
              const QString &table_name,
              const QString &index_name);
    void loadIndex(const QStringList &sl);
private slots:
    void create();
    void tableChanged(const QString &tb);
    void cellClicked(int row,int column);
private:

    void init_combobox();
    void init_table();

    QLineEdit *txtName;
    QCheckBox *ckUnique;
    QComboBox *cbTable;
    QTableWidget *tbColumnList;
    QPushButton *btnOk;
    QPushButton *btnCancel;

    QString conname;
    QString dbname;
    QString tbname;
    QString indexname;

    bool create_flag;
};

#endif // INDEXWIDGET_H
