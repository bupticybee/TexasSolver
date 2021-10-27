#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stdio.h"
#include "include/runtime/qsolverjob.h"
#include <QFileDialog>
#include "include/library.h"

QSTextEdit* MainWindow::s_textEdit = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    MainWindow::s_textEdit = this->get_logwindow();
    connect(this->ui->actionChinese, SIGNAL(triggered()), this, SLOT(on_chinese_local()));
    connect(this->ui->actionEng, SIGNAL(triggered()), this, SLOT(on_en_local()));
    connect(this->ui->actionjson, SIGNAL(triggered()), this, SLOT(on_save_json()));
    qSolverJob = new QSolverJob;
    qSolverJob->setContext(this->getLogArea());
    qSolverJob->current_mission = QSolverJob::MissionType::LOADING;
    qSolverJob->start();
    this->setWindowTitle(tr("TexasSolver"));
}

QSTextEdit * MainWindow::get_logwindow(){
    return this->ui->logOutput;
}

MainWindow::~MainWindow()
{
    delete qSolverJob;
    delete ui;
}

void MainWindow::on_save_json(){
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               "output_strategy.json",
                               tr("Json file (*.json)"));
    this->qSolverJob->savefile = fileName;
    qSolverJob->current_mission = QSolverJob::MissionType::SAVING;
    qSolverJob->start();
}

void MainWindow::on_ip_range(QString range_text){
    this->ui->ipRangeText->setText(range_text);
}

void MainWindow::on_buttomSolve_clicked()
{   
    qSolverJob->max_iteration = ui->iterationText->text().toInt();
    qSolverJob->accuracy = ui->exploitabilityText->text().toFloat();
    qSolverJob->print_interval = ui->logIntervalText->text().toInt();
    qSolverJob->thread_number = ui->threadsText->text().toInt();
    qSolverJob->current_mission = QSolverJob::MissionType::SOLVING;
    qSolverJob->start();
}

Ui::MainWindow * MainWindow::getPriUi(){
    return this->ui;
}

QSTextEdit * MainWindow::getLogArea(){
    return this->ui->logOutput;
}

void MainWindow::on_clearLogButtom_clicked()
{
    this->ui->logOutput->clear();
}

void MainWindow::on_chinese_local(){

}

void MainWindow::on_en_local(){

}

vector<float> sizes_convert(QString input){
    QStringList list = input.split(" ");
    vector<float> sizes;
    foreach(QString num, list){
        sizes.push_back(num.toFloat());
    }
    return sizes;
}

void MainWindow::on_buildTreeButtom_clicked()
{
    qSolverJob->range_ip = this->ui->ipRangeText->toPlainText().toStdString();
    qSolverJob->range_oop = this->ui->oopRangeText->toPlainText().toStdString();
    qSolverJob->board = this->ui->boardText->toPlainText().toStdString();

    vector<string> board_str_arr = string_split(qSolverJob->board,',');
    if(board_str_arr.size() == 3){
        qSolverJob->current_round = 1;
    }else if(board_str_arr.size() == 4){
        qSolverJob->current_round = 2;
    }else if(board_str_arr.size() == 5){
        qSolverJob->current_round = 3;
    }else{
        this->ui->logOutput->log_with_signal(QString::fromStdString(tfm::format("Error : board %s not recognized",qSolverJob->board)));
        return;
    }
    qSolverJob->raise_limit = this->ui->raiseLimitText->text().toInt();
    qSolverJob->ip_commit = this->ui->potText->text().toFloat() / 2;
    qSolverJob->oop_commit = this->ui->potText->text().toFloat() / 2;
    qSolverJob->stack = this->ui->effectiveStackText->text().toFloat() + qSolverJob->ip_commit;
    qSolverJob->mode = this->ui->mode_box->currentIndex() == 0 ? QSolverJob::Mode::HOLDEM:QSolverJob::Mode::SHORTDECK;
    qSolverJob->allin_threshold = this->ui->allinThresholdText->text().toFloat();
    qSolverJob->use_isomorphism = this->ui->useIsoCheck->isChecked();

    StreetSetting gbs_flop_ip = StreetSetting(sizes_convert(ui->flop_ip_bet->text()),
                                              sizes_convert(ui->flop_ip_raise->text()),
                                              vector<float>{},
                                              ui->flop_ip_allin->isChecked()
                                              );
    StreetSetting gbs_turn_ip = StreetSetting(sizes_convert(ui->turn_ip_bet->text()),
                                              sizes_convert(ui->turn_ip_raise->text()),
                                              vector<float>{},
                                              ui->turn_ip_allin->isChecked()
                                              );
    StreetSetting gbs_river_ip = StreetSetting(sizes_convert(ui->river_ip_bet->text()),
                                              sizes_convert(ui->river_ip_raise->text()),
                                              vector<float>{},
                                              ui->river_ip_allin->isChecked()
                                              );

    StreetSetting gbs_flop_oop = StreetSetting(sizes_convert(ui->flop_oop_bet->text()),
                                              sizes_convert(ui->flop_oop_raise->text()),
                                              vector<float>{},
                                              ui->flop_oop_allin->isChecked()
                                              );
    StreetSetting gbs_turn_oop = StreetSetting(sizes_convert(ui->turn_oop_bet->text()),
                                              sizes_convert(ui->turn_oop_raise->text()),
                                              sizes_convert(ui->turn_oop_donk->text()),
                                              ui->turn_oop_allin->isChecked()
                                              );
    StreetSetting gbs_river_oop = StreetSetting(sizes_convert(ui->river_oop_bet->text()),
                                              sizes_convert(ui->river_oop_raise->text()),
                                              sizes_convert(ui->river_oop_donk->text()),
                                              ui->river_oop_allin->isChecked()
                                              );

    qSolverJob->gtbs = make_shared<GameTreeBuildingSettings>(gbs_flop_ip,gbs_turn_ip,gbs_river_ip,gbs_flop_oop,gbs_turn_oop,gbs_river_oop);
    qSolverJob->current_mission = QSolverJob::MissionType::BUILDTREE;
    qSolverJob->start();
}

