#include "strategyexplorer.h"
#include "ui_strategyexplorer.h"
#include "qstandarditemmodel.h"

StrategyExplorer::StrategyExplorer(QWidget *parent,QSolverJob * qSolverJob) :
    QDialog(parent),
    ui(new Ui::StrategyExplorer)
{
    this->qSolverJob = qSolverJob;
    ui->setupUi(this);
    /*
    QStandardItemModel* model = new QStandardItemModel();
    for (int row = 0; row < 4; ++row) {
         QStandardItem *item = new QStandardItem(QString("%1").arg(row) );
         model->appendRow( item );
    }
    this->ui->gameTreeView->setModel(model);
    */
    this->ui->gameTreeView->setTreeData(qSolverJob);
    connect(
                this->ui->gameTreeView,
                SIGNAL(expanded(const QModelIndex&)),
                this,
                SLOT(item_clicked(const QModelIndex&))
                );
}

StrategyExplorer::~StrategyExplorer()
{
    delete ui;
}

void StrategyExplorer::item_clicked(const QModelIndex& index){
    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    int num_child = item->childCount();
    for (int i = 0;i < num_child;i ++){
        TreeItem* one_child = item->child(i);
        if(one_child->childCount() != 0)continue;
        this->ui->gameTreeView->tree_model->reGenerateTreeItem(one_child->m_treedata.lock()->getRound(),one_child);
    }
}

void StrategyExplorer::selection_changed(const QItemSelection &selected,
                                         const QItemSelection &deselected){
}
