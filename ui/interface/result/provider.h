#pragma once

#include "dataitemro.h"
#include "qobject_list_model.h"
#include <QList>
#include <QObject>
#include <QSharedPointer>
#include <QJsonArray>

#define Provider_Register \
{       \
   qmlRegisterUncreatableType<CC::DataItemRO>("CC",1,0,"DataItemRO","interface"); \
   qmlRegisterUncreatableType<CC::QObjectListModel_DataItemRO>("CC",1,0,"QObjectListModel_DataItemRO","interface"); \
    auto provider = CC::Provider::instance();                                   \
    engine.rootContext()->setContextProperty("provider", provider);   \
}

// "app" namespace is defined on purpose to show the case of using
// list model within namespaces
namespace CC {

//    DECLARE_Q_OBJECT_LIST_MODEL( DataItem )
DECLARE_Q_OBJECT_LIST_MODEL( DataItemRO)

// Provider is an example of class with some business logic that
// needs to expose some collection of items of class DataItem
class Provider : public QObject {
    Q_OBJECT
    Q_PROPERTY( QObjectListModel_DataItemRO* items READ items CONSTANT )
public:
    Q_INVOKABLE void addItem(const QString& file, const QString& severity,
                             const QString& id, int line, const QString& summary, const QJsonArray &array);
    Q_INVOKABLE void removeItem(int index);
    static Provider* instance();

private:
    explicit Provider( QObject* parent = Q_NULLPTR );
private:
    // Since this getter is not safe (ownership remains to c++)
    // and it is used for QML only it'd better to make it private.
    QObjectListModel_DataItemRO* items();
    QObjectListModel_DataItemRO  m_items;
    static Provider *Instance;
};
}
