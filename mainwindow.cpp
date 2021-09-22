#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stdio.h"
#include "include/runtime/qsolverjob.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(this->ui->actionChinese, SIGNAL(triggered()), this, SLOT(on_chinese_local()));
    connect(this->ui->actionEng, SIGNAL(triggered()), this, SLOT(on_en_local()));
    qSolverJob.setContext(this->getLogArea());
    this->setWindowTitle("TexasSolver");
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

void MainWindow::on_clearLogButtom_clicked()
{
    this->ui->logOutput->clear();
}

void MainWindow::on_chinese_local(){

}

void MainWindow::on_en_local(){

}
