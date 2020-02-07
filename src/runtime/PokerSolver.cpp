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

void PokerSolver::train(string p1_range, string p2_range, string boards, string log_file, int iteration_number,
                        int print_interval, string algorithm,int threads) {
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
    PCfrSolver solver = PCfrSolver(
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
            , threads
    );
    solver.train();

}
