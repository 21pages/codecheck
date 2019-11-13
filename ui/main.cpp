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
#include "cppcheck.h"
//#include "common.h"
//#include "erroritem.h"
#include "material/src/plugin.h"
#include "material/src/core/device.h"
#include "material/src/core/units.h"
#include "manager.h"
#include "interface.h"
using namespace CC;

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    qputenv( "QSG_RENDER_LOOP", "basic" );
#endif

//#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
//    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
//    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
//#endif

//    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);

    QApplication app(argc, argv);

#if QT_VERSION < 0x050000
    // Set codecs so that UTF-8 strings in sources are handled correctly.
    // This is ONLY needed for Qt versions 4.x.
    // Qt 5.x assumes UTF-8 by default.
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif

    QCoreApplication::setOrganizationName("WSD");
    QCoreApplication::setApplicationName("codecheck");

//    app.setWindowIcon(QIcon(":cppcheck-gui.png"));

//    // Register this metatype that is used to transfer error info
//    qRegisterMetaType<ErrorItem>("ErrorItem");

    if ( QThreadPool::globalInstance()->maxThreadCount() > 1 )
    {
        QThreadPool::globalInstance()->setMaxThreadCount( QThreadPool::globalInstance()->maxThreadCount() - 1 );
    }
    Manager *manager = Manager::instance();
    Q_UNUSED(manager);


    QQmlApplicationEngine engine;
    REGISTERS_INITIALIZA;
    qmlRegisterSingletonType<Device>("Device", 0, 1, "Device", Device::qmlSingleton);
    qmlRegisterUncreatableType<Units>("Units", 0, 3, "Units", QStringLiteral("Units can only be used via the attached property."));
    engine.addImportPath(QString("qrc:/"));
    const QUrl url(QStringLiteral("qrc:/main2.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
