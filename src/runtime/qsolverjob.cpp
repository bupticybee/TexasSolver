#include "include/runtime/qsolverjob.h"


using namespace std;

void QSolverJob:: setContext(QSTextEdit * textEdit){
    this->textEdit = textEdit;
    //QDebugStream qout(qDebug().noquote(), this->textEdit);

}

PokerSolver* QSolverJob::get_solver(){
    if(this->mode == PokerMode::HOLDEM){
        return &this->ps_holdem;
    }else if(this->mode == PokerMode::SHORTDECK){
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
        // qDebug().noquote() << tr("Encountering error:");//.toStdString() << endl;
        // qDebug().noquote() << error.what() << "\n";
        logger->log("Encountering error:\n%s\n", error.what());
    }
}

void QSolverJob::loading(){
    /*
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
    */
    resource_dir =  "./resources";
    // qDebug().noquote() << tr("Loading holdem compairing file");//.toStdString() << endl;
    logger->log("Loading holdem compairing file");
    ps_holdem = PokerSolver(PokerMode::HOLDEM, resource_dir);
    // qDebug().noquote() << tr("Loading shortdeck compairing file");//.toStdString() << endl;
    logger->log("Loading shortdeck compairing file");
    ps_shortdeck = PokerSolver(PokerMode::SHORTDECK, resource_dir);
    // qDebug().noquote() << tr("Loading finished. Good to go.");//.toStdString() << endl;
    logger->log("Loading finished. Good to go.");
    ps_holdem.logger = logger;
    ps_shortdeck.logger = logger;
}


void QSolverJob::saving(){
    // qDebug().noquote() << tr("Saving json file..");//.toStdString() << std::endl;
    logger->log("Saving json file..");
    /*
    QSettings setting("TexasSolver", "Setting");
    setting.beginGroup("solver");
    this->dump_rounds = setting.value("dump_round").toInt();
    */
    // qDebug().noquote() << tr("Dump round: ") << clt->dump_rounds;
    logger->log("Dump round: %d", clt->dump_rounds);
    if(clt->dump_rounds == 3){
        // qDebug().noquote() << tr("This could be slow, or even blow your RAM, dump to river is not well optimized :(");
        logger->log("This could be slow, or even blow your RAM, dump to river is not well optimized");
    }
    if(this->mode == PokerMode::HOLDEM){
        this->ps_holdem.dump_strategy(clt->res_file, clt->dump_rounds);
    }else if(this->mode == PokerMode::SHORTDECK){
        this->ps_shortdeck.dump_strategy(clt->res_file, clt->dump_rounds);
    }
    // qDebug().noquote() << tr("Saving done.");//.toStdString() << std::endl;
    logger->log("Saving done.");
}

void QSolverJob::stop(){
    // qDebug().noquote() << tr("Trying to stop solver.");
    logger->log("Trying to stop solver.");
    if(this->mode == PokerMode::HOLDEM){
        this->ps_holdem.stop();
    }else if(this->mode == PokerMode::SHORTDECK){
        this->ps_shortdeck.stop();
    }
}

void QSolverJob::solving(){
    // TODO  为什么ui上多次求解会积累memory？哪里leak了？
    // TODO  为什么有时候会莫名闪退？
    // qDebug().noquote() << tr("Start Solving..");//.toStdString() << std::endl;
    logger->log("Start Solving..");
    PokerSolver *ps = (mode == PokerMode::HOLDEM ? &ps_holdem : &ps_shortdeck);
    clt->start_solve(ps);
    /*
    if(this->mode == PokerMode::HOLDEM){
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
    }else if(this->mode == PokerMode::SHORTDECK){
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
    */
    // qDebug().noquote() << tr("Solving done.");//.toStdString() << std::endl;
    logger->log("Solving done.");
}

long long QSolverJob::estimate_tree_memory(string &range1, string &range2, string &board) {
    // qDebug().noquote() << tr("Estimating tree memory..");//.toStdString() << endl;
    logger->log("Estimating tree memory..");
    if(this->mode == PokerMode::HOLDEM){
        return ps_holdem.estimate_tree_memory(range1, range2, board);
    }else if(this->mode == PokerMode::SHORTDECK){
        return ps_shortdeck.estimate_tree_memory(range1, range2, board);
    }
    return 0;
}

void QSolverJob::build_tree(){
    // qDebug().noquote() << tr("building tree..");//.toStdString() << endl;
    logger->log("building tree..");
    PokerSolver *ps = (mode == PokerMode::HOLDEM ? &ps_holdem : &ps_shortdeck);
    clt->build_tree(ps);
    /*
    if(this->mode == PokerMode::HOLDEM){
        ps_holdem.build_game_tree(oop_commit,ip_commit,current_round,raise_limit,small_blind,big_blind,stack,*gtbs.get(),allin_threshold);
    }else if(this->mode == PokerMode::SHORTDECK){
        ps_shortdeck.build_game_tree(oop_commit,ip_commit,current_round,raise_limit,small_blind,big_blind,stack,*gtbs.get(),allin_threshold);
    }
    */
    // qDebug().noquote() << tr("build tree finished");//.toStdString() << endl;
    logger->log("build tree finished");
}