void MainWindow::on_copyButtom_clicked()
{
    ui->flop_oop_bet->setText(ui->flop_ip_bet->text());
    ui->flop_oop_raise->setText(ui->flop_ip_raise->text());
    ui->flop_oop_allin->setChecked(ui->flop_ip_allin->isChecked());

    ui->turn_oop_bet->setText(ui->turn_ip_bet->text());
    ui->turn_oop_raise->setText(ui->turn_ip_raise->text());
    ui->turn_oop_allin->setChecked(ui->turn_ip_allin->isChecked());

    ui->river_oop_bet->setText(ui->river_ip_bet->text());
    ui->river_oop_raise->setText(ui->river_ip_raise->text());
    ui->river_oop_allin->setChecked(ui->river_ip_allin->isChecked());
}

void MainWindow::on_showResultButton_clicked()
{
    this->strategyExplorer = new StrategyExplorer(this,this->qSolverJob);
    strategyExplorer->setAttribute(Qt::WA_DeleteOnClose);
    strategyExplorer->show();
}

void MainWindow::on_stopSolvingButton_clicked()
{
    if(this->qSolverJob != NULL){
        this->qSolverJob->stop();
    }
}

void MainWindow::on_ipRangeSelectButtom_clicked()
{
    QSolverJob::Mode mode = this->ui->mode_box->currentIndex() == 0 ? QSolverJob::Mode::HOLDEM:QSolverJob::Mode::SHORTDECK;
    this->rangeSelector = new RangeSelector(this->ui->ipRangeText,this,mode);
    rangeSelector->setAttribute(Qt::WA_DeleteOnClose);
    rangeSelector->show();
}

void MainWindow::on_oopRangeSelectButtom_clicked()
{
    QSolverJob::Mode mode = this->ui->mode_box->currentIndex() == 0 ? QSolverJob::Mode::HOLDEM:QSolverJob::Mode::SHORTDECK;
    this->rangeSelector = new RangeSelector(this->ui->oopRangeText,this,mode);
    rangeSelector->setAttribute(Qt::WA_DeleteOnClose);
    rangeSelector->show();
}

float iso_corh(QString board){
    vector<string> board_str_arr = string_split(board.toStdString(),',');
    vector<Card> initialBoard;
    for(string one_board_str:board_str_arr){
        initialBoard.push_back(Card(one_board_str));
    }
    float corh = 1;
    uint16_t color_hash[4];
    for(int i = 0;i < 4;i ++)color_hash[i] = 0;
    for (Card one_card:initialBoard) {
        int rankind = one_card.getCardInt() % 4;
        int suitind = one_card.getCardInt() / 4;
        color_hash[rankind] = color_hash[rankind] | (1 << suitind);
    }
    for(int i = 0;i < 4;i ++){
        for(int j = 0;j < i;j ++){
            if(color_hash[i] == color_hash[j]){
                corh = corh * 0.70;
                continue;
            }
        }
    }
    return corh;
}

void MainWindow::on_estimateMemoryButtom_clicked()
{
    long long memory_float = this->qSolverJob->estimate_tree_memory(this->ui->ipRangeText->toPlainText(),this->ui->oopRangeText->toPlainText(),this->ui->boardText->toPlainText());
    // float32 should take 4bytes
    float corh = 1;
    if(this->ui->useIsoCheck->isChecked()){
        corh =iso_corh(this->ui->boardText->toPlainText());
    }
    float memory_mb = (float)memory_float / 1024 / 1024 * corh * 4 ;
    float memory_gb = (float)memory_float / 1024 / 1024 / 1024 * corh * 4;
    QString message;
    if(memory_gb == 0){
        message = tr("Please build tree first.");
    }else if(memory_gb < 1){
        message = tr("Estimated Memory Usage: ") + QString::number(memory_mb,'f',1) + tr(" Mb");
    }else{
        message = tr("Estimated Memory Usage: ") + QString::number(memory_gb,'f',2) + tr(" Gb");
    }
    qDebug().noquote() << message;
    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.exec();
}

void MainWindow::on_selectBoardButton_clicked()
{
    QSolverJob::Mode mode = this->ui->mode_box->currentIndex() == 0 ? QSolverJob::Mode::HOLDEM:QSolverJob::Mode::SHORTDECK;
    this->boardSelector = new boardselector(this->ui->boardText,mode,this);
    boardSelector->setAttribute(Qt::WA_DeleteOnClose);
    boardSelector->show();
}
