#include <iostream>
#include <QApplication>
#include "GUI/MainWindow.h"
using namespace std;

int main(int argc, char *argv[])
{
    QSurfaceFormat format;
    format = QSurfaceFormat::defaultFormat();
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setMajorVersion(4);
    format.setMinorVersion(5);
    QSurfaceFormat::setDefaultFormat(format);
    QApplication app(argc,argv);
    MainWindow mainwindow;
    mainwindow.show();
    return app.exec();
}
