#ifndef BOARDSELECTOR_H
#define BOARDSELECTOR_H

#include <QDialog>
#include "include/runtime/qsolverjob.h"
#include <QTextEdit>
#include <QWidget>

namespace Ui {
class boardselector;
}

class boardselector : public QDialog
{
    Q_OBJECT

public:
    explicit boardselector(QTextEdit* boardEdit,QSolverJob::Mode mode = QSolverJob::Mode::HOLDEM,QWidget *parent = 0);
    ~boardselector();

private:
    Ui::boardselector *ui;
    QTextEdit* boardEdit = NULL;
    QSolverJob::Mode mode;
};

#endif // BOARDSELECTOR_H
