#ifndef DROPTEXTEDIT_H
#define DROPTEXTEDIT_H

#include <QApplication>
#include <QTextEdit>
#include <QDropEvent>
#include <QDebug>
#include <QMimeData>
#include <QUrl>
#include <QFile>
#include <QTextStream>

class DropTextEdit: public QTextEdit
{
    Q_OBJECT
public:
    explicit DropTextEdit(QWidget *parent = 0);
    DropTextEdit();

protected:
    void dropEvent(QDropEvent* event);
};

#endif // DROPTEXTEDIT_H
