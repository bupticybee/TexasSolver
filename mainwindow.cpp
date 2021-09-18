#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stdio.h"
#include "include/runtime/qsolverjob.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qSolverJob.setContext(this->getLogArea());
    // QDebugStream qout(std::cout, this->ui->logOutput);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_buttomSolve_clicked()
{   
    qSolverJob.start();
}

Ui::MainWindow * MainWindow::getPriUi(){
    return this->ui;
}

QTextEdit * MainWindow::getLogArea(){
    return this->ui->logOutput;
}
