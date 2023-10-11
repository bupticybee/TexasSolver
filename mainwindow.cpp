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
    connect(this->ui->actionjson, &QAction::triggered, this, &MainWindow::on_actionjson_triggered);
    connect(this->ui->actionSettings, &QAction::triggered, this, &MainWindow::on_actionSettings_triggered);
    connect(this->ui->actionimport, &QAction::triggered, this, &MainWindow::on_actionimport_triggered);
    connect(this->ui->actionexport, &QAction::triggered, this, &MainWindow::on_actionexport_triggered);
    connect(this->ui->actionclear_all, &QAction::triggered, this, &MainWindow::on_actionclear_all_triggered);
    qSolverJob = new QSolverJob;
    qSolverJob->setContext(this->getLogArea());
    qSolverJob->current_mission = QSolverJob::MissionType::LOADING;
    qSolverJob->start();
    this->setWindowTitle(tr("TexasSolver"));

    // parameters tree view
    QStringList filters;
    filters << "*.txt";
    qFileSystemModel = new QFileSystemModel(this);
    QDir filedir = QDir::current().filePath("parameters");
    qFileSystemModel->setRootPath(filedir.path());
#ifdef Q_OS_MAC
    filedir = QDir("");
    qFileSystemModel->setRootPath(filedir.path());
#endif
    qFileSystemModel->setNameFilters(filters);
    qFileSystemModel->setFilter(QDir::AllDirs | QDir::AllEntries |QDir::NoDotAndDotDot);
    this->ui->parametersTreeView->setModel(this->qFileSystemModel);
    this->ui->parametersTreeView->setRootIndex(this->qFileSystemModel->index(filedir.path()));
    for (int i=1; i<=4; i++)
    {   this->ui->parametersTreeView->hideColumn(i);}
    connect(
                this->ui->parametersTreeView,
                SIGNAL(clicked(const QModelIndex&)),
                this,
                SLOT(item_clicked(const QModelIndex&))
                );
    connect(ui->parametersTreeView, SIGNAL(expanded(QModelIndex)), this, SLOT(onExpanded(QModelIndex)));

    // Thumbnail process
    this->ip_model = new RangeSelectorTableModel(QString("A,K,Q,J,T,9,8,7,6,5,4,3,2").split(","),this->ui->ipRangeText->toPlainText(),this,true);
    this->ip_delegate = new RangeSelectorTableDelegate(QString("A,K,Q,J,T,9,8,7,6,5,4,3,2").split(","),this->ip_model,this);
    this->ui->IpRangeTableView->setModel(this->ip_model);
    this->ui->IpRangeTableView->setItemDelegate(this->ip_delegate);
    this->ui->IpRangeTableView->verticalHeader()->setMinimumSectionSize(1);
    this->ui->IpRangeTableView->horizontalHeader()->setMinimumSectionSize(1);

    this->oop_model = new RangeSelectorTableModel(QString("A,K,Q,J,T,9,8,7,6,5,4,3,2").split(","),this->ui->oopRangeText->toPlainText(),this,true);
    this->oop_delegate = new RangeSelectorTableDelegate(QString("A,K,Q,J,T,9,8,7,6,5,4,3,2").split(","),this->oop_model,this);
    this->ui->oopRangeTableView->setModel(this->oop_model);
    this->ui->oopRangeTableView->setItemDelegate(this->oop_delegate);
    this->ui->oopRangeTableView->verticalHeader()->setMinimumSectionSize(1);
    this->ui->oopRangeTableView->horizontalHeader()->setMinimumSectionSize(1);
    this->ui->tabWidget->hide();
}

QSTextEdit * MainWindow::get_logwindow(){
    return this->ui->logOutput;
}

MainWindow::~MainWindow()
{
    delete qSolverJob;
    delete qFileSystemModel;
    delete ip_delegate;
    delete ip_model;
    delete oop_delegate;
    delete oop_model;
    delete ui;
}

void MainWindow::on_actionjson_triggered(){
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               "output_strategy.json",
                               tr("Json file (*.json)"));
    if(fileName.isNull())return;
    this->qSolverJob->savefile = fileName;
    qSolverJob->current_mission = QSolverJob::MissionType::SAVING;
    qSolverJob->start();
}

QString getParams(QString input,QString key){
    if(input.contains(key)){
        return input.replace(key,"").trimmed();
    }else{
        return "INVALID";
    }
}

