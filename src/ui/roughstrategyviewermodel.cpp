#include "include/ui/roughstrategyviewermodel.h"

RoughStrategyViewerModel::RoughStrategyViewerModel(TableStrategyModel* tableStrategyModel, QObject *parent):QAbstractItemModel(parent){
    this->tableStrategyModel = tableStrategyModel;
}

RoughStrategyViewerModel::~RoughStrategyViewerModel()
{
}

QModelIndex RoughStrategyViewerModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    return createIndex(row, column, nullptr);
}

QVariant RoughStrategyViewerModel::headerData(int section, Qt::Orientation orientation, int role){
    return QString::fromStdString("");
}

QModelIndex RoughStrategyViewerModel::parent(const QModelIndex &child) const{
    return QModelIndex();
}

void RoughStrategyViewerModel::onchanged(){
    emit headerDataChanged(Qt::Horizontal, 0 , columnCount());
}

int RoughStrategyViewerModel::columnCount(const QModelIndex &parent) const
{
    return this->tableStrategyModel->total_strategy.size();
}

int RoughStrategyViewerModel::rowCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant RoughStrategyViewerModel::data(const QModelIndex &index, int role) const
{
    std::size_t col = index.column();
    if(col < this->tableStrategyModel->total_strategy.size()){
        pair<GameActions,pair<float,float>> one_strategy = this->tableStrategyModel->total_strategy[col];
        return QString::number(one_strategy.second.second);
    }else{
        return "Rough Strategy";
    }
}
