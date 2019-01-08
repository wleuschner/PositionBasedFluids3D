#include <iostream>
#include <QApplication>
#include "GUI/MainWindow.h"
using namespace std;

int main(int argc, char *argv[])
{
    QGLFormat format;
    format = QGLFormat::defaultFormat();
    format.setProfile(QGLFormat::CoreProfile);
    format.setVersion(4,5);
    QGLFormat::setDefaultFormat(format);
    QApplication app(argc,argv);
    MainWindow mainwindow;
    mainwindow.show();
    return app.exec();
}
