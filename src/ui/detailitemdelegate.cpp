#include "include/ui/detailitemdelegate.h"
#include <QPainter>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QRect>
#include <QBrush>

DetailItemDelegate::DetailItemDelegate(DetailWindowSetting* detailWindowSetting,QObject *parent) :
    WordItemDelegate(parent)
{
    this->detailWindowSetting = detailWindowSetting;
}

void DetailItemDelegate::paint_strategy(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto options = option;
    initStyleOption(&options, index);

    const DetailViewerModel * detailViewerModel = qobject_cast<const DetailViewerModel*>(index.model());
    //vector<pair<GameActions,float>> strategy = detailViewerModel->tableStrategyModel->get_strategy(this->detailWindowSetting->grid_i,this->detailWindowSetting->grid_j);

    options.text = "";
    if(detailViewerModel->tableStrategyModel->treeItem != NULL &&
            detailViewerModel->tableStrategyModel->treeItem->m_treedata.lock()->getType() == GameTreeNode::GameTreeNode::ACTION){
        shared_ptr<GameTreeNode> node = detailViewerModel->tableStrategyModel->treeItem->m_treedata.lock();
        int strategy_number = 0;
        if(this->detailWindowSetting->grid_i >= 0 && this->detailWindowSetting->grid_j >= 0){
           strategy_number = detailViewerModel->tableStrategyModel->ui_strategy_table[this->detailWindowSetting->grid_i][this->detailWindowSetting->grid_j].size();
        }
        int ind = index.row() * detailViewerModel->columns + index.column();

        if(ind < strategy_number){

            pair<int,int> strategy_ui_table = detailViewerModel->tableStrategyModel->ui_strategy_table[this->detailWindowSetting->grid_i][this->detailWindowSetting->grid_j][ind];
            int card1 = strategy_ui_table.first;
            int card2 = strategy_ui_table.second;
            vector<float> strategy = detailViewerModel->tableStrategyModel->current_strategy[card1][card2];


            shared_ptr<ActionNode> actionNode = dynamic_pointer_cast<ActionNode>(node);

            vector<GameActions>& gameActions = actionNode->getActions();
            if(gameActions.size() != strategy.size())throw runtime_error("size mismatch in DetailItemItemDelegate paint");
            float fold_prob = 0;
            vector<float> strategy_without_fold;
            float strategy_without_fold_sum = 0;
            for(std::size_t i = 0;i < strategy.size();i ++){
                GameActions one_action = gameActions[i];
                if(one_action.getAction() == GameTreeNode::PokerActions::FOLD){
                    fold_prob = strategy[i];
                }else{
                    strategy_without_fold.push_back(strategy[i]);
                    strategy_without_fold_sum += strategy[i];
                }
            }

            for(std::size_t i = 0;i < strategy_without_fold.size();i ++){
                strategy_without_fold[i] = strategy_without_fold[i] / strategy_without_fold_sum;
            }

            // get range data - copied initially from paint_range
            float range_number;
            if(0 == detailViewerModel->tableStrategyModel->current_player){
                range_number = detailViewerModel->tableStrategyModel->p1_range[card1][card2];
            }else{
                range_number = detailViewerModel->tableStrategyModel->p2_range[card1][card2];
            }
            if(range_number < 0 || range_number > 1) throw runtime_error("range number incorrect in strategyitemdeletage");
            // got range data

            float not_in_range = 1 - range_number;
            int niR_height = (int)(not_in_range * option.rect.height() * 0.975);

            int disable_height = (int)(0.5 + fold_prob * (option.rect.height()-niR_height));
            int remain_height = option.rect.height() - niR_height - disable_height;

            // draw background for flod
        if ( disable_height > 0) {
            QRect rect(option.rect.left(), option.rect.top() + niR_height,\
                 option.rect.width(), disable_height);
            QBrush brush(QColor	(0,191,255));
            painter->fillRect(rect, brush);
        }
        if (remain_height > 0){
            int ind = 0;
            float last_prob = 0;
            int bet_raise_num = 0;
            for(std::size_t i = 0;i < strategy.size();i ++){
                GameActions one_action = gameActions[i];
                QBrush brush(Qt::gray);
                if(one_action.getAction() != GameTreeNode::PokerActions::FOLD){
                if(one_action.getAction() == GameTreeNode::PokerActions::CHECK
                || one_action.getAction() == GameTreeNode::PokerActions::CALL){
                    brush = QBrush(Qt::green);
                }
                else if(one_action.getAction() == GameTreeNode::PokerActions::BET
                || one_action.getAction() == GameTreeNode::PokerActions::RAISE){
                    int color_base = max(128 - 32 * bet_raise_num - 1,0);
                    brush = QBrush(QColor(255,color_base,color_base));
                    bet_raise_num += 1;
                }else{
                brush = QBrush(Qt::blue);
                }

                int delta_x = (int)(option.rect.width() * last_prob);
                int delta_width = (int)(option.rect.width() * (last_prob + strategy_without_fold[ind])) - (int)(option.rect.width() * last_prob);

                QRect rect(option.rect.left() + delta_x, option.rect.top() + niR_height + disable_height,\
                 delta_width , remain_height);
                painter->fillRect(rect, brush);
            }

                last_prob += strategy_without_fold[ind];
                ind += 1;
                }
            }
            options.text = "";
            options.text += detailViewerModel->tableStrategyModel->cardint2card[card1].toFormattedHtml();
            options.text += detailViewerModel->tableStrategyModel->cardint2card[card2].toFormattedHtml();
            options.text = "<h2 >" + options.text + "<\/h2>";
            for(std::size_t i = 0;i < strategy.size();i ++){
                GameActions one_action = gameActions[i];
                float one_strategy = strategy[i] * 100;
                if(one_action.getAction() ==  GameTreeNode::PokerActions::FOLD){
                    options.text +=  QString(" <h5> %1 : %2\%<\/h5>").arg(tr("FOLD"),QString::number(one_strategy,'f',1));
                }
                else if(one_action.getAction() ==  GameTreeNode::PokerActions::CALL){
                    options.text +=  QString(" <h5> %1 : %2\%<\/h5>").arg(tr("CALL"),QString::number(one_strategy,'f',1));
                }
                else if(one_action.getAction() ==  GameTreeNode::PokerActions::CHECK){
                    options.text +=  QString(" <h5> %1 : %2\%<\/h5>").arg(tr("CHECK"),QString::number(one_strategy,'f',1));
                }
                else if(one_action.getAction() ==  GameTreeNode::PokerActions::BET){
                    options.text +=  QString(" <h5> %1 %2 : %3\%<\/h5>").arg(tr("BET"),QString::number(one_action.getAmount()),QString::number(one_strategy,'f',1));
                }
                else if(one_action.getAction() ==  GameTreeNode::PokerActions::RAISE){
                    options.text +=  QString(" <h5> %1 %2 : %3\%<\/h5>").arg(tr("RAISE"),QString::number(one_action.getAmount()),QString::number(one_strategy,'f',1));
                }
            }
        }
    }

    QTextDocument doc;
    doc.setHtml(options.text);

    options.text = "";
    //options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &option, painter);

    painter->translate(options.rect.left(), options.rect.top());
    QRect clip(0, 0, options.rect.width(), options.rect.height());
    doc.drawContents(painter, clip);
}

