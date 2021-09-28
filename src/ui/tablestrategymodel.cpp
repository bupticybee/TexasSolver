#include "include/ui/tablestrategymodel.h"

TableStrategyModel::TableStrategyModel(QSolverJob * data, QObject *parent)
    : QAbstractItemModel(parent)
{
    this->qSolverJob = data;
    setupModelData();
}

TableStrategyModel::~TableStrategyModel()
{
}

QModelIndex TableStrategyModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    return createIndex(row, column, nullptr);
}

QVariant TableStrategyModel::headerData(int section, Qt::Orientation orientation, int role){
    return QString::fromStdString("");
}

QModelIndex TableStrategyModel::parent(const QModelIndex &child) const{
    return QModelIndex();
}

int TableStrategyModel::columnCount(const QModelIndex &parent) const
{
    return this->qSolverJob->get_solver()->get_deck()->getRanks().size();
}

int TableStrategyModel::rowCount(const QModelIndex &parent) const
{
    return this->qSolverJob->get_solver()->get_deck()->getRanks().size();
}

QVariant TableStrategyModel::data(const QModelIndex &index, int role) const
{
    vector<string>ranks = this->qSolverJob->get_solver()->get_deck()->getRanks();
    int row = index.row();
    int col = index.column();
    int larger = row > col?row:col;
    int smaller = row > col?col:row;
    QString  retval = QString("<h4>%1%2%3</h4>")\
            .arg(QString::fromStdString(ranks[ranks.size() - 1 - smaller]))\
            .arg(QString::fromStdString(ranks[ranks.size() - 1 - larger]));
    if(row > col){
        retval = retval.arg("o");
    }else if(row < col){
        retval = retval.arg("s");
    }else{
        retval = retval.arg("");
    }
    return retval;
}

void TableStrategyModel::setupModelData()
{
}

void TableStrategyModel::clicked_event(const QModelIndex & index){
}
