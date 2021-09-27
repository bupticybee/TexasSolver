#ifndef TREEMODEL_H
#define TREEMODEL_H

#include "include/ui/treeitem.h"
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include "include/runtime/qsolverjob.h"
#include "include/nodes/ActionNode.h"
#include "include/nodes/ChanceNode.h"
#include "include/nodes/TerminalNode.h"
#include "include/nodes/ShowdownNode.h"

//! [0]
class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit TreeModel(QSolverJob* data, QObject *parent = nullptr);
    ~TreeModel();

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    void reGenerateTreeItem(GameTreeNode::GameRound round,TreeItem* node_to_process);

private:
    QSolverJob* qSolverJob;
    void setupModelData();
    TreeItem *rootItem;

public slots:
    void clicked_event(const QModelIndex & index);
};

#endif // TREEMODEL_H
