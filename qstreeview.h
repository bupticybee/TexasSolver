#ifndef QSTREEVIEW_H
#define QSTREEVIEW_H
#include "QTreeView"
#include "QObject"
#include "include/runtime/qsolverjob.h"
#include "include/ui/treemodel.h"

class QSTreeView: public QTreeView
{
public:
    explicit QSTreeView(QWidget *parent = nullptr);
    ~QSTreeView();
    void setTreeData(QSolverJob* qSolverJob);
    TreeModel* tree_model;

private:
    QSolverJob * qSolverJob=NULL;
};

#endif // QSTREEVIEW_H
