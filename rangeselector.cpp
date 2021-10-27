#include "rangeselector.h"
#include "ui_rangeselector.h"

RangeSelector::RangeSelector(QTextEdit* rangeEdit,QWidget *parent,QSolverJob::Mode mode) :
    QDialog(parent),
    ui(new Ui::RangeSelector)
{

    QString ranks;
    if(mode == QSolverJob::Mode::HOLDEM){
        ranks = "A,K,Q,J,T,9,8,7,6,5,4,3,2";
    }else if(mode == QSolverJob::Mode::SHORTDECK){
        ranks = "A,K,Q,J,T,9,8,7,6";
    }else{
        throw runtime_error("mode not found in range selector");
    }
    this->rank_list = ranks.split(",");
    this->rangeSelectorTableModel = new RangeSelectorTableModel(this->rank_list,rangeEdit->toPlainText(),this);
    this->rangeSelectorTableDelegate = new RangeSelectorTableDelegate(this->rank_list,this->rangeSelectorTableModel,this);
    ui->setupUi(this);
    this->ui->rangeTableView->setModel(this->rangeSelectorTableModel);
    this->ui->rangeTableView->setItemDelegate(this->rangeSelectorTableDelegate);
    this->rangeEdit = rangeEdit;
    this->ui->textEdit->setText(this->rangeEdit->toPlainText());
    this->ui->rangeNumberSlider->setMaximum(this->max_val);
    this->ui->rangeNumberSlider->setValue(this->max_val);
    this->setWindowTitle(tr("RangeSelector"));
    connect(this->ui->rangeTableView,SIGNAL(view_item_pressed(int,int)),this,SLOT(grid_pressed(int,int)));
    connect(this->ui->rangeTableView,SIGNAL(view_item_area(int,int,int,int)),this,SLOT(grid_area(int,int,int,int)));
    connect(this->ui->rangeTableView,SIGNAL(item_release(int,int)),this,SLOT(grid_release(int,int)));
    /*
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update_second()));
    timer->start(1000);
    */
}

void RangeSelector::grid_area(int i1,int j1,int i2,int j2){
    if(i1 <= i2 && j1 <= j2){
        for(int i = i1;i <= i2;i ++){
            for(int j = j1;j <= j2;j ++){
                this->rangeSelectorTableModel->setRangeAt(i,j,this->range_num);
            }
        }
    }
    this->ui->textEdit->setText(this->rangeSelectorTableModel->getRangeText());
    //this->ui->rangeTableView->update();
    // DIRTY TRICK TO REPLACE update(), update() doesn't work here for some reason I dont know.
    this->ui->textEdit->setFocus();
    this->ui->rangeTableView->setFocus();
}

void RangeSelector::grid_release(int i,int j){
    //this->ui->rangeTableView->update();
    this->ui->textEdit->setFocus();
    this->ui->rangeTableView->setFocus();
}

void RangeSelector::grid_pressed(int i,int j){

    float range_float = this->rangeSelectorTableModel->getRangeAt(i,j);
    if(this->range_num == range_float){
        this->rangeSelectorTableModel->setRangeAt(i,j,0);
    }else{
        this->rangeSelectorTableModel->setRangeAt(i,j,this->range_num);
    }
    if(this->rangeSelectorTableModel->getRangeText() != this->ui->textEdit->toPlainText()){
        this->ui->textEdit->setText(this->rangeSelectorTableModel->getRangeText());
    }
    //this->ui->rangeTableView->update();
}

void RangeSelector::update_second(){
    this->ui->rangeTableView->update();
}

RangeSelector::~RangeSelector()
{
    delete ui;
    delete this->rangeSelectorTableModel;
}

void RangeSelector::on_confirmButtom_clicked()
{
    this->rangeEdit->setText(this->ui->textEdit->toPlainText());
    close();
}

void RangeSelector::on_rangeNumberSlider_valueChanged(int value)
{
    this->range_num = float(this->ui->rangeNumberSlider->value()) / this->max_val;
    if(this->range_num != this->ui->rangeNumberEdit->text().toFloat()){
        this->ui->rangeNumberEdit->setText(QString("%2").arg(QString::number(this->range_num,'f',3)));
    }
}

void RangeSelector::on_rangeNumberEdit_textEdited(const QString &arg1)
{
    this->range_num = this->ui->rangeNumberEdit->text().toFloat();
    this->ui->rangeNumberSlider->setValue((int)(this->max_val * this->range_num));
}

void RangeSelector::on_clearRangeButtom_clicked()
{
    this->ui->textEdit->setText("");
    this->rangeSelectorTableModel->clear_range();
    // DIRTY TRICK TO REPLACE update(), update() doesn't work here for some reason I dont know.
    this->ui->textEdit->setFocus();
    this->ui->rangeTableView->setFocus();
    this->ui->textEdit->setFocus();
}

void RangeSelector::on_textEdit_textChanged()
{
    QString range_text = this->ui->textEdit->toPlainText();
    this->rangeSelectorTableModel->setRangeText(range_text);
    // DIRTY TRICK TO REPLACE update(), update() doesn't work here for some reason I dont know.
    this->ui->rangeTableView->setFocus();
    this->ui->textEdit->setFocus();
}

void RangeSelector::on_exportRangeButton_clicked()
{
    QString range_text = this->ui->textEdit->toPlainText();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Range"),
                               "output_range.txt",
                               tr("Text file (*.txt)"));
    setlocale(LC_ALL,"");

    ofstream fileWriter;
    fileWriter.open(fileName.toLocal8Bit());
    QMessageBox msgBox;
    QString message;
    if(!fileWriter.fail()){
        fileWriter << range_text.toStdString();
        fileWriter.flush();
        fileWriter.close();

         message = QObject::tr("save success");
    }else{
        message = QObject::tr("save failed, file cannot be open");
    }
    qDebug().noquote() << message;
    msgBox.setText(message);
    setlocale(LC_CTYPE, "C");
    msgBox.exec();
}
