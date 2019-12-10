#ifndef FILEFOLDERINTERFACE_H
#define FILEFOLDERINTERFACE_H

#include <QQuickItem>
#include <QJsonObject>

#define FileFolder_Register   \
{\
    qmlRegisterType<FileFolderInterface>("FileFolder",1,0,"FileFolderInterface"); \
    qmlRegisterType<FileFolderData>("FileFolder",1,0,"FileFolderData"); \
    engine.rootContext()->setContextProperty("fileFolderData",FileFolderData::instance());\
}

class FileFolderData:public QObject {
    Q_OBJECT
public:
    static FileFolderData* instance();
    Q_INVOKABLE void setObj(const QJsonObject& obj);
    QJsonObject obj();
private:
    QJsonObject m_obj;
};

class FileFolderInterface : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QStringList driveModel READ driveModel CONSTANT)
    Q_PROPERTY(QStringList nameFilters READ nameFilters WRITE setNameFilters NOTIFY nameFiltersChanged)
    Q_PROPERTY(QString startDir READ startDir WRITE setStartDir NOTIFY startDirChanged)
    Q_PROPERTY(QString startName READ startName WRITE setStartName NOTIFY startNameChanged)
    Q_PROPERTY(FileFolderType type READ type WRITE setType NOTIFY typeChanged)
public:
    enum FileFolderType{
        FileFolderType_File,
        FileFolderType_Dir
    };
    Q_ENUM(FileFolderType)

    explicit FileFolderInterface(QQuickItem *parent = nullptr);
    QStringList driveModel() const;
    QStringList nameFilters()const;
    void setNameFilters(const QStringList &str);
    QString startDir() const;
    void setStartDir(const QString& str);
    QString startName() const;
    void setStartName(const QString& str);
    FileFolderType type() const;
    void setType(FileFolderType type);
    void classBegin() override;
    void componentComplete() override;
signals:
    void nameFiltersChanged();
    void startDirChanged();
    void startNameChanged();
    void typeChanged();
    void sigComponentComplete();
private:
    QStringList m_nameFilters = QStringList()<<"*.*";
    QString m_startDir = "C:/";
    QString m_startName = "";
    FileFolderType m_type = FileFolderType_Dir;
    QStringList m_driveList;
};

#endif // FILEFOLDERINTERFACE_H
