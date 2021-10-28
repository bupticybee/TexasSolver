#ifndef RANGESELECTOR_H
#define RANGESELECTOR_H

#include <QDialog>
#include <QString>
#include "include/nodes/GameTreeNode.h"
#include "include/runtime/qsolverjob.h"
#include "include/ui/rangeselectortablemodel.h"
#include "include/ui/rangeselectortabledelegate.h"
#include <QModelIndex>
#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>

namespace Ui {
class RangeSelector;
}

class RangeSelector : public QDialog
{
    Q_OBJECT

public:
    explicit RangeSelector(QTextEdit* rangeEdit,QWidget *parent = 0,QSolverJob::Mode mode = QSolverJob::Mode::HOLDEM);
    ~RangeSelector();
signals:
    void confirm_text(QString content);
private:
    int max_val = 1000;
    float range_num = 1;
    QSolverJob::Mode mode;
    Ui::RangeSelector *ui;
    QStringList rank_list;
    RangeSelectorTableModel * rangeSelectorTableModel = NULL;
    RangeSelectorTableDelegate * rangeSelectorTableDelegate = NULL;
    QTextEdit* rangeEdit = NULL;
    QTimer * timer;
private slots:
    void on_confirmButtom_clicked();
    void on_rangeNumberSlider_valueChanged(int value);
    void grid_pressed(int i,int j);
    void grid_area(int i1,int j1,int i2,int j2);
    void grid_release(int i,int j);
    void update_second();
    void on_rangeNumberEdit_textEdited(const QString &arg1);
    void on_clearRangeButtom_clicked();
    void on_textEdit_textChanged();
    void on_exportRangeButton_clicked();
    void on_cancelButton_clicked();
};

#endif // RANGESELECTOR_H