void MainWindow::on_actionclear_all_triggered(){
    this->clear_all_params();
    this->ui->IpRangeTableView->update();
    this->ui->oopRangeTableView->update();
    this->ui->IpRangeTableView->setFocus();
    this->ui->oopRangeTableView->setFocus();
}

void MainWindow::clear_all_params(){
    this->ui->potText->clear();
    this->ui->effectiveStackText->clear();
    this->ui->boardText->clear();
    this->ui->oopRangeText->clear();
    this->ui->ipRangeText->clear();
    this->ui->flop_oop_bet->clear();
    this->ui->flop_oop_raise->clear();
    this->ui->flop_oop_allin->setChecked(false);
    this->ui->flop_ip_bet->clear();
    this->ui->flop_ip_raise->clear();
    this->ui->flop_ip_allin->setChecked(false);
    this->ui->turn_oop_bet->clear();
    this->ui->turn_oop_raise->clear();
    this->ui->turn_oop_donk->clear();
    this->ui->turn_oop_allin->setChecked(false);
    this->ui->turn_ip_bet->clear();
    this->ui->turn_ip_raise->clear();
    this->ui->turn_ip_allin->setChecked(false);
    this->ui->river_oop_bet->clear();
    this->ui->river_oop_raise->clear();
    this->ui->river_oop_donk->clear();
    this->ui->river_oop_allin->setChecked(false);
    this->ui->river_ip_bet->clear();
    this->ui->river_ip_raise->clear();
    this->ui->river_ip_allin->setChecked(false);
    this->ui->allinThresholdText->clear();
    this->ui->threadsText->clear();
    this->ui->exploitabilityText->clear();
    this->ui->iterationText->clear();
    this->ui->logIntervalText->clear();
    this->ui->raiseLimitText->clear();
    this->ui->useIsoCheck->setChecked(false);
}

