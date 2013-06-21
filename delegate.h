#ifndef DELEGATE_H
#define DELEGATE_H
#include <QStyledItemDelegate>

class QTableView;
class QWidget;

class TypeDelegate:public QStyledItemDelegate
{
    Q_OBJECT
public:
    TypeDelegate(QWidget *parent=0);

    QTableView *owner;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                               const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                   const QModelIndex &index) const;

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const;
    int item_id;
private:
    QStringList types;
    QStringList sorts;
};

//class LenDelegate:public QStyledItemDelegate
//{
//    Q_OBJECT
//public:
//    LenDelegate(QWidget *parent=0);

//    QTableView *owner;
//    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
//                               const QModelIndex &index) const;
//    void setEditorData(QWidget *editor, const QModelIndex &index) const;
//    void setModelData(QWidget *editor, QAbstractItemModel *model,
//                   const QModelIndex &index) const;

//    void paint(QPainter *painter,
//               const QStyleOptionViewItem &option,
//               const QModelIndex &index) const;
//    void updateEditorGeometry(QWidget *editor,
//                              const QStyleOptionViewItem &option,
//                              const QModelIndex &index) const;
//private:
//    QStringList types;
//};

class TextDelegate:public QStyledItemDelegate
{
    Q_OBJECT
public:
    TextDelegate(QWidget *parent=0):QStyledItemDelegate(parent){}

    QTableView *owner;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                               const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                   const QModelIndex &index) const;

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const;
};

class KeyDelegate:public QStyledItemDelegate
{
    Q_OBJECT
public:
    KeyDelegate(QWidget *parent=0):QStyledItemDelegate(parent){}
    QTableView *owner;
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
};

class CheckDelegate:public QStyledItemDelegate
{
    Q_OBJECT
public:
    CheckDelegate(QWidget *parent = 0) : QStyledItemDelegate(parent) {}

    QTableView *owner;

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
};
#endif // DELEGATE_H
