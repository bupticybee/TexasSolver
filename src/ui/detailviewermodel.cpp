#include "include/ui/detailviewermodel.h"

DetailViewerModel::DetailViewerModel(TableStrategyModel* tableStrategyModel, QObject *parent):QAbstractItemModel(parent){
    this->tableStrategyModel = tableStrategyModel;
    this->columns = 4;
    this->rows = 3;
}

DetailViewerModel::~DetailViewerModel()
{
}

QModelIndex DetailViewerModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    return createIndex(row, column, nullptr);
}

QVariant DetailViewerModel::headerData(int section, Qt::Orientation orientation, int role){
    return QString::fromStdString("");
}

QModelIndex DetailViewerModel::parent(const QModelIndex &child) const{
    return QModelIndex();
}

int DetailViewerModel::columnCount(const QModelIndex &parent) const
{
    return this->columns;
}

int DetailViewerModel::rowCount(const QModelIndex &parent) const
{
    return this->rows;
}

QVariant DetailViewerModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int col = index.column();

    return "Viewer";
}

void DetailViewerModel::clicked_event(const QModelIndex & index){
}
