#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPlainTextEdit>
#include "include/runtime/qsolverjob.h"
#include "qstextedit.h"
#include "strategyexplorer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static QSTextEdit * s_textEdit;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Ui::MainWindow * getPriUi();
    QSTextEdit * getLogArea();
    QSTextEdit * get_logwindow();

private slots:
    void on_buttomSolve_clicked();
    void on_clearLogButtom_clicked();
    void on_chinese_local();
    void on_en_local();
    void on_buildTreeButtom_clicked();
    void on_save_json();
    void on_copyButtom_clicked();
    void on_showResultButton_clicked();

    void on_stopSolvingButton_clicked();

private:
    Ui::MainWindow *ui;
    QSolverJob* qSolverJob;
    StrategyExplorer* strategyExplorer;

};

#endif // MAINWINDOW_H
