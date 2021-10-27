#include "include/ui/boardselectortablemodel.h"

BoardSelectorTableModel::BoardSelectorTableModel(QStringList ranks,QString initial_board,QObject *parent):QAbstractItemModel(parent){
    this->ranklist = ranks;
    this->suitlist = QString("c,d,h,s").split(",");

    this->grids_string = vector<vector<QString>>(this->suitlist.size());
    for(int i = 0;i < this->suitlist.size();i ++){
        this->grids_string[i] = vector<QString>(this->ranklist.size());
        for(int j = 0;j < this->ranklist.size();j ++){
            this->grids_string[i][j] = this->get_ij_text(i,j);
            this->string2ij[this->get_ij_text(i,j)] = pair<int,int>(i,j);
        }
    }

    this->grids_float = vector<vector<float>>(this->suitlist.size());
    for(int i = 0;i < this->suitlist.size();i ++){
        this->grids_float[i] = vector<float>(this->ranklist.size(),0.0);
    }
    this->setBoardText(initial_board);
}

float BoardSelectorTableModel::getBoardAt(int i, int j){
    if(i < 0 || i >= this->ranklist.size()){
        qDebug().noquote() << "BoardSelectorTableModel::getBoardAt i not valid: " << i;
        return 0;
    }
    else if(j < 0 || j >= this->ranklist.size()){
        qDebug().noquote() << "BoardSelectorTableModel::getBoardAt j not valid: " << j;
        return 0;
    }
    else{
        return this->grids_float[i][j];
    }
}

void BoardSelectorTableModel::clear_board(){
    for(int i = 0;i < this->suitlist.size();i ++){
        for(int j = 0;j < this->ranklist.size();j ++){
            this->grids_float[i][j] = 0;
        }
    }
}

void BoardSelectorTableModel::setBoardAt(int i, int j,float value){
    if(i < 0 || i >= this->suitlist.size()){
        qDebug().noquote() << "BoardSelectorTableModel::setBoardAt i not valid: " << i;
    }
    else if(j < 0 || j >= this->ranklist.size()){
        qDebug().noquote() << "BoardSelectorTableModel::setBoardAt j not valid: " << j;
    }
    else{
        this->grids_float[i][j] = value;
    }
}

BoardSelectorTableModel::~BoardSelectorTableModel(){
}

QString BoardSelectorTableModel::getBoardText(){
    QString retval = "";
    for(int i = 0;i < this->suitlist.size();i ++){
        for(int j = 0;j < this->ranklist.size();j ++){
            if(this->grids_float[i][j] == 0)continue;
            QString one_board_str = QString("%1").arg(this->grids_string[i][j]);
            if(retval == ""){
                retval = one_board_str;
            }else{
                retval = retval + "," + one_board_str;
            }
        }
    }
    return retval;
}

void BoardSelectorTableModel::setBoardText(QString input_board){
    this->clear_board();
    for(QString one_board:input_board.split(",")){
        float board_float = 1.0;


        if(one_board.count(" ") == one_board.length() || one_board == "")continue;
        if(!this->string2ij.count(one_board)){
            qDebug().noquote() << QString(tr("skipping board %1, format error")).arg(one_board);
            continue;
        }
        pair<int,int> cord = this->string2ij[one_board];
        this->grids_float[cord.first][cord.second] = board_float;
    }
}

QString BoardSelectorTableModel::get_ij_text(int i,int j) const{
    int row = i;
    int col = j;
    return ranklist[col] + suitlist[row];
}

QVariant BoardSelectorTableModel::data(const QModelIndex &index, int role) const {
    int row = index.row();
    int col = index.column();
    return this->get_ij_text(row,col);
}

QModelIndex BoardSelectorTableModel::index(int row, int column,
          const QModelIndex &parent) const  {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    return createIndex(row, column, nullptr);
}

QVariant BoardSelectorTableModel::headerData(int section, Qt::Orientation orientation, int role){
    return QString::fromStdString("");
}

QModelIndex BoardSelectorTableModel::parent(const QModelIndex &child) const {
    return QModelIndex();
}

int BoardSelectorTableModel::rowCount(const QModelIndex &parent) const {
    return this->suitlist.size();
}

int BoardSelectorTableModel::columnCount(const QModelIndex &parent) const {
    return this->ranklist.size();
}
