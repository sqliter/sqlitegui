#include <QtGui>
#include "delegate.h"

//QWidget* LenDelegate::createEditor(QWidget *parent,
//                                    const QStyleOptionViewItem &option,
//                                    const QModelIndex &index) const
//{
//    QLineEdit *editor = new QLineEdit(parent);
//    editor->setStyleSheet(QString("QLineEdit{border:none}"));
//    editor->setGeometry(option.rect);
//    return editor;
//}

//void LentDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
//{
//    QLineEdit *ed = (QLineEdit*)editor;
//    ed->setText(index.data().toString());
//}

//void LenDelegate::setModelData(QWidget *editor,
//                                QAbstractItemModel *model,
//                                const QModelIndex &index) const
//{
//    QLineEdit *ed=(QLineEdit*)editor;
//    model->setData(index,QVariant(ed->text().toInt()));
//}

//void LenDelegate::paint(QPainter *painter,
//                         const QStyleOptionViewItem &option,
//                         const QModelIndex &index) const
//{
//    if(owner->selectionModel()->isSelected(index))
//        painter->fillRect(option.rect,
//                          QColor(51,204,102,122));
//    painter->drawText(option.rect,Qt::AlignLeft|Qt::AlignVCenter,
//                      index.data().toString());

//}

//void LenDelegate::updateEditorGeometry(QWidget *editor,
//                                        const QStyleOptionViewItem &option,
//                                        const QModelIndex &index) const
//{
//    editor->setGeometry(option.rect);
//}

TypeDelegate::TypeDelegate(QWidget *parent):QStyledItemDelegate(parent){
    types<<QString("char")<<QString("varchar")
        <<QString("integer")<<QString("float")
       <<QString("decimal")<<QString("datetime")
      <<QString("text")<<QString("blob");
    sorts<<QString()<<QString("ASC")<<QString("DESC");
    item_id = 0;
}

QWidget* TypeDelegate::createEditor(QWidget *parent,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
    QComboBox *editor = new QComboBox(parent);
    if(item_id == 0){
        editor->setEditable(true);
        editor->addItems(types);
    }
    else{
        editor->setEditable(false);
        editor->addItems(sorts);
    }

    //editor->setStyleSheet(QString("QLineEdit{border:none}"));
    editor->setGeometry(option.rect);
    //editor->setText(index.data().toString());
    return editor;
}

void TypeDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *ed = (QComboBox*)editor;
    QString text = index.data(Qt::EditRole).toString();

    if(text.isEmpty())return;
    if(text == QString(""))return;

    for(int i=0;i<ed->count();++i)
    {
        if(text == ed->itemText(i))
        {
            ed->setCurrentIndex(i);
            return;
        }
    }
    ed->addItem(text);
    for(int i=0;i<ed->count();++i)
    {
        if(text == ed->itemText(i))
        {
            ed->setCurrentIndex(i);
            return;
        }
    }

}

void TypeDelegate::setModelData(QWidget *editor,
                                QAbstractItemModel *model,
                                const QModelIndex &index) const
{
    QComboBox *ed=(QComboBox*)editor;
    model->setData(index,QVariant(ed->currentText()));
}

void TypeDelegate::paint(QPainter *painter,
                         const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
    if(owner->selectionModel()->isSelected(index))
        painter->fillRect(option.rect,
                          QColor(51,204,102,122));
    QRect rc=option.rect;
    rc.setLeft(rc.left()+3);
    painter->drawText(rc,Qt::AlignLeft|Qt::AlignVCenter,
                      index.data().toString());

}

void TypeDelegate::updateEditorGeometry(QWidget *editor,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}


QWidget* TextDelegate::createEditor(QWidget *parent,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
    QLineEdit *editor = new QLineEdit(parent);
    editor->setStyleSheet(QString("QLineEdit{border:none}"));
    editor->setGeometry(option.rect);
    //editor->setText(index.data().toString());
    return editor;
}

void TextDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit *le = (QLineEdit*)editor;
    le->setText(index.data().toString());
}

void TextDelegate::setModelData(QWidget *editor,
                                QAbstractItemModel *model,
                                const QModelIndex &index) const
{
    QLineEdit *le=(QLineEdit*)editor;
    model->setData(index,QVariant(le->text()));
}

void TextDelegate::paint(QPainter *painter,
                         const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
    if(owner->selectionModel()->isSelected(index))
        painter->fillRect(option.rect,
                          QColor(51,204,102,122));
    QRect rc=option.rect;
    rc.setLeft(rc.left()+3);
    painter->drawText(rc,Qt::AlignLeft|Qt::AlignVCenter,
                      index.data().toString());

}

void TextDelegate::updateEditorGeometry(QWidget *editor,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
void KeyDelegate::paint(QPainter *painter,
                        const QStyleOptionViewItem &option,
                        const QModelIndex &index) const
{
    if(owner->selectionModel()->isSelected(index))
        painter->fillRect(option.rect,
                          QColor(51,204,102,122));
    bool pk = index.data().toBool();
    QRect rc = option.rect;
    int size=14;
    rc.setRect(rc.left()+(rc.width()-size)/2,
               rc.top()+(rc.height()-size)/2,
               size,size);
    if(pk)
        painter->drawPixmap(rc,QPixmap(":/images/key.png"));
    else
        painter->fillRect(rc,painter->background());


    //if(index.data(Qt::BackgroundRole))
}

void CheckDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    if(owner->selectionModel()->isSelected(index))
        painter->fillRect(option.rect,
                          QColor(51,204,102,122));

    bool value = index.data(Qt::EditRole).toBool();
    QRect rc = option.rect;
    int size=16;
    rc.setRect(rc.left()+1+(rc.width()-size)/2,
               rc.top()+1+(rc.height()-size)/2,
               size,size);

    if(value)
        painter->drawPixmap(rc,QPixmap(":/images/check.png"));
        //checkOption.state = QStyle::State_On;
    else
        painter->drawPixmap(rc,QPixmap(":/images/uncheck.png"));
        //checkOption.state = QStyle::State_Off;


    //QApplication::style()->drawControl(QStyle::CE_CheckBox,
    //                                    &checkOption, painter);
}
