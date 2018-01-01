#include"MainWindow.h"

MainWindow::MainWindow()
{
    ui.setupUi(this);
    connect(ui.spinBoxIterations,SIGNAL(valueChanged(int)),ui.widget,SLOT(setNumIterations(int)));
    connect(ui.spinBoxRadius,SIGNAL(valueChanged(double)),ui.widget,SLOT(setKernelSupport(double)));
    connect(ui.spinBoxTimeStep,SIGNAL(valueChanged(double)),ui.widget,SLOT(setTimestep(double)));
    connect(ui.spinBoxRestDensity,SIGNAL(valueChanged(double)),ui.widget,SLOT(setRestDensity(double)));

    connect(ui.spinBoxArtVisc,SIGNAL(valueChanged(double)),ui.widget,SLOT(setArtVisc(double)));
    connect(ui.spinBoxArtVort,SIGNAL(valueChanged(double)),ui.widget,SLOT(setArtVort(double)));
    connect(ui.spinBoxCFM,SIGNAL(valueChanged(double)),ui.widget,SLOT(setCfmRegularization(double)));
    connect(ui.spinBoxCorrConst,SIGNAL(valueChanged(double)),ui.widget,SLOT(setCorrConst(double)));
    connect(ui.spinBoxCorrDist,SIGNAL(valueChanged(double)),ui.widget,SLOT(setCorrDist(double)));
    connect(ui.spinBoxCorrExp,SIGNAL(valueChanged(double)),ui.widget,SLOT(setCorrExp(double)));
}