void MainWindow::import_from_file(QString fileName){
    if( fileName.isNull() )
    {
        qDebug().noquote() << tr("File selection invalid.");
        return;
    }
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)){
        qDebug().noquote() << tr("File open failed.");
        return;
    }
    QString content;
    QTextStream s1(&file);
    content.append(s1.readAll());
    this->clear_all_params();
    for(QString one_line_content:content.split("\n")){
        if(getParams(one_line_content,"set_pot") != "INVALID"){
            this->ui->potText->setText(getParams(one_line_content,"set_pot"));
        }
        else if(getParams(one_line_content,"set_effective_stack") != "INVALID"){
            this->ui->effectiveStackText->setText(getParams(one_line_content,"set_effective_stack"));
        }
        else if(getParams(one_line_content,"set_board") != "INVALID"){
            this->ui->boardText->setText(getParams(one_line_content,"set_board"));
        }
        else if(getParams(one_line_content,"set_range_oop") != "INVALID"){
            this->ui->oopRangeText->setText(getParams(one_line_content,"set_range_oop"));
        }
        else if(getParams(one_line_content,"set_range_ip") != "INVALID"){
            this->ui->ipRangeText->setText(getParams(one_line_content,"set_range_ip"));
        }
        // FLOP
        else if(getParams(one_line_content,"set_bet_sizes oop,flop,bet,") != "INVALID"){
            this->ui->flop_oop_bet->setText(getParams(one_line_content,"set_bet_sizes oop,flop,bet,").replace(',',' '));
        }
        else if(getParams(one_line_content,"set_bet_sizes oop,flop,raise") != "INVALID"){
            this->ui->flop_oop_raise->setText(getParams(one_line_content,"set_bet_sizes oop,flop,raise").replace(',',' '));
        }
        else if(getParams(one_line_content,"set_bet_sizes oop,flop,allin") != "INVALID"){
            this->ui->flop_oop_allin->setChecked(true);
        }
        else if(getParams(one_line_content,"set_bet_sizes ip,flop,bet,") != "INVALID"){
            this->ui->flop_ip_bet->setText(getParams(one_line_content,"set_bet_sizes ip,flop,bet,").replace(',',' '));
        }
        else if(getParams(one_line_content,"set_bet_sizes ip,flop,raise") != "INVALID"){
            this->ui->flop_ip_raise->setText(getParams(one_line_content,"set_bet_sizes ip,flop,raise").replace(',',' '));
        }
        else if(getParams(one_line_content,"set_bet_sizes ip,flop,allin") != "INVALID"){
            this->ui->flop_ip_allin->setChecked(true);
        }
        // TURN
        else if(getParams(one_line_content,"set_bet_sizes oop,turn,bet,") != "INVALID"){
            this->ui->turn_oop_bet->setText(getParams(one_line_content,"set_bet_sizes oop,turn,bet,").replace(',',' '));
        }
        else if(getParams(one_line_content,"set_bet_sizes oop,turn,raise") != "INVALID"){
            this->ui->turn_oop_raise->setText(getParams(one_line_content,"set_bet_sizes oop,turn,raise").replace(',',' '));
        }
        else if(getParams(one_line_content,"set_bet_sizes oop,turn,donk") != "INVALID"){
            this->ui->turn_oop_donk->setText(getParams(one_line_content,"set_bet_sizes oop,turn,donk").replace(',',' '));
        }
        else if(getParams(one_line_content,"set_bet_sizes oop,turn,allin") != "INVALID"){
            this->ui->turn_oop_allin->setChecked(true);
        }
        else if(getParams(one_line_content,"set_bet_sizes ip,turn,bet,") != "INVALID"){
            this->ui->turn_ip_bet->setText(getParams(one_line_content,"set_bet_sizes ip,turn,bet,").replace(',',' '));
        }
        else if(getParams(one_line_content,"set_bet_sizes ip,turn,raise") != "INVALID"){
            this->ui->turn_ip_raise->setText(getParams(one_line_content,"set_bet_sizes ip,turn,raise").replace(',',' '));
        }
        else if(getParams(one_line_content,"set_bet_sizes ip,turn,allin") != "INVALID"){
            this->ui->turn_ip_allin->setChecked(true);
        }
        // RIVER
        else if(getParams(one_line_content,"set_bet_sizes oop,river,bet,") != "INVALID"){
            this->ui->river_oop_bet->setText(getParams(one_line_content,"set_bet_sizes oop,river,bet,").replace(',',' '));
        }
        else if(getParams(one_line_content,"set_bet_sizes oop,river,raise") != "INVALID"){
            this->ui->river_oop_raise->setText(getParams(one_line_content,"set_bet_sizes oop,river,raise").replace(',',' '));
        }
        else if(getParams(one_line_content,"set_bet_sizes oop,river,donk") != "INVALID"){
            this->ui->river_oop_donk->setText(getParams(one_line_content,"set_bet_sizes oop,river,donk").replace(',',' '));
        }
        else if(getParams(one_line_content,"set_bet_sizes oop,river,allin") != "INVALID"){
            this->ui->river_oop_allin->setChecked(true);
        }
        else if(getParams(one_line_content,"set_bet_sizes ip,river,bet,") != "INVALID"){
            this->ui->river_ip_bet->setText(getParams(one_line_content,"set_bet_sizes ip,river,bet,").replace(',',' '));
        }
        else if(getParams(one_line_content,"set_bet_sizes ip,river,raise") != "INVALID"){
            this->ui->river_ip_raise->setText(getParams(one_line_content,"set_bet_sizes ip,river,raise").replace(',',' '));
        }
        else if(getParams(one_line_content,"set_bet_sizes ip,river,allin") != "INVALID"){
            this->ui->river_ip_allin->setChecked(true);
        }
        // OTHER PARAMS
        else if(getParams(one_line_content,"set_allin_threshold") != "INVALID"){
            this->ui->allinThresholdText->setText(getParams(one_line_content,"set_allin_threshold"));
        }
        else if(getParams(one_line_content,"set_thread_num") != "INVALID"){
            this->ui->threadsText->setText(getParams(one_line_content,"set_thread_num"));
        }
        else if(getParams(one_line_content,"set_accuracy") != "INVALID"){
            this->ui->exploitabilityText->setText(getParams(one_line_content,"set_accuracy"));
        }
        else if(getParams(one_line_content,"set_max_iteration") != "INVALID"){
            this->ui->iterationText->setText(getParams(one_line_content,"set_max_iteration"));
        }
        else if(getParams(one_line_content,"set_print_interval") != "INVALID"){
            this->ui->logIntervalText->setText(getParams(one_line_content,"set_print_interval"));
        }
        else if(getParams(one_line_content,"set_raise_limit") != "INVALID"){
            this->ui->raiseLimitText->setText(getParams(one_line_content,"set_raise_limit"));
        }
        else if(getParams(one_line_content,"set_use_isomorphism") != "INVALID"){
            if(getParams(one_line_content,"set_use_isomorphism") == "1"){
                this->ui->useIsoCheck->setChecked(true);
            }else{
                this->ui->useIsoCheck->setChecked(false);
            }
        }
    }
    this->update();
}

