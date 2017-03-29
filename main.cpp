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
