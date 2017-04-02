/*********************************************************************
 ** Copyright (C) 2016 Mayura Ramanayaka
 ** Main Repository : "http://mayuraray.github.io/shots/"
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This package is distributed in the hope that it will be useful,
 ** but without any warranty; without even the implied warranty of
 ** merchantability or fitness for a particular purpose .  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program. If not, see the license!
 **
 **********************************************************************/

#include "Views/mainwindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QFile>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(resources);

    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("Mayura Ramanayaka");
    QCoreApplication::setApplicationName("Shots");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    QScopedPointer<QMainWindow> viewer(new MainWindow);
    const QRect availableGeometry = QApplication::desktop()->availableGeometry(viewer.data());
    viewer->resize(availableGeometry.width() / 2 - 50, (availableGeometry.height() * 2) / 3);
    viewer->move((availableGeometry.width() - viewer->width()) / 2, (availableGeometry.height() - viewer->height()) / 2);
    viewer->show();

    return a.exec();
}