void MainWindow::on_actionimport_triggered(){
    QString fileName =  QFileDialog::getOpenFileName(
              this,
              tr("Open parameters file"),
              QDir::currentPath(),
              tr("Text files (*.txt)"));
    this->import_from_file(fileName);
    this->ui->IpRangeTableView->update();
    this->ui->oopRangeTableView->update();
    this->ui->IpRangeTableView->setFocus();
    this->ui->oopRangeTableView->setFocus();
}

void MainWindow::on_actionexport_triggered(){
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Parameters"),
                               "parameters/output_parameters.txt",
                               tr("Text file (*.txt)"));
    if(fileName.isNull())return;
    QString output_text = "";
    QTextStream out(&output_text);
    out << "set_pot " << this->ui->potText->text().trimmed();
    out << "\n";
    out << "set_effective_stack " << this->ui->effectiveStackText->text().trimmed();
    out << "\n";
    out << "set_board " << this->ui->boardText->toPlainText();
    out << "\n";
    out << "set_range_oop " << this->ui->oopRangeText->toPlainText();
    out << "\n";
    out << "set_range_ip " << this->ui->ipRangeText->toPlainText();
    out << "\n";
    // FLOP
    out << "set_bet_sizes oop,flop,bet," << this->ui->flop_oop_bet->text().trimmed().replace(' ',',');
    out << "\n";
    out << "set_bet_sizes oop,flop,raise," << this->ui->flop_oop_raise->text().trimmed().replace(' ',',');
    out << "\n";
    if(this->ui->flop_ip_allin->isChecked()){
        out << "set_bet_sizes oop,flop,allin" << "\n";
    }
    out << "set_bet_sizes ip,flop,bet," << this->ui->flop_ip_bet->text().trimmed().replace(' ',',');
    out << "\n";
    out << "set_bet_sizes ip,flop,raise," << this->ui->flop_ip_raise->text().trimmed().replace(' ',',');
    out << "\n";
    if(this->ui->flop_ip_allin->isChecked()){
        out << "set_bet_sizes ip,flop,allin" << "\n";
    }
    // TURN
    out << "set_bet_sizes oop,turn,bet," << this->ui->turn_oop_bet->text().trimmed().replace(' ',',');
    out << "\n";
    out << "set_bet_sizes oop,turn,raise," << this->ui->turn_oop_raise->text().trimmed().replace(' ',',');
    out << "\n";
    if(this->ui->turn_oop_donk->text().trimmed() != ""){
        out << "set_bet_sizes oop,turn,donk," << this->ui->turn_oop_donk->text().trimmed().replace(' ',',');
        out << "\n";
    }
    if(this->ui->turn_ip_allin->isChecked()){
        out << "set_bet_sizes oop,turn,allin" << "\n";
    }
    out << "set_bet_sizes ip,turn,bet," << this->ui->turn_ip_bet->text().trimmed().replace(' ',',');
    out << "\n";
    out << "set_bet_sizes ip,turn,raise," << this->ui->turn_ip_raise->text().trimmed().replace(' ',',');
    out << "\n";
    if(this->ui->turn_ip_allin->isChecked()){
        out << "set_bet_sizes ip,turn,allin" << "\n";
    }
    // RIVER
    out << "set_bet_sizes oop,river,bet," << this->ui->river_oop_bet->text().trimmed().replace(' ',',');
    out << "\n";
    out << "set_bet_sizes oop,river,raise," << this->ui->river_oop_raise->text().trimmed().replace(' ',',');
    out << "\n";
    if(this->ui->river_ip_allin->isChecked()){
        out << "set_bet_sizes oop,river,allin" << "\n";
    }
    out << "set_bet_sizes ip,river,bet," << this->ui->river_ip_bet->text().trimmed().replace(' ',',');
    out << "\n";
    out << "set_bet_sizes ip,river,raise," << this->ui->river_ip_raise->text().trimmed().replace(' ',',');
    out << "\n";
    if(this->ui->river_oop_donk->text().trimmed() != ""){
        out << "set_bet_sizes oop,river,donk," << this->ui->river_oop_donk->text().trimmed().replace(' ',',');
        out << "\n";
    }
    if(this->ui->river_ip_allin->isChecked()){
        out << "set_bet_sizes ip,river,allin" << "\n";
    }

    out << "set_allin_threshold " << this->ui->allinThresholdText->text().trimmed();
    out << "\n";
    out << "set_raise_limit " << this->ui->raiseLimitText->text().trimmed();
    out << "\n";
    out << "build_tree";
    out << "\n";
    out << "set_thread_num " << this->ui->threadsText->text().trimmed();
    out << "\n";
    out << "set_accuracy " << this->ui->exploitabilityText->text().trimmed();
    out << "\n";
    out << "set_max_iteration " << this->ui->iterationText->text().trimmed();
    out << "\n";
    out << "set_print_interval " << this->ui->logIntervalText->text().trimmed();
    out << "\n";
    if(this->ui->useIsoCheck->isChecked()){
        out << "set_use_isomorphism 1" << "\n";
    }else{
        out << "set_use_isomorphism 0" << "\n";
    }
    out << "start_solve";
    out << "\n";

    this->setWindowTitle(tr("Settings"));
    QSettings setting("TexasSolver", "Setting");
    setting.beginGroup("solver");
    int dump_round = setting.value("dump_round").toInt();
    out << "set_dump_rounds " << dump_round;
    out << "\n";
    out << "dump_result output_result.json";

    setlocale(LC_ALL,"");

    ofstream fileWriter;
    fileWriter.open(fileName.toLocal8Bit());
    QMessageBox msgBox;
    QString message;
    if(!fileWriter.fail()){
        fileWriter << output_text.toStdString();
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

void MainWindow::on_actionSettings_triggered(){
    this->settingEditor = new SettingEditor(this);
    settingEditor->setAttribute(Qt::WA_DeleteOnClose);
    settingEditor->show();
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

vector<float> sizes_convert(QString input){
    QStringList list = input.split(" ");
    vector<float> sizes;
    foreach(QString num, list){
        if(num.endsWith("x")){
            int pos = num.lastIndexOf(QChar('x'));
            num = num.left(pos);
            sizes.push_back(num.toFloat()  * 100);
        }
        else{
            sizes.push_back(num.toFloat());
        }
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
    qSolverJob->use_halffloats =  this->ui->useHalfFloats_box->currentIndex();

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
    switch(this->ui->useHalfFloats_box->currentIndex()){
    case 0:
        break;
    case 1:
        corh *= 0.75;
        break;
    case 2:
        corh *= 0.5;
        break;
    }
    float memory_mb = (float)memory_float / 1024 / 1024 * corh * 4 ;
    float memory_gb = (float)memory_float / 1024 / 1024 / 1024 * corh * 4;
    QString message;
    if(memory_gb == 0){
        message = tr("Please build tree first.");
    }else if(memory_gb < 1){
        message = tr("Estimated Memory Usage: ") + QString::number(memory_mb,'f',0) + tr(" Mb") +
                tr("\nRebuild tree to have changed optimization options take effect!");
    }else{
        message = tr("Estimated Memory Usage: ") + QString::number(memory_gb,'f',1) + tr(" Gb") +
                tr("\nRebuild tree to have changed optimization options take effect!");
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

void MainWindow::on_openParametersFolderButton_clicked()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(QDir::current().filePath("parameters")));
}

void MainWindow::item_clicked(const QModelIndex& index){
    if(!this->qFileSystemModel->isDir(index)){
        QFileInfo fileinfo = QFileInfo(this->qFileSystemModel->filePath(index));
        if(fileinfo.suffix() == "txt"){
            this->import_from_file(this->qFileSystemModel->filePath(index));
            this->ui->IpRangeTableView->update();
            this->ui->oopRangeTableView->update();
            this->ui->IpRangeTableView->setFocus();
            this->ui->oopRangeTableView->setFocus();
        }
    }
}

void MainWindow::on_exportCurrentParameterButton_clicked()
{
    this->on_actionexport_triggered();
}

void MainWindow::on_ipRangeText_textChanged()
{
    this->ip_model->setRangeText(this->ui->ipRangeText->toPlainText());
    this->ui->IpRangeTableView->update();
}

void MainWindow::on_oopRangeText_textChanged()
{
    this->oop_model->setRangeText(this->ui->oopRangeText->toPlainText());
    this->ui->oopRangeTableView->update();
}

void MainWindow::onExpanded(const QModelIndex& index){
    ui->parametersTreeView->resizeColumnToContents(0);
}
