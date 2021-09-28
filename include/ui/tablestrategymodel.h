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

private:
    QSolverJob* qSolverJob;
    void setupModelData();

public slots:
    void clicked_event(const QModelIndex & index);
};

#endif // TABLESTRATEGYMODEL_H
