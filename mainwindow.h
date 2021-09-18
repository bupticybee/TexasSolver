#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include "include/runtime/qsolverjob.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Ui::MainWindow * getPriUi();
    QTextEdit * getLogArea();

private slots:
    void on_buttomSolve_clicked();

private:
    Ui::MainWindow *ui;
    QSolverJob qSolverJob;
};

#endif // MAINWINDOW_H
