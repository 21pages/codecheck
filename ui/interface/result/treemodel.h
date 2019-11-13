
#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include "iglobal.h"

#define TREEMODEL_REGISTER \
{   \
    auto treeModel = CC::TreeModel::instance();                                   \
    engine.rootContext()->setContextProperty("treeModel", treeModel);   \
}

class  ErrorItem;
namespace CC {
class TreeItem;
//! [0]
class TreeModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_DISABLE_COPY(TreeModel)

public:
    enum class TreeModelRoles : int {
        file = Qt::UserRole + 1,
        iconFile,
        severity,
        severityStr,
        line,
        id,
        summary,
        hide,
        message,
        column,
        inconclusive,
        file0,
        sinceDate,
        tags,
    };
    enum class TreeModelColumn : int {
        iconFile = 100,
        severityStr = 0,
        line,
        id,
        summary,
    };
    ~TreeModel() Q_DECL_OVERRIDE;
    /* QAbstractItemModel interface */
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QHash<int, QByteArray> roleNames() const override;
    static TreeModel* instance();
private:
    explicit TreeModel(QObject *parent = nullptr);
private:
    TreeItem *rootItem;
    QHash<int, QByteArray> m_roleNameMapping;


public:
    void BeginResetModel();
    void EndResetModel();
    bool addErrorItem(const ErrorItem &item);
private:
    bool addErrorItemExec(const ErrorItem &item);
    TreeItem * checkExistingItem(QList<TreeItem*> children, const QMap<QString,QVariant> &data);
private:
    QStringList mHiddenMessageId;
    QString mFilter;
    bool mHasVisibleErrors = false;
};
//! [0]
}


#endif // TREEMODEL_H
