//
// Created by Xuefeng Huang on 2020/2/6.
//

#include "runtime/PokerSolver.h"

PokerSolver::PokerSolver() {

}

PokerSolver::PokerSolver(string ranks, string suits, string compairer_file,int compairer_file_lines) {
    vector<string> ranks_vector = string_split(ranks,',');
    vector<string> suits_vector = string_split(suits,',');
    this->deck = Deck(ranks_vector,suits_vector);
    this->compairer = make_shared<Dic5Compairer>(compairer_file,compairer_file_lines);
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

void PokerSolver::train(string p1_range, string p2_range, string boards, string log_file, int iteration_number,
                        int print_interval, string algorithm,int warmup,float accuracy,bool use_isomorphism,int threads) {
    string player1RangeStr = p1_range;
    string player2RangeStr = p2_range;

    vector<string> board_str_arr = string_split(boards,',');
    vector<int> initialBoard;
    for(string one_board_str:board_str_arr){
        initialBoard.push_back(Card::strCard2int(one_board_str));
    }

    vector<PrivateCards> player1Range = PrivateRangeConverter::rangeStr2Cards(player1RangeStr,initialBoard);
    vector<PrivateCards> player2Range = PrivateRangeConverter::rangeStr2Cards(player2RangeStr,initialBoard);
    string logfile_name = log_file;
    this->solver = make_shared<PCfrSolver>(
            game_tree
            , player1Range
            , player2Range
            , initialBoard
            , compairer
            , deck
            , iteration_number
            , false
            , print_interval
            , logfile_name
            , algorithm
            , Solver::MonteCarolAlg::NONE
            , warmup
            , accuracy
            , use_isomorphism
            , threads
    );
    this->solver->train();
}

void PokerSolver::dump_strategy(string dump_file,int dump_rounds) {
    json dump_json = this->solver->dumps(false,dump_rounds);
    ofstream fileWriter;
    fileWriter.open(dump_file);
    fileWriter << dump_json;
    fileWriter.flush();
    fileWriter.close();
}

const shared_ptr<GameTree> &PokerSolver::getGameTree() const {
    return game_tree;
}
