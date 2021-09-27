#include "include/ui/treemodel.h"

TreeModel::TreeModel(QSolverJob * data, QObject *parent)
    : QAbstractItemModel(parent)
{
    this->qSolverJob = data;
    setupModelData();
}

TreeModel::~TreeModel()
{
    delete rootItem;
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    return rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return item->data();
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data();

    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

void TreeModel::reGenerateTreeItem(GameTreeNode::GameRound round,TreeItem* node_to_process){
    const shared_ptr<GameTreeNode> gameTreeNode = node_to_process->m_treedata.lock();
    if(gameTreeNode->getType() == GameTreeNode::GameTreeNodeType::ACTION){
        shared_ptr<ActionNode> actionNode = dynamic_pointer_cast<ActionNode>(gameTreeNode);
        vector<shared_ptr<GameTreeNode>>& childrens = actionNode->getChildrens();
        for(shared_ptr<GameTreeNode> one_child:childrens){
            TreeItem * child_node = new TreeItem(one_child,node_to_process);
            node_to_process->insertChild(child_node);
            if(one_child->getRound() != round){
                continue;
            }
            this->reGenerateTreeItem(round,child_node);
        }
    }
    else if(gameTreeNode->getType() == GameTreeNode::GameTreeNodeType::CHANCE){
        shared_ptr<ChanceNode> chanceNode = dynamic_pointer_cast<ChanceNode>(gameTreeNode);
        TreeItem * child_node = new TreeItem(chanceNode->getChildren(),node_to_process);
        node_to_process->insertChild(child_node);
        this->reGenerateTreeItem(round,child_node);
    }
}

void TreeModel::setupModelData()
{
    PokerSolver * solver;
    if(this->qSolverJob->mode == QSolverJob::Mode::HOLDEM){
        solver = &(this->qSolverJob->ps_holdem);
    }else if(this->qSolverJob->mode == QSolverJob::Mode::SHORTDECK){
        solver = &(this->qSolverJob->ps_shortdeck);
    }else{
        throw runtime_error("holdem mode incorrect");
    }

    if(solver->get_game_tree() == nullptr || solver->get_game_tree()->getRoot() == nullptr){
        return;
    }

    GameTreeNode::GameRound round =	solver->get_game_tree()->getRoot()->getRound();

    this->rootItem = new TreeItem(solver->get_game_tree()->getRoot());
    TreeItem* ti = new TreeItem(solver->get_game_tree()->getRoot(),this->rootItem);
    rootItem->insertChild(ti);
    this->reGenerateTreeItem(round,ti);
}


void TreeModel::clicked_event(const QModelIndex & index){
}
