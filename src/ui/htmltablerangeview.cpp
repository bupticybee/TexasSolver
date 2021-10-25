#include "include/ui/htmltablerangeview.h"

HtmlTableRangeView::HtmlTableRangeView(QWidget *parent):
HtmlTableView(parent)
{
    viewport()->installEventFilter(this);
    setMouseTracking(true);
    mouseTracker.tracking = false;
    mouseTracker.pressed_i = -1;
    mouseTracker.pressed_j = -1;
}

bool HtmlTableRangeView::eventFilter(QObject *watched, QEvent *event){
    if(viewport() == watched){
        if(event->type() == QEvent::MouseButtonPress){
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            QModelIndex index = indexAt(mouseEvent->pos());
            if(index.isValid()){
                int i = index.row();
                int j = index.column();
                mouseTracker.tracking = true;
                mouseTracker.pressed_i = i;
                mouseTracker.pressed_j = j;
            }
        }
        else if(event->type() == QEvent::MouseMove){
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            QModelIndex index = indexAt(mouseEvent->pos());
            if(index.isValid()){
                int i = index.row();
                int j = index.column();
                if(i != this->last_bearing_i || j != last_bearing_j){
                    this->last_bearing_i = i;
                    this->last_bearing_j = j;
                    if(mouseTracker.tracking == true){
                        emit view_item_area(mouseTracker.pressed_i,mouseTracker.pressed_j,i,j);
                    }
                }
            }
        }
        else if(event->type() == QEvent::MouseButtonRelease){
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            QModelIndex index = indexAt(mouseEvent->pos());
            if(index.isValid()){
                int i = index.row();
                int j = index.column();
                mouseTracker.tracking = false;
                emit item_release(i,j);
            }
        }
        else if(event->type() == QEvent::Leave){
                mouseTracker.tracking = false;
        }else{

        }
    }
    return QTableView::eventFilter(watched, event);
}

void HtmlTableRangeView::mousePressEvent(QMouseEvent *event) {
    HtmlTableView::mousePressEvent(event);
    QModelIndex index = indexAt(event->pos());
    emit view_item_pressed(index.row(),index.column());
}


void HtmlTableRangeView::mouseReleaseEvent(QMouseEvent *event) {
    HtmlTableView::mouseReleaseEvent(event);
}
