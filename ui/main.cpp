/*
 * Cppcheck - A tool for static C/C++ code analysis
 * Copyright (C) 2007-2018 Cppcheck team.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QApplication>
#include <QCoreApplication>
#include <QTextCodec>
#include <QMetaType>
#include <QStringList>
#include <QSettings>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QMessageBox>
#include <QThreadPool>
#include <QDir>
#include <QQuickStyle>
//#include "common.h"
//#include "erroritem.h"
//#include "material/src/plugin.h"
//#include "material/src/core/device.h"
//#include "material/src/core/units.h"
#include "manager.h"
#include "interface.h"
#include "cppcheck.h"
using namespace CC;

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    qputenv( "QSG_RENDER_LOOP", "basic" ); // threaded
#endif

//#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
//    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
//    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
//#endif
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("WSD");
    QCoreApplication::setApplicationName("codecheck");

    if (qgetenv("QT_QUICK_CONTROLS_1_STYLE").isEmpty()) {
#ifdef QT_STATIC
        // Need a full path to find the style when built statically
        qputenv("QT_QUICK_CONTROLS_1_STYLE", ":/ExtrasImports/QtQuick/Controls/Styles/Flat");
#else
        qputenv("QT_QUICK_CONTROLS_1_STYLE", "Flat");
#endif
    }

    if ( QThreadPool::globalInstance()->maxThreadCount() > 1 )
    {
        QThreadPool::globalInstance()->setMaxThreadCount( QThreadPool::globalInstance()->maxThreadCount() - 1 );
    }
    QQmlApplicationEngine engine;
    Manager *manager = Manager::instance();
    manager->setEngine(&engine);
    REGISTERS_INITIALIZA

//    qmlRegisterSingletonType<Device>("Device", 0, 1, "Device", Device::qmlSingleton);
//    qmlRegisterUncreatableType<Units>("Units", 0, 3, "Units", QStringLiteral("Units can only be used via the attached property."));
//    engine.addImportPath(QString("qrc:/"));
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    qDebug()<<"main-threadid:"<<QThread::currentThread();

    return app.exec();
}
