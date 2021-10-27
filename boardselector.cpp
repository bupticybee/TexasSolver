#include "boardselector.h"
#include "ui_boardselector.h"

boardselector::boardselector(QTextEdit* boardEdit,QSolverJob::Mode mode,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::boardselector)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("BoardSelector"));
    this->boardEdit = boardEdit;
    this->mode = mode;
}

boardselector::~boardselector()
{
    delete ui;
}
