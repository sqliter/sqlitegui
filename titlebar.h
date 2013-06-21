#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QWidget>

//class QPushButton;
class QComboBox;
//class QWidgetAction;

class TitleBar:public QWidget{
    Q_OBJECT
public:
    TitleBar();

    void addTitle(const QString &title);
//public slots:
    void setCurrent(int index);
signals:
    void indexChanged(int index);
    void title_removed(int index);

private slots:
    void prev();
    void next();
    void remove();
private:
    void init();

    QAction *prevAct;
    QAction *nextAct;
    QAction *removeAct;

    QComboBox *cbTitle;

    //int cur_index;
};

#endif // TITLEBAR_H
