
#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

#define TREEMODEL_REGISTER \
{   \
    auto treeModel = CC::TreeModel::instance();                                   \
    engine.rootContext()->setContextProperty("treeModel", treeModel);   \
}

const QString CONST_file("file");
const QString CONST_iconFile("iconFile");
const QString CONST_severity("severity");
const QString CONST_severityStr("severityStr");
const QString CONST_line("line");
const QString CONST_id("id");
const QString CONST_summary("summary");
const QString CONST_hide("hide");
const QString CONST_message("message");
const QString CONST_column("column");
const QString CONST_inconclusive("inconclusive");
const QString CONST_file0("file0");
const QString CONST_sinceDate("sinceDate");
const QString CONST_tags("tags");

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
        iconFile,
        severityStr,
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
