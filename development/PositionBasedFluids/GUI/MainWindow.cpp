#include"MainWindow.h"

MainWindow::MainWindow()
{
    ui.setupUi(this);
    connect(&updateTimer,SIGNAL(timeout()),ui.widget,SLOT(update()));

    updateTimer.setInterval(1000.0/60);
    updateTimer.setSingleShot(false);
    updateTimer.start();
}
