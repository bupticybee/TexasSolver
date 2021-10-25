#ifndef HTMLTABLEVIEW_H
#define HTMLTABLEVIEW_H
#include <QApplication>
#include <QTableView>
#include <QCursor>
#include <QMouseEvent>
#include <iostream>
#include "include/ui/worditemdelegate.h"
#include <QHeaderView>


class HtmlTableView: public QTableView
{
    Q_OBJECT
public:
    explicit HtmlTableView(QWidget *parent = 0);
    HtmlTableView();
    bool eventFilter(QObject *watched, QEvent *event);
    void triger_resize();
signals:
    void linkActivated(QString link);
    void linkHovered(QString link);
    void linkUnhovered();
    void itemMouseChange(int i,int j);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    QString anchorAt(const QPoint &pos) const;
    int last_bearing_i = 0;
    int last_bearing_j = 0;

private:
    QString _mousePressAnchor;
    QString _lastHoveredAnchor;
    void resizeEvent(QResizeEvent* ev);
};

#endif // HTMLTABLEVIEW_H
