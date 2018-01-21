#include"MainWindow.h"
#include<QFileDialog>
#include<QDataStream>

MainWindow::MainWindow()
{
    ui.setupUi(this);

    connect(ui.action_Reset,SIGNAL(triggered()),ui.widget,SLOT(reset()));
    connect(ui.action_Load_Parameters,SIGNAL(triggered()),this,SLOT(loadParameters()));
    connect(ui.action_Save_Parameters,SIGNAL(triggered()),this,SLOT(saveParameters()));


    connect(ui.spinBoxIterations,SIGNAL(valueChanged(int)),ui.widget,SLOT(setNumIterations(int)));
    connect(ui.spinBoxRadius,SIGNAL(valueChanged(double)),ui.widget,SLOT(setKernelSupport(double)));
    connect(ui.spinBoxParticleSize,SIGNAL(valueChanged(double)),ui.widget,SLOT(setParticleSize(double)));
    connect(ui.spinBoxTimeStep,SIGNAL(valueChanged(double)),ui.widget,SLOT(setTimestep(double)));
    connect(ui.spinBoxRestDensity,SIGNAL(valueChanged(double)),ui.widget,SLOT(setRestDensity(double)));

    connect(ui.spinBoxArtVisc,SIGNAL(valueChanged(double)),ui.widget,SLOT(setArtVisc(double)));
    connect(ui.spinBoxArtVort,SIGNAL(valueChanged(double)),ui.widget,SLOT(setArtVort(double)));
    connect(ui.spinBoxCFM,SIGNAL(valueChanged(double)),ui.widget,SLOT(setCfmRegularization(double)));
    connect(ui.spinBoxCorrConst,SIGNAL(valueChanged(double)),ui.widget,SLOT(setCorrConst(double)));
    connect(ui.spinBoxCorrDist,SIGNAL(valueChanged(double)),ui.widget,SLOT(setCorrDist(double)));
    connect(ui.spinBoxCorrExp,SIGNAL(valueChanged(double)),ui.widget,SLOT(setCorrExp(double)));
    connect(ui.checkBoxGPU,SIGNAL(stateChanged(int)),ui.widget,SLOT(setGPU(int)));
    connect(ui.checkBoxSurface,SIGNAL(stateChanged(int)),ui.widget,SLOT(setSurface(int)));
}

void MainWindow::loadParameters()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if(!fileName.isEmpty())
    {
        unsigned int iter = ui.spinBoxIterations->value();
        float particleSize = ui.spinBoxParticleSize->value();
        float kernelSupport = ui.spinBoxRadius->value();
        float timestep = ui.spinBoxTimeStep->value();
        float restDensity = ui.spinBoxRestDensity->value();
        float artVisc = ui.spinBoxArtVisc->value();
        float artVort = ui.spinBoxArtVort->value();
        float cfm = ui.spinBoxCFM->value();
        float corrConst = ui.spinBoxCorrConst->value();
        float corrDist = ui.spinBoxCorrDist->value();
        float corrExp = ui.spinBoxCorrExp->value();

        QFile file(fileName);
        if(file.open(QFileDevice::ReadOnly))
        {
            QDataStream out(&file);
            out.setVersion(QDataStream::Qt_5_0);
            out>>iter>>particleSize>>kernelSupport>>timestep>>restDensity>>artVisc>>artVort>>cfm>>corrConst>>corrDist>>corrExp;

            ui.spinBoxIterations->setValue(iter);
            ui.spinBoxParticleSize->setValue(particleSize);
            ui.spinBoxRadius->setValue(kernelSupport);
            ui.spinBoxTimeStep->setValue(timestep);
            ui.spinBoxRestDensity->setValue(restDensity);
            ui.spinBoxArtVisc->setValue(artVisc);
            ui.spinBoxArtVort->setValue(artVort);
            ui.spinBoxCFM->setValue(cfm);
            ui.spinBoxCorrConst->setValue(corrConst);
            ui.spinBoxCorrDist->setValue(corrDist);
            ui.spinBoxCorrExp->setValue(corrExp);
        }
    }
}

void MainWindow::saveParameters()
{
    QString fileName = QFileDialog::getSaveFileName(this);
    if(!fileName.isEmpty())
    {
        unsigned int iter = ui.spinBoxIterations->value();
        float particleSize = ui.spinBoxParticleSize->value();
        float kernelSupport = ui.spinBoxRadius->value();
        float timestep = ui.spinBoxTimeStep->value();
        float restDensity = ui.spinBoxRestDensity->value();
        float artVisc = ui.spinBoxArtVisc->value();
        float artVort = ui.spinBoxArtVort->value();
        float cfm = ui.spinBoxCFM->value();
        float corrConst = ui.spinBoxCorrConst->value();
        float corrDist = ui.spinBoxCorrDist->value();
        float corrExp = ui.spinBoxCorrExp->value();

        QFile file(fileName);
        if(file.open(QFileDevice::WriteOnly))
        {
            QDataStream out(&file);
            out.setVersion(QDataStream::Qt_5_0);
            out<<iter<<particleSize<<kernelSupport<<timestep<<restDensity<<artVisc<<artVort<<cfm<<corrConst<<corrDist<<corrExp;
        }
    }
}
