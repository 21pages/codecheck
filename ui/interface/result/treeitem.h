
#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>

namespace  CC{
//! [0]
class TreeItem
{
public:
    explicit TreeItem(const QMap<QString,QVariant> &data,TreeItem *parentItem = nullptr);
    ~TreeItem();
    TreeItem(const TreeItem& other);
    TreeItem& operator = (const TreeItem & other);
    void appendChild(TreeItem *child);

    TreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(QString key) const;
    int row() const;
    TreeItem *parentItem();
    QList<TreeItem*> children();

private:
    QList<TreeItem*> m_childItems;
    QMap<QString,QVariant> m_itemData;
    TreeItem *m_parentItem;
};
//! [0]
}


#endif // TREEITEM_H
