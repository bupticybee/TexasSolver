#include "include/runtime/qsolverjob.h"


using namespace std;

void QSolverJob:: setContext(QSTextEdit * textEdit){
    this->textEdit = textEdit;
    //QDebugStream qout(qDebug().noquote(), this->textEdit);

}

PokerSolver* QSolverJob::get_solver(){
    if(this->mode == Mode::HOLDEM){
        return &this->ps_holdem;
    }else if(this->mode == Mode::SHORTDECK){
        return &this->ps_shortdeck;
    }else throw runtime_error("unknown mode in get_solver");
}

void QSolverJob::run()
{
    //QDebugStream qout(qDebug().noquote(), this->textEdit);
    try{
        if(this->current_mission == MissionType::SOLVING){
            this->solving();
        }else if(this->current_mission == MissionType::LOADING){
            this->loading();
        }else if(this->current_mission == MissionType::BUILDTREE){
            this->build_tree();
        }else if(this->current_mission == MissionType::SAVING){
            this->saving();
        }else{
            throw runtime_error("unsupported mission type");
        }
    }
    catch (const runtime_error& error)
    {
        qDebug().noquote() << tr("Encountering error:");//.toStdString() << endl;
        qDebug().noquote() << error.what() << "\n";
    }
}

void QSolverJob::loading(){
    string suits = "c,d,h,s";
    string ranks;
    this->resource_dir =  ":/resources";
    string compairer_file, compairer_file_bin;
    int lines;
    qDebug().noquote() << tr("Loading holdem compairing file");//.toStdString() << endl;
    //if(mode == "holdem"){
    ranks = "2,3,4,5,6,7,8,9,T,J,Q,K,A";
    compairer_file = this->resource_dir + "/compairer/card5_dic_sorted.txt";
    compairer_file_bin = this->resource_dir + "/compairer/card5_dic_zipped.bin";
    //qDebug().noquote() << compairer_file_bin.c_str();
    lines = 2598961;
    this->ps_holdem = PokerSolver(ranks,suits,compairer_file,lines,compairer_file_bin);

    qDebug().noquote() << tr("Loading shortdeck compairing file");//.toStdString() << endl;
    //}else if(mode == "shortdeck"){
    ranks = "6,7,8,9,T,J,Q,K,A";
    compairer_file = this->resource_dir + "/compairer/card5_dic_sorted_shortdeck.txt";
    compairer_file_bin = this->resource_dir + "/compairer/card5_dic_zipped_shortdeck.bin";
    lines = 376993;
    this->ps_shortdeck = PokerSolver(ranks,suits,compairer_file,lines,compairer_file_bin);
    qDebug().noquote() << tr("Loading finished. Good to go.");//.toStdString() << endl;
}


void QSolverJob::saving(){
    qDebug().noquote() << tr("Saving json file..");//.toStdString() << std::endl;

    QSettings setting("TexasSolver", "Setting");
    setting.beginGroup("solver");
    this->dump_rounds = setting.value("dump_round").toInt();

    qDebug().noquote() << tr("Dump round: ") << this->dump_rounds;
    if(this->dump_rounds == 3){
        qDebug().noquote() << tr("This could be slow, or even blow your RAM, dump to river is not well optimized :(");
    }
    if(this->mode == Mode::HOLDEM){
        this->ps_holdem.dump_strategy(this->savefile,this->dump_rounds);
    }else if(this->mode == Mode::SHORTDECK){
        this->ps_shortdeck.dump_strategy(this->savefile,this->dump_rounds);
    }
    qDebug().noquote() << tr("Saving done.");//.toStdString() << std::endl;
}

void QSolverJob::stop(){
    qDebug().noquote() << tr("Trying to stop solver.");
    if(this->mode == Mode::HOLDEM){
        this->ps_holdem.stop();
    }else if(this->mode == Mode::SHORTDECK){
        this->ps_shortdeck.stop();
    }
}

void QSolverJob::solving(){
    // TODO  为什么ui上多次求解会积累memory？哪里leak了？
    // TODO  为什么有时候会莫名闪退？
    qDebug().noquote() << tr("Start Solving..");//.toStdString() << std::endl;

    if(this->mode == Mode::HOLDEM){
        this->ps_holdem.train(
            this->range_ip,
            this->range_oop,
            this->board,
            "",
            max_iteration,
            this->print_interval,
            "discounted_cfr",
            -1,
            this->accuracy,
            this->use_isomorphism,
            this->use_halffloats,
            this->thread_number
        );
    }else if(this->mode == Mode::SHORTDECK){
        this->ps_shortdeck.train(
            this->range_ip,
            this->range_oop,
            this->board,
            "",
            max_iteration,
            this->print_interval,
            "discounted_cfr",
            -1,
            this->accuracy,
            this->use_isomorphism,
            this->use_halffloats,
            this->thread_number
        );
    }
    qDebug().noquote() << tr("Solving done.");//.toStdString() << std::endl;
}

long long QSolverJob::estimate_tree_memory(QString range1,QString range2,QString board){
    qDebug().noquote() << tr("Estimating tree memory..");//.toStdString() << endl;
    if(this->mode == Mode::HOLDEM){
        return ps_holdem.estimate_tree_memory(range1,range2,board);
    }else if(this->mode == Mode::SHORTDECK){
        return ps_shortdeck.estimate_tree_memory(range1,range2,board);
    }
    return 0;
}

void QSolverJob::build_tree(){
    qDebug().noquote() << tr("building tree..");//.toStdString() << endl;
    if(this->mode == Mode::HOLDEM){
        ps_holdem.build_game_tree(oop_commit,ip_commit,current_round,raise_limit,small_blind,big_blind,stack,*gtbs.get(),allin_threshold);
    }else if(this->mode == Mode::SHORTDECK){
        ps_shortdeck.build_game_tree(oop_commit,ip_commit,current_round,raise_limit,small_blind,big_blind,stack,*gtbs.get(),allin_threshold);
    }
    qDebug().noquote() << tr("build tree finished");//.toStdString() << endl;
}
