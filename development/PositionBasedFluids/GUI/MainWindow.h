#ifndef __MAINWINDOW_H
#define __MAINWINDOW_H
#include<QMainWindow>
#include"ui_MainWindow.h"

/*namespace Ui {
    MainWindow;
}*/

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
private:
    Ui::MainWindow ui;
};

#endif
