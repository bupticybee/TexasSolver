#ifndef HTMLTABLERANGEVIEW_H
#define HTMLTABLERANGEVIEW_H

#include <QApplication>
#include <QTableView>
#include <QCursor>
#include <QMouseEvent>
#include <iostream>
#include "htmltablerangeview.h"
#include <QHeaderView>
#include "htmltableview.h"
#include "include/ui/rangeselectortablemodel.h"

struct MouseTracker {
    bool tracking;
    int pressed_i;
    int pressed_j;
} ;

class HtmlTableRangeView: public HtmlTableView
{
    Q_OBJECT
public:
    explicit HtmlTableRangeView(QWidget *parent = 0);
signals:
    void view_item_pressed(int i,int j);
    void view_item_area(int i1,int j1,int i2,int j2);
    void item_release(int i,int j);
protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
private:
    MouseTracker mouseTracker;
};

#endif // HTMLTABLERANGEVIEW_H
