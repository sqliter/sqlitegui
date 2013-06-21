#ifndef TRIGGERWIDGET_H
#define TRIGGERWIDGET_H

#include <QDialog>

class QLineEdit;
class QComboBox;
class QTextEdit;

class TriggerDialog:public QDialog
{
    Q_OBJECT
public:
    TriggerDialog();

    void init(const QString &con_name,
              const QString &table_name,
              const QString &trigger_name);
    void loadTrigger(const QString &sql);
private slots:
    void create();
    bool check();
private:

    void init_combobox();
    void init_editor();

    QLineEdit *txtName;
    QComboBox *cbTable;
    QTextEdit *sqlEdit;

    QPushButton *btnOk;
    QPushButton *btnCancel;

    QString conname;
    QString dbname;
    QString tbname;
    QString triggername;

    bool create_flag;
};

#endif // TRIGGERWIDGET_H
