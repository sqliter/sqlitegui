#ifndef STACKEDPAGE_H
#define STACKEDPAGE_H

class QAction;
#include <QList>

class StackedPage
{
public:
    virtual QList<QAction*>* actions()=0;
};

#endif // STACKEDPAGE_H
