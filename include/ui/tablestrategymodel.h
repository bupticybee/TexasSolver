#ifndef TABLESTRATEGYMODEL_H
#define TABLESTRATEGYMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include "include/runtime/qsolverjob.h"
#include "include/nodes/ActionNode.h"
#include "include/nodes/ChanceNode.h"
#include "include/nodes/TerminalNode.h"
#include "include/nodes/ShowdownNode.h"
#include "include/ui/treeitem.h"
#include "include/nodes/GameActions.h"
#include <map>

class TableStrategyModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit TableStrategyModel(QSolverJob* data, QObject *parent = nullptr);
    ~TableStrategyModel();

    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole );
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    void setGameTreeNode(TreeItem* treeItem);
    void setTrunCard(Card turn_card);
    void setRiverCard(Card river_card);
    Card getTrunCard(){return turn_card;};
    Card getRiverCard(){return river_card;};
    void updateStrategyData();
    const vector<pair<GameActions,float>> get_strategy(int i,int j) const;
    const vector<pair<GameActions,pair<float,float>>> get_total_strategy() const;
    const vector<float> get_ev_grid(int i,int j) const;
    const vector<float> get_strategies_evs(int i,int j) const;
    vector<vector<vector<float>>> current_strategy; // cardint(52) * cardint(52) * strategy_type
    vector<vector<vector<float>>> current_evs; // cardint(52) * cardint(52) * strategy_type
    vector<vector<float>> p1_range; // cardint(52) * cardint(52)
    vector<vector<float>> p2_range; // cardint(52) * cardint(52)
    vector<vector<vector<pair<int,int>>>> ui_strategy_table; // rank * rank * (id,id)
    vector<vector<vector<pair<int,int>>>> ui_p1_range; // rank * rank * (id,id)
    vector<vector<vector<pair<int,int>>>> ui_p2_range; // rank * rank * (id,id)
    vector<pair<GameActions,pair<float,float>>> total_strategy;
    map<int,Card> cardint2card;
    TreeItem * treeItem = NULL;// = static_cast<TreeItem*>(index.internalPointer());
    QSolverJob* get_solver(){return this->qSolverJob;};
    int current_player;

private:
    QSolverJob* qSolverJob;
    void setupModelData();
    Card turn_card;
    Card river_card;

public slots:
    void clicked_event(const QModelIndex & index);
};

#endif // TABLESTRATEGYMODEL_H
