#include "openprojcet.h"
#include <QFileDialog>

using namespace CC;
OpenProjcet::OpenProjcet(QObject *parent) : QObject(parent)
{

}

QJsonObject CC::OpenProjcet::open()
{
    QFileDialog::getOpenFileName();
    return QJsonObject();
}
