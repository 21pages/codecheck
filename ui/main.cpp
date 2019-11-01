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
#ifdef _WIN32
#include <QMessageBox>
#else
#include <iostream>
#endif
#include "cppcheck.h"
//#include "common.h"
//#include "erroritem.h"

int main(int argc, char *argv[])
{

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

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

    QSettings* settings = new QSettings("WSD", "codecheck", &app);

    // Set data dir..
    foreach (const QString arg, app.arguments()) {
        if (arg.startsWith("--data-dir=")) {
            settings->setValue("DATADIR", arg.mid(11));
            return 0;
        }
    }

//    app.setWindowIcon(QIcon(":cppcheck-gui.png"));

//    // Register this metatype that is used to transfer error info
//    qRegisterMetaType<ErrorItem>("ErrorItem");


    return app.exec();
}
