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
    void updateStrategyData();
    const vector<pair<GameActions,float>> get_strategy(int i,int j) const;

private:
    QSolverJob* qSolverJob;
    void setupModelData();
    TreeItem * treeItem = NULL;// = static_cast<TreeItem*>(index.internalPointer());
    Card turn_card;
    Card river_card;
    vector<vector<vector<float>>> current_strategy; // cardint(52) * cardint(52) * strategy_type
    vector<vector<vector<pair<int,int>>>> ui_strategy_table; // rank * rank * (id,id)
    map<int,Card> cardint2card;

public slots:
    void clicked_event(const QModelIndex & index);
};

#endif // TABLESTRATEGYMODEL_H