#include <iostream>
#include <QApplication>
#include "GUI/MainWindow.h"
using namespace std;

int main(int argc, char *argv[])
{
    QApplication app(argc,argv);
    MainWindow mainwindow;
    mainwindow.show();
    return app.exec();
}
