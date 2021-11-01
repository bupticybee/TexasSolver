#ifndef RANGESELECTORTABLEMODEL_H
#define RANGESELECTORTABLEMODEL_H

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
#include <vector>
#include <QAbstractItemModel>
#include <QStringList>
#include <QString>

class RangeSelectorTableModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit RangeSelectorTableModel(QStringList ranks,QString initial_board,QObject *parent = nullptr,bool thumbnail=false);
    ~RangeSelectorTableModel();

    float getRangeAt(int i, int j);
    void setRangeAt(int i, int j,float value);

    void setRangeText(QString input_range);
    QString getRangeText();
    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole );
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    void clear_range();
    bool in_thumbnail_mode(){return this->thumbnail;}
private:
    vector<vector<QString>> grids_string;
    vector<vector<float>> grids_float;
    map<QString,pair<int,int>> string2ij;
    QString get_ij_text(int i,int j);
    QStringList ranklist;
    bool thumbnail = false;
};


#endif // RANGESELECTORTABLEMODEL_H
