#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPlainTextEdit>
#include "include/runtime/qsolverjob.h"
#include "qstextedit.h"
#include "strategyexplorer.h"
#include "rangeselector.h"
#include <QMessageBox>
#include "boardselector.h"
#include "settingeditor.h"
#include "include/ui/rangeselectortablemodel.h"
#include "include/ui/rangeselectortabledelegate.h"
#include <qlineedit.h>

class QLogger : public Logger {
public:
    QLogger(const char *path, const char *mode = "w+", bool timestamp = false, int period = 10):Logger(false, path, mode, timestamp, true, period) {}
    virtual void log(const char *format, ...) {
        if(timestamp) log_time();
        va_list args;
        va_start(args, format);
        if(file) {
            vfprintf(file, format, args);
            if((++step) == period) {
                step = 0;
                fflush(file);
            }
            if(new_line) fprintf(file, "\n");
#ifdef __GNUC__
            va_end(args);
            va_start(args, format);
#endif
        }
        // qDebug().noquote() << QString::vasprintf(QObject::tr(format).toLocal8Bit(), args);
        qDebug().noquote() << QString::vasprintf(QObject::tr(format).toStdString().c_str(), args);
        va_end(args);
    }
};

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
    void on_buildTreeButtom_clicked();
    void on_actionjson_triggered();
    void on_actionimport_triggered();
    void import_from_file(QString from);
    void on_actionexport_triggered();
    void on_actionSettings_triggered();
    void on_actionclear_all_triggered();
    void on_ip_range(QString range_text);
    void on_copyButtom_clicked();
    void on_showResultButton_clicked();
    void on_stopSolvingButton_clicked();
    void on_ipRangeSelectButtom_clicked();
    void on_oopRangeSelectButtom_clicked();
    void on_estimateMemoryButtom_clicked();
    void on_selectBoardButton_clicked();
    void on_openParametersFolderButton_clicked();
    void item_clicked(const QModelIndex&);

    void on_exportCurrentParameterButton_clicked();

    void on_ipRangeText_textChanged();

    void on_oopRangeText_textChanged();
    void onExpanded(const QModelIndex& index);

private:
    void clear_all_params();
    void get_tree_params();
    void get_solver_params();
    void show_tree_params();
    void show_solver_params();
    void set_bet_sizes(QLineEdit *edit, vector<float> *sizes) {
        string s = edit->text().toStdString();
        clt.set_bet_sizes(s, ' ', sizes);
    }
    void show_bet_sizes(QLineEdit *edit, vector<float> &sizes) {
        string s;
        join(sizes, ' ', s);
        edit->setText(s.c_str());
    }
    void update_range_ui();
    Ui::MainWindow *ui = NULL;
    QSolverJob* qSolverJob = NULL;
    CommandLineTool clt;
    Logger *logger = nullptr;
    QFileSystemModel * qFileSystemModel = NULL;
    StrategyExplorer* strategyExplorer = NULL;
    RangeSelector* rangeSelector = NULL;
    boardselector* boardSelector = NULL;
    SettingEditor* settingEditor = NULL;
    RangeSelectorTableDelegate * ip_delegate;
    RangeSelectorTableDelegate * oop_delegate;
    RangeSelectorTableModel * ip_model;
    RangeSelectorTableModel * oop_model;

};

#endif // MAINWINDOW_H