void DetailItemDelegate::paint_range(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto options = option;
    initStyleOption(&options, index);

    const DetailViewerModel * detailViewerModel = qobject_cast<const DetailViewerModel*>(index.model());
    //vector<pair<GameActions,float>> strategy = detailViewerModel->tableStrategyModel->get_strategy(this->detailWindowSetting->grid_i,this->detailWindowSetting->grid_j);
    options.text = "";

    if(detailViewerModel->tableStrategyModel->treeItem != NULL){
        vector<pair<int,int>> card_cords;
        if(this->detailWindowSetting->mode == DetailWindowSetting::DetailWindowMode::RANGE_IP){
            card_cords = detailViewerModel->tableStrategyModel->ui_p1_range[this->detailWindowSetting->grid_i][this->detailWindowSetting->grid_j];
        }else{
            card_cords = detailViewerModel->tableStrategyModel->ui_p2_range[this->detailWindowSetting->grid_i][this->detailWindowSetting->grid_j];
        }

        int ind = index.row() * detailViewerModel->columns + index.column();
        if(ind < card_cords.size()){
            pair<int,int> cord = card_cords[ind];
            float range_number;
            if(this->detailWindowSetting->mode == DetailWindowSetting::DetailWindowMode::RANGE_IP){
                range_number = detailViewerModel->tableStrategyModel->p1_range[cord.first][cord.second];
            }else{
                range_number = detailViewerModel->tableStrategyModel->p2_range[cord.first][cord.second];
            }

            if(range_number < 0 || range_number > 1) throw runtime_error("range number incorrect in strategyitemdeletage");

            float fold_prob = 1 - range_number;
            int disable_height = (int)(fold_prob * option.rect.height());
            int remain_height = option.rect.height() - disable_height;

            // draw background for flod
            QRect rect(option.rect.left(), option.rect.top() + disable_height,\
             option.rect.width(), remain_height);
            QBrush brush(Qt::yellow);
            painter->fillRect(rect, brush);

            options.text = "";
            options.text += detailViewerModel->tableStrategyModel->cardint2card[cord.first].toFormattedHtml();
            options.text += detailViewerModel->tableStrategyModel->cardint2card[cord.second].toFormattedHtml();
            options.text = "<h2>" + options.text + "<\/h2>";

            options.text +=  QString(" <h2>%1<\/h2>").arg(QString::number(range_number,'f',3));
        }
    }

    QTextDocument doc;
    doc.setHtml(options.text);

    painter->translate(options.rect.left(), options.rect.top());
    QRect clip(0, 0, options.rect.width(), options.rect.height());
    doc.drawContents(painter, clip);
}

