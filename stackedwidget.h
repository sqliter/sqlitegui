#ifndef STACKEDWIDGET_H
#define STACKEDWIDGET_H

#include <QFrame>

class QStackedWidget;
class QComboBox;
class QHBoxLayout;
class QToolBar;

class StackedWidget:public QFrame
{
    Q_OBJECT
public:
    StackedWidget();

    void addWidget(QWidget *widget);
    bool removeWidget(QWidget *widget);
    void showWidget(QWidget *widget);
    void removeAll();

    void setWidgetTitle(QWidget *widget,const QString &title);

private slots:
    void back();
    void forward();
    bool closepage();
    void current_changed(int index);
private:
    void init();
    void init_titlebar();

    void updateActions();

    QFrame *titlebar;
    QComboBox *cbTitle;
    QHBoxLayout *hbox_titlebar;

    QAction *backAct;
    QAction *forwardAct;
    QAction *closeAct;

    QStackedWidget *contentwidget;
};

#endif // STACKEDWIDGET_H
