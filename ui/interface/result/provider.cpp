#include "provider.h"
#include <QDebug>

namespace CC {

Provider *Provider::Instance = new Provider;

Provider::Provider( QObject* parent )
    : QObject( parent ) {
}

Provider *Provider::instance()
{
    return Instance;
}
void Provider::addItem(const QString& file, const QString& severity,
                       const QString& id, int line, const QString& summary,const QJsonArray &array) {
    auto item = QSharedPointer<DataItemRO>( new DataItemRO(
                                                file, severity, id, line, summary,array));
    m_items << item;
}

void Provider::removeItem(int index) {
    if (index < m_items.count()) {
        m_items.removeAt( index );
    }
}

QObjectListModel_DataItemRO* Provider::items() {
    return &m_items;
}
}
