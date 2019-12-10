#include "filefolderinterface.h"
#include <QDir>

FileFolderInterface::FileFolderInterface(QQuickItem *parent) : QQuickItem(parent)
{

    for(auto info :QDir::drives()) {m_driveList<<info.path();}
}

QStringList FileFolderInterface::driveModel() const
{
    return m_driveList;
}

QStringList FileFolderInterface::nameFilters() const
{
    return m_nameFilters;
}

void FileFolderInterface::setNameFilters(const QStringList &str)
{
    if(m_nameFilters != str) {
        m_nameFilters = str;
        emit nameFiltersChanged();
    }
}

QString FileFolderInterface::startDir() const
{
    return m_startDir;
}

void FileFolderInterface::setStartDir(const QString &str)
{
    QFileInfo info(str);
    if(!info.exists()) {
        m_startDir = "C:/";
    } else {
        m_startDir = info.absolutePath();
    }
}

QString FileFolderInterface::startName() const
{
    return m_startName;
}

void FileFolderInterface::setStartName(const QString &str)
{
    if(str != m_startName) {
        m_startName = str;
        emit startNameChanged();
    }
}

FileFolderInterface::FileFolderType FileFolderInterface::type() const
{
    return m_type;
}

void FileFolderInterface::setType(FileFolderInterface::FileFolderType type)
{
    if(m_type != type) {
        m_type = type;
        emit typeChanged();
    }
}

void FileFolderInterface::classBegin()
{
    const QJsonObject &obj = FileFolderData::instance()->obj();
    if(obj.contains("nameFilters")) {
        setNameFilters(obj.value("nameFilter").toVariant().toStringList());
    }
    if(obj.contains("startDir")) {
        setStartDir(obj.value("startDir").toString());
    }
    if(obj.contains("startName")) {
        setStartName(obj.value("startName").toString());
    }
    if(obj.contains("type")) {
        setType(static_cast<FileFolderType>(obj.value("type").toInt()));
    }
}

void FileFolderInterface::componentComplete()
{
    emit sigComponentComplete();
}

FileFolderData *FileFolderData::instance()
{
    static FileFolderData data;
    return &data;
}

QJsonObject FileFolderData::obj()
{
    return m_obj;
}

void FileFolderData::setObj(const QJsonObject &obj)
{
    m_obj = obj;
}
