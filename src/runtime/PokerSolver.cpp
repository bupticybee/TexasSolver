﻿//
// Created by Xuefeng Huang on 2020/2/6.
//

#include "include/runtime/PokerSolver.h"
#ifdef USE_CUDA
#include "include/solver/cuda_cfr.h"
#endif

PokerSolver::PokerSolver(PokerMode mode, string &resource_dir) {
    string suits = "c,d,h,s";
    string ranks;
    string compairer_file, compairer_file_bin;
    int lines;
    if(mode == PokerMode::HOLDEM){
        ranks = "2,3,4,5,6,7,8,9,T,J,Q,K,A";
        compairer_file = resource_dir + "/compairer/card5_dic_sorted.txt";
        compairer_file_bin = resource_dir + "/compairer/card5_dic_zipped.bin";
        lines = 2598961;
    }else if(mode == PokerMode::SHORTDECK){
        ranks = "6,7,8,9,T,J,Q,K,A";
        compairer_file = resource_dir + "/compairer/card5_dic_sorted_shortdeck.txt";
        compairer_file_bin = resource_dir + "/compairer/card5_dic_zipped_shortdeck.bin";
        lines = 376993;
    }else{
        throw runtime_error(tfm::format("mode not recognized : ",mode));
    }
    init(ranks, suits, compairer_file, lines, compairer_file_bin);
}

PokerSolver::PokerSolver(string &ranks, string &suits, string &compairer_file, int compairer_file_lines, string &compairer_file_bin) {
    init(ranks, suits, compairer_file, compairer_file_lines, compairer_file_bin);
}
void PokerSolver::init(string &ranks, string &suits, string &compairer_file, int compairer_file_lines, string &compairer_file_bin) {
    vector<string> ranks_vector = string_split(ranks,',');
    vector<string> suits_vector = string_split(suits,',');
    this->deck = Deck(ranks_vector,suits_vector);
    this->compairer = make_shared<Dic5Compairer>(compairer_file,compairer_file_lines,compairer_file_bin);
}

void PokerSolver::load_game_tree(string game_tree_file) {
    shared_ptr<GameTree> game_tree = make_shared<GameTree>(game_tree_file,this->deck);
    this->game_tree = game_tree;
}

void PokerSolver::build_game_tree(
        float oop_commit,
        float ip_commit,
        int current_round,
        int raise_limit,
        float small_blind,
        float big_blind,
        float stack,
        GameTreeBuildingSettings buildingSettings,
        float allin_threshold
){
    shared_ptr<GameTree> game_tree = make_shared<GameTree>(
            this->deck,
            oop_commit,
            ip_commit,
            current_round,
            raise_limit,
            small_blind,
            big_blind,
            stack,
            buildingSettings,
            allin_threshold
    );
    this->game_tree = game_tree;
}

vector<PrivateCards> noDuplicateRange(const vector<PrivateCards> &private_range, uint64_t board_long) {
    vector<PrivateCards> range_array;
    unordered_map<int,bool> rangekv;
    for(PrivateCards one_range:private_range){
        if(rangekv.find(one_range.hashCode()) != rangekv.end())
            throw runtime_error(tfm::format("duplicated key %s",one_range.toString()));
        rangekv[one_range.hashCode()] = true;
        uint64_t hand_long = Card::boardInts2long(one_range.get_hands());
        if(!Card::boardsHasIntercept(hand_long,board_long)){
            range_array.push_back(one_range);
        }
    }
    return range_array;
}

void PokerSolver::stop(){
    if(this->solver != nullptr){
        this->solver->stop();
    }
}

long long PokerSolver::estimate_tree_memory(string &p1_range, string &p2_range, string &board){
    if(this->game_tree == nullptr){
        // qDebug().noquote() << QObject::tr("Please buld tree first.");
        logger->log("Please buld tree first.");
        return 0;
    }
    else{
        vector<string> board_str_arr = string_split(board,',');
        vector<int> initialBoard;
        for(string one_board_str:board_str_arr){
            initialBoard.push_back(Card::strCard2int(one_board_str));
        }

        vector<PrivateCards> range1 = PrivateRangeConverter::rangeStr2Cards(p1_range,initialBoard);
        vector<PrivateCards> range2 = PrivateRangeConverter::rangeStr2Cards(p2_range,initialBoard);
        return this->game_tree->estimate_tree_memory(this->deck.getCards().size() - initialBoard.size(),range1.size(),range2.size());
    }
}

void PokerSolver::train(string &p1_range, string &p2_range, string &boards, /*string &log_file,*/ int iteration_number,
                        int print_interval, string &algorithm,int warmup,float accuracy,bool use_isomorphism, int use_halffloats, int threads, int slice_cfr) {
    if(game_tree == nullptr) {
        logger->log("Please buld tree first.");
        return;
    }
    string player1RangeStr = p1_range;
    string player2RangeStr = p2_range;

    vector<string> board_str_arr = string_split(boards,',');
    vector<int> initialBoard;
    for(string one_board_str:board_str_arr){
        initialBoard.push_back(Card::strCard2int(one_board_str));
    }

    vector<PrivateCards> range1 = PrivateRangeConverter::rangeStr2Cards(player1RangeStr,initialBoard);
    vector<PrivateCards> range2 = PrivateRangeConverter::rangeStr2Cards(player2RangeStr,initialBoard);
    uint64_t initial_board_long = Card::boardInts2long(initialBoard);

    this->player1Range = noDuplicateRange(range1,initial_board_long);
    this->player2Range = noDuplicateRange(range2,initial_board_long);

    // string logfile_name = log_file;
    if(solver) solver.reset();// 释放内存
    try {
        if(slice_cfr == 1) {
            solver = make_shared<SliceCFR>(game_tree, range1, range2, initialBoard, compairer, deck, iteration_number, print_interval, accuracy, threads, logger);
        }
        else if(slice_cfr == 2) {
#ifdef USE_CUDA
            solver = make_shared<CudaCFR>(game_tree, range1, range2, initialBoard, compairer, deck, iteration_number, print_interval, accuracy, threads, logger);
#else
            logger->log("please set USE_CUDA ON in CMakeLists.txt and rebuild project");
            return;
#endif
        }
        else {
            solver = make_shared<PCfrSolver>(
                    game_tree
                    , range1
                    , range2
                    , initialBoard
                    , compairer
                    , deck
                    , iteration_number
                    , false
                    , print_interval
                    , /*logfile_name*/logger
                    , algorithm
                    , Solver::MonteCarolAlg::NONE
                    , warmup
                    , accuracy
                    , use_isomorphism
                    , use_halffloats
                    , threads
            );
        }
        solver->train();
    }
    catch(std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}

void PokerSolver::dump_strategy(string &dump_file, int dump_rounds) {
    //locale &loc=locale::global(locale(locale(),"",LC_CTYPE));
    setlocale(LC_ALL,"");

    json dump_json = this->solver->dumps(false,dump_rounds);
    //QFile ofile( QString::fromStdString(dump_file));
    ofstream fileWriter;
    fileWriter.open(dump_file);
    if(!fileWriter.fail()){
        fileWriter << dump_json;
        fileWriter.flush();
        fileWriter.close();
        // qDebug().noquote() << QObject::tr("save success");
        logger->log("save success");
    }else{
        // qDebug().noquote() << QObject::tr("save failed, file cannot be open");
        logger->log("save failed, file cannot be open");
    }
    setlocale(LC_CTYPE, "C");
}

const shared_ptr<GameTree> &PokerSolver::getGameTree() const {
    return game_tree;
}
