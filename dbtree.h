#ifndef DBTREE_H
#define DBTREE_H

#include <QTreeWidget>

class DBTree:public QTreeWidget
{
    Q_OBJECT
public:
protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
};

#endif // DBTREE_H
