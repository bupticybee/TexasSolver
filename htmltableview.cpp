#include "htmltableview.h"
#include <iostream>

HtmlTableView::HtmlTableView()
{

}

HtmlTableView::HtmlTableView(QWidget *parent):
    QTableView(parent)
{
    viewport()->installEventFilter(this);
    setMouseTracking(true);
}

void HtmlTableView::mousePressEvent(QMouseEvent *event) {
    QTableView::mousePressEvent(event);

    auto anchor = anchorAt(event->pos());
    _mousePressAnchor = anchor;
}

bool HtmlTableView::eventFilter(QObject *watched, QEvent *event){
    if(viewport() == watched){
        if(event->type() == QEvent::MouseMove){
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QModelIndex index = indexAt(mouseEvent->pos());
        if(index.isValid()){
            int i = index.row();
            int j = index.column();
            if(i != this->last_bearing_i || j != last_bearing_j){
                this->last_bearing_i = i;
                this->last_bearing_j = j;
                emit itemMouseChange(i,j);
            }
        }
        else{
        }
        }
        else if(event->type() == QEvent::Leave){
        }
    }
    return QTableView::eventFilter(watched, event);
}

void HtmlTableView::mouseMoveEvent(QMouseEvent *event) {
    auto anchor = anchorAt(event->pos());

    if (_mousePressAnchor != anchor) {
        _mousePressAnchor.clear();
    }

    if (_lastHoveredAnchor != anchor) {
        _lastHoveredAnchor = anchor;
        if (!_lastHoveredAnchor.isEmpty()) {
            QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
            emit linkHovered(_lastHoveredAnchor);
        } else {
            QApplication::restoreOverrideCursor();
            emit linkUnhovered();
        }
    }
}


void HtmlTableView::mouseReleaseEvent(QMouseEvent *event) {
    if (!_mousePressAnchor.isEmpty()) {
        auto anchor = anchorAt(event->pos());

        if (anchor == _mousePressAnchor) {
            emit linkActivated(_mousePressAnchor);
        }

        _mousePressAnchor.clear();
    }

    QTableView::mouseReleaseEvent(event);
}

QString HtmlTableView::anchorAt(const QPoint &pos) const {
    auto index = indexAt(pos);
    if (index.isValid()) {
        auto delegate = itemDelegate(index);
        auto wordDelegate = qobject_cast<WordItemDelegate *>(delegate);
        if (wordDelegate != 0) {
            auto itemRect = visualRect(index);
            auto relativeClickPosition = pos - itemRect.topLeft();

            if(model() != NULL){
                auto html = model()->data(index, Qt::DisplayRole).toString();
                return wordDelegate->anchorAt(html, relativeClickPosition);
            }
        }
    }

    return QString();
}

void HtmlTableView::triger_resize(){
    QResizeEvent* ev = new QResizeEvent(this->size(),this->size());
    this->resizeEvent(ev);
    //this->resize(this->parentWidget()->size());
    delete ev;
}

void HtmlTableView::resizeEvent(QResizeEvent* ev){
    if(model() != NULL){
        int num_columns = this->model()->columnCount(QModelIndex());
        int num_rows = this->model()->rowCount(QModelIndex());
        if (num_columns > 0) {
        int width = ev->size().width();
        int used_width = 0;
        // Set our widths to be a percentage of the available width
        for (int i = 0; i < num_columns - 1; i++) {
            //int column_width = width / num_columns;
            int column_width = (int)((float)width * (i + 1) / num_columns) -  (int)((float)width * (i) / num_columns);
            this->setColumnWidth(i, column_width);
            used_width += column_width;
        }

        // Set our last column to the remaining width
        this->setColumnWidth(num_columns - 1, width - used_width);
        }
        if (num_columns > 0) {
        int height = ev->size().height();
        int used_height = 0;
        for (int i = 0; i < num_columns - 1; i++) {
            //int column_height = height / num_rows;
            int column_height = (int)((float)height * (i + 1) / num_rows) -  (int)((float)height * (i) / num_rows);
            this->setRowHeight(i, column_height);
            used_height += column_height;
        }
        this->setRowHeight(num_columns - 1, height - used_height);
        }
    }
    QTableView::resizeEvent(ev);
}
