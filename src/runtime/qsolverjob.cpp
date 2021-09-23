#include "include/runtime/qsolverjob.h"


using namespace std;

void QSolverJob:: setContext(QTextEdit * textEdit){
    this->textEdit = textEdit;
    QDebugStream qout(std::cout, this->textEdit);

}

void QSolverJob::run()
{
    QDebugStream qout(std::cout, this->textEdit);
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

void QSolverJob::loading(){
    string suits = "c,d,h,s";
    string ranks;
    this->resource_dir =  "/Users/bytedance/TexasSolverGui/resources";
    string compairer_file;
    int lines;
    string logfile_name = "./outputs_log.txt";
    cout << tr("Loading holdem compairing file").toStdString() << endl;
    //if(mode == "holdem"){
    ranks = "2,3,4,5,6,7,8,9,T,J,Q,K,A";
    compairer_file = this->resource_dir + "/compairer/card5_dic_sorted.txt";
    lines = 2598961;
    this->ps_holdem = PokerSolver(ranks,suits,compairer_file,lines);

    cout << tr("Loading shortdeck compairing file").toStdString() << endl;
    //}else if(mode == "shortdeck"){
    ranks = "6,7,8,9,T,J,Q,K,A";
    compairer_file = this->resource_dir + "/compairer/card5_dic_sorted_shortdeck.txt";
    lines = 376993;
    this->ps_shortdeck = PokerSolver(ranks,suits,compairer_file,lines);
    cout << tr("Loading finished. Good to go.").toStdString() << endl;
}


void QSolverJob::saving(){
    std::cout << tr("Saving json file..").toStdString() << std::endl;
    if(this->mode == Mode::HOLDEM){
        this->ps_holdem.dump_strategy(this->savefile,this->dump_rounds);
    }else if(this->mode == Mode::SHORTDECK){
        this->ps_shortdeck.dump_strategy(this->savefile,this->dump_rounds);
    }
    std::cout << tr("Saving done.").toStdString() << std::endl;
}

void QSolverJob::solving(){
    // TODO  为什么ui上多次求解会积累memory？哪里leak了？
    std::cout << tr("Start Solving..").toStdString() << std::endl;

    if(this->mode == Mode::HOLDEM){
        this->ps_holdem.train(
            this->range_ip,
            this->range_oop,
            this->board,
            "tmp_log.txt",
            max_iteration,
            this->print_interval,
            "discounted_cfr",
            -1,
            this->accuracy,
            this->use_isomorphism,
            this->thread_number
        );
    }else if(this->mode == Mode::SHORTDECK){
        this->ps_shortdeck.train(
            this->range_ip,
            this->range_oop,
            this->board,
            "tmp_log.txt",
            max_iteration,
            this->print_interval,
            "discounted_cfr",
            -1,
            this->accuracy,
            this->use_isomorphism,
            this->thread_number
        );
    }
    std::cout << tr("Solving done.").toStdString() << std::endl;
}

void QSolverJob::build_tree(){
    cout << tr("building tree..").toStdString() << endl;
    if(this->mode == Mode::HOLDEM){
        ps_holdem.build_game_tree(oop_commit,ip_commit,current_round,raise_limit,small_blind,big_blind,stack,*gtbs.get(),allin_threshold);
    }else if(this->mode == Mode::SHORTDECK){
        ps_shortdeck.build_game_tree(oop_commit,ip_commit,current_round,raise_limit,small_blind,big_blind,stack,*gtbs.get(),allin_threshold);
    }
    cout << tr("build tree finished").toStdString() << endl;
}
