#ifndef DETAILVIEWERMODEL_H
#define DETAILVIEWERMODEL_H

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
#include "include/ui/tablestrategymodel.h"
#include "include/ui/detailwindowsetting.h"
#include <map>

class DetailViewerModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit DetailViewerModel(TableStrategyModel* tableStrategyModel, QObject *parent = nullptr);
    ~DetailViewerModel();

    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole );
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int columns;
    int rows;
    TableStrategyModel* tableStrategyModel = NULL;

public slots:
    void clicked_event(const QModelIndex & index);
};

#endif // DETAILVIEWERMODEL_H