void DetailItemDelegate::paint_evs(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto options = option;
    initStyleOption(&options, index);

    const DetailViewerModel * detailViewerModel = qobject_cast<const DetailViewerModel*>(index.model());

    options.text = "";
    if(detailViewerModel->tableStrategyModel->treeItem != NULL &&
            detailViewerModel->tableStrategyModel->treeItem->m_treedata.lock()->getType() == GameTreeNode::GameTreeNode::ACTION){
        shared_ptr<GameTreeNode> node = detailViewerModel->tableStrategyModel->treeItem->m_treedata.lock();
        int strategy_number = 0;
        if(this->detailWindowSetting->grid_i >= 0 && this->detailWindowSetting->grid_j >= 0){
           strategy_number = detailViewerModel->tableStrategyModel->ui_strategy_table[this->detailWindowSetting->grid_i][this->detailWindowSetting->grid_j].size();
        }
        int ind = index.row() * detailViewerModel->columns + index.column();

        if(ind < strategy_number){

            pair<int,int> strategy_ui_table = detailViewerModel->tableStrategyModel->ui_strategy_table[this->detailWindowSetting->grid_i][this->detailWindowSetting->grid_j][ind];
            int card1 = strategy_ui_table.first;
            int card2 = strategy_ui_table.second;
            shared_ptr<ActionNode> actionNode = dynamic_pointer_cast<ActionNode>(node);
            vector<GameActions>& gameActions = actionNode->getActions();
            vector<float> evs = detailViewerModel->tableStrategyModel->current_evs.empty()? vector<float>(gameActions.size(),-1.0):detailViewerModel->tableStrategyModel->current_evs[card1][card2] ;
            if(gameActions.size() != evs.size())throw runtime_error("evs size mismatch in DetailItemItemDelegate paint");

            vector<float> strategy = detailViewerModel->tableStrategyModel->current_strategy[card1][card2];
            if(gameActions.size() != strategy.size())throw runtime_error("strategy size mismatch in DetailItemItemDelegate paint");

            float fold_prob = 0;
            vector<float> strategy_without_fold;
            float strategy_without_fold_sum = 0;
            for(std::size_t i = 0;i < strategy.size();i ++){
                GameActions one_action = gameActions[i];
                if(one_action.getAction() == GameTreeNode::PokerActions::FOLD){
                    fold_prob = strategy[i];
                }else{
                    strategy_without_fold.push_back(strategy[i]);
                    strategy_without_fold_sum += strategy[i];
                }
            }

            for(std::size_t i = 0;i < strategy_without_fold.size();i ++){
                strategy_without_fold[i] = strategy_without_fold[i] / strategy_without_fold_sum;
            }

            // get range data - copied initally from paint_range - probably could need cleanup
            float range_number;
            if(0 == detailViewerModel->tableStrategyModel->current_player){
                range_number = detailViewerModel->tableStrategyModel->p1_range[card1][card2];
            }else{
                range_number = detailViewerModel->tableStrategyModel->p2_range[card1][card2];
            }

            if(range_number < 0 || range_number > 1) throw runtime_error("range number incorrect in strategyitemdeletage");
            // got range data

            float not_in_range = 1 - range_number;
            int niR_height = (int)(not_in_range * option.rect.height() * 0.975);

            int disable_height = (int)(0.5 + fold_prob * (option.rect.height()-niR_height));
            int remain_height = option.rect.height() - niR_height - disable_height;

            // draw background for flod
        if (disable_height > 0) {
            QRect rect(option.rect.left(), option.rect.top() + niR_height,\
                 option.rect.width(), disable_height);
            QBrush brush(QColor	(0,191,255));
            painter->fillRect(rect, brush);
        }
        if (remain_height > 0){
            int ind = 0;
            float last_prob = 0;
            int bet_raise_num = 0;
            for(std::size_t i = 0;i < strategy.size();i ++){
                GameActions one_action = gameActions[i];
                float normalized_ev = normalization_tanh(node->getPot() * 3,evs[i]);
                QBrush brush(Qt::gray);
                if(one_action.getAction() != GameTreeNode::PokerActions::FOLD){
                    if(one_action.getAction() == GameTreeNode::PokerActions::CHECK
                    || one_action.getAction() == GameTreeNode::PokerActions::CALL){
                        int green = 255;
                        int blue = max((int)(225 - normalized_ev * 175),55);
                        int red = 55;
                        brush = QBrush(QColor(red,green,blue));
                }
                else if(one_action.getAction() == GameTreeNode::PokerActions::BET
                || one_action.getAction() == GameTreeNode::PokerActions::RAISE){
                     int color_base = max(128 - 32 * bet_raise_num - 1,0);
                     int blue = max((int)(255 - normalized_ev * (255 - color_base)), color_base);
                     int red = color_base + min((int)(normalized_ev * (255-color_base)),255-color_base);;
                     int green = color_base;
                     brush = QBrush(QColor(red,green,blue));

                    bet_raise_num += 1;
                }else{
                brush = QBrush(Qt::blue);
                }

                int delta_x = (int)(option.rect.width() * last_prob);
                int delta_width = (int)(option.rect.width() * (last_prob + strategy_without_fold[ind])) - (int)(option.rect.width() * last_prob);

                QRect rect(option.rect.left() + delta_x, option.rect.top() + niR_height + disable_height,\
                 delta_width , remain_height);
                painter->fillRect(rect, brush);
            }

                last_prob += strategy_without_fold[ind];
                ind += 1;
                }
            }

            options.text = "";
            options.text += detailViewerModel->tableStrategyModel->cardint2card[card1].toFormattedHtml();
            options.text += detailViewerModel->tableStrategyModel->cardint2card[card2].toFormattedHtml();
            options.text = "<h2>" + options.text + "<\/h2>";
            for(std::size_t i = 0;i < evs.size();i ++){
                GameActions one_action = gameActions[i];
                QString one_ev = evs[i] != evs[i]? tr("Can't calculate"):QString::number(evs[i],'f',1);
                QString ev_str = tr("EV");
                if(one_action.getAction() ==  GameTreeNode::PokerActions::FOLD){
                    options.text +=  QString(" <h5> %1 %2: %3<\/h5>").arg(tr("FOLD"),ev_str,one_ev);
                }
                else if(one_action.getAction() ==  GameTreeNode::PokerActions::CALL){
                    options.text +=  QString(" <h5> %1 %2: %3<\/h5>").arg(tr("CALL"),ev_str,one_ev);
                }
                else if(one_action.getAction() ==  GameTreeNode::PokerActions::CHECK){
                    options.text +=  QString(" <h5> %1 %2: %3<\/h5>").arg(tr("CHECK"),ev_str,one_ev);
                }
                else if(one_action.getAction() ==  GameTreeNode::PokerActions::BET){
                    options.text +=  QString(" <h5> %1 %2 %3: %4<\/h5>").arg(tr("BET"),QString::number(one_action.getAmount()),ev_str,one_ev);
                }
                else if(one_action.getAction() ==  GameTreeNode::PokerActions::RAISE){
                    options.text +=  QString(" <h5> %1 %2 %3: %4<\/h5>").arg(tr("RAISE"),QString::number(one_action.getAmount()),ev_str,one_ev);
                }
            }
        }
    }

    QTextDocument doc;
    doc.setHtml(options.text);

    options.text = "";
    //options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &option, painter);

    painter->translate(options.rect.left(), options.rect.top());
    QRect clip(0, 0, options.rect.width(), options.rect.height());
    doc.drawContents(painter, clip);
}

