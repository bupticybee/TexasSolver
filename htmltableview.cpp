#include "htmltableview.h"
#include <iostream>

HtmlTableView::HtmlTableView()
{

}

HtmlTableView::HtmlTableView(QWidget *parent)
{
    setMouseTracking(true);
}

void HtmlTableView::mousePressEvent(QMouseEvent *event) {
    QTableView::mousePressEvent(event);

    auto anchor = anchorAt(event->pos());
    _mousePressAnchor = anchor;
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

            auto html = model()->data(index, Qt::DisplayRole).toString();

            return wordDelegate->anchorAt(html, relativeClickPosition);
        }
    }

    return QString();
}


void HtmlTableView::resizeEvent(QResizeEvent* ev){
    int num_columns = this->model()->columnCount(QModelIndex());
    if (num_columns > 0) {
        int width = ev->size().width();
        int used_width = 0;
        // Set our widths to be a percentage of the available width
        for (int i = 0; i < num_columns - 1; i++) {
            int column_width = width / num_columns;
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
            int column_height = height / num_columns;
            this->setRowHeight(i, column_height);
            used_height += column_height;
        }
        this->setRowHeight(num_columns - 1, height - used_height);
    }
    QTableView::resizeEvent(ev);
}
