#ifndef __MAINWINDOW_H
#define __MAINWINDOW_H
#include<QMainWindow>
#include<QTimer>
#include"ui_MainWindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
protected slots:
    void loadParameters();
    void saveParameters();

    void loadModel();
private:
    Ui::MainWindow ui;
};

#endif
