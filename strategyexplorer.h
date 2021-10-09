#ifndef STRATEGYEXPLORER_H
#define STRATEGYEXPLORER_H

#include <QDialog>
#include <QTimer>
#include "include/runtime/qsolverjob.h"
#include "QItemSelection"
#include "include/ui/worditemdelegate.h"
#include "include/ui/tablestrategymodel.h"
#include "include/ui/strategyitemdelegate.h"
#include "include/Card.h"

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
    QTimer *timer;
    Ui::StrategyExplorer *ui;
    QSolverJob * qSolverJob;
    StrategyItemDelegate * delegate_strategy;
    TableStrategyModel * tableStrategyModel;
    vector<Card> cards;
public slots:
    void item_expanded(const QModelIndex& index);
    void item_clicked(const QModelIndex& index);
    void selection_changed(const QItemSelection &selected,
                                            const QItemSelection &deselected);
private slots:
    void on_turnCardBox_currentIndexChanged(int index);
    void on_riverCardBox_currentIndexChanged(int index);
    void update_second();
};

#endif // STRATEGYEXPLORER_H
