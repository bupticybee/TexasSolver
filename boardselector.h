#ifndef BOARDSELECTOR_H
#define BOARDSELECTOR_H

#include <QDialog>
#include "include/runtime/qsolverjob.h"
#include <QTextEdit>
#include <QWidget>
#include <QStringList>
#include "include/ui/boardselectortablemodel.h"
#include "include/ui/boardselectortabledelegate.h"

namespace Ui {
class boardselector;
}

class boardselector : public QDialog
{
    Q_OBJECT

public:
    explicit boardselector(QTextEdit* boardEdit,QSolverJob::Mode mode = QSolverJob::Mode::HOLDEM,QWidget *parent = 0);
    ~boardselector();

private slots:
    void on_boardSelectorTable_clicked(const QModelIndex &index);

    void on_boardEdit_textEdited(const QString &arg1);

    void on_boardEdit_textChanged(const QString &arg1);

    void on_confirmButton_clicked();

    void on_cancelButton_clicked();

    void on_clearBoardButton_clicked();

private:
    Ui::boardselector *ui;
    QTextEdit* boardEdit = NULL;
    QSolverJob::Mode mode;
    QStringList rank_list;
    BoardSelectorTableModel * boardSelectorTableModel = NULL;
    BoardSelectorTableDelegate * boardSelectorTableDelegate = NULL;
};

#endif // BOARDSELECTOR_H
