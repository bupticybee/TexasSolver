#ifndef STRATEGYEXPLORER_H
#define STRATEGYEXPLORER_H

#include <QDialog>
#include "include/runtime/qsolverjob.h"
#include "QItemSelection"
#include "include/ui/worditemdelegate.h"
#include "include/ui/tablestrategymodel.h"
#include "include/ui/strategyitemdelegate.h"

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
    WordItemDelegate * delegate;
    TableStrategyModel * tableStrategyModel;
public slots:
    void item_expanded(const QModelIndex& index);
    void item_clicked(const QModelIndex& index);
    void selection_changed(const QItemSelection &selected,
                                            const QItemSelection &deselected);
};

#endif // STRATEGYEXPLORER_H