void DetailItemDelegate::paint_evs_only(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto options = option;
    initStyleOption(&options, index);

    const DetailViewerModel * detailViewerModel = qobject_cast<const DetailViewerModel*>(index.model());
    //vector<pair<GameActions,float>> strategy = detailViewerModel->tableStrategyModel->get_strategy(this->detailWindowSetting->grid_i,this->detailWindowSetting->grid_j);
    options.text = "";

    if(detailViewerModel->tableStrategyModel->treeItem != NULL &&
            detailViewerModel->tableStrategyModel->treeItem->m_treedata.lock()->getType() == GameTreeNode::GameTreeNode::ACTION){

        shared_ptr<GameTreeNode> node = detailViewerModel->tableStrategyModel->treeItem->m_treedata.lock();
        int strategy_number = 0;
        if(this->detailWindowSetting->grid_i >= 0 && this->detailWindowSetting->grid_j >= 0){
           strategy_number = detailViewerModel->tableStrategyModel->ui_strategy_table[this->detailWindowSetting->grid_i][this->detailWindowSetting->grid_j].size();
        }

        vector<float> evs = detailViewerModel->tableStrategyModel->get_ev_grid(this->detailWindowSetting->grid_i,this->detailWindowSetting->grid_j);
        std::size_t ind = index.row() * detailViewerModel->columns + index.column();

        if(ind < evs.size() and ind < strategy_number)
        {
            float one_ev = evs[ind];
            float normalized_ev = normalization_tanh(detailViewerModel->tableStrategyModel->get_solver()->stack,one_ev);
            //options.text += QString("</br>%1").arg(QString::number(normalized_ev));

            pair<int,int> strategy_ui_table = detailViewerModel->tableStrategyModel->ui_strategy_table[this->detailWindowSetting->grid_i][this->detailWindowSetting->grid_j][ind];
            int card1 = strategy_ui_table.first;
            int card2 = strategy_ui_table.second;

            int red = max((int)(255 - normalized_ev * 255),0);
            int green = min((int)(normalized_ev * 255),255);
            int blue = min(red, green);
            QBrush brush = QBrush(QColor(red,green,blue));

            // draw background for flod
            QRect rect(option.rect.left(), option.rect.top(),
             option.rect.width(), option.rect.height());
            painter->fillRect(rect, brush);

            options.text = "";
            options.text += detailViewerModel->tableStrategyModel->cardint2card[card1].toFormattedHtml();
            options.text += detailViewerModel->tableStrategyModel->cardint2card[card2].toFormattedHtml();
            options.text = "<h2>" + options.text + "<\/h2>";

            options.text +=  QString(" <h2>%1<\/h2>").arg(QString::number(one_ev,'f',3));
        }
    }

    QTextDocument doc;
    doc.setHtml(options.text);

    painter->translate(options.rect.left(), options.rect.top());
    QRect clip(0, 0, options.rect.width(), options.rect.height());
    doc.drawContents(painter, clip);
}

void DetailItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    painter->save();

    QRect rect(option.rect.left(), option.rect.top(),\
             option.rect.width(), option.rect.height());
    QBrush brush(Qt::gray);
    painter->fillRect(rect, brush);

    if(this->detailWindowSetting->mode == DetailWindowSetting::DetailWindowMode::STRATEGY){
        this->paint_strategy(painter,option,index);
    }
    else if(this->detailWindowSetting->mode == DetailWindowSetting::DetailWindowMode::RANGE_IP ||
            this->detailWindowSetting->mode == DetailWindowSetting::DetailWindowMode::RANGE_OOP ){
        this->paint_range(painter,option,index);
    }
    else if(this->detailWindowSetting->mode == DetailWindowSetting::DetailWindowMode::EV){
        this->paint_evs(painter,option,index);
    }
    else if(this->detailWindowSetting->mode == DetailWindowSetting::DetailWindowMode::EV_ONLY){
        this->paint_evs_only(painter,option,index);
    }


    painter->restore();
}
