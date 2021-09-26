#ifndef STRATEGYEXPLORER_H
#define STRATEGYEXPLORER_H

#include <QDialog>
#include "include/runtime/qsolverjob.h"

namespace Ui {
class StrategyExplorer;
}

class StrategyExplorer : public QDialog
{
    Q_OBJECT

public:
    explicit StrategyExplorer(QWidget *parent = 0,QSolverJob * qSolverJob=nullptr);
    ~StrategyExplorer();

private:
    Ui::StrategyExplorer *ui;
    QSolverJob * qSolverJob;
};

#endif // STRATEGYEXPLORER_H
