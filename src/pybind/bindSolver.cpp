//
// Created by Xuefeng Huang on 2020/2/7.
//
#include <pybind11/pybind11.h>
#include "runtime/PokerSolver.h"


int cfr_test(int n_threads) {
    shared_ptr<Dic5Compairer> compairer;
    compairer = make_shared<Dic5Compairer>("../resources/compairer/card5_dic_sorted_shortdeck.txt",376993);
    vector<string> ranks = {"A", "K", "Q", "J", "T", "9", "8", "7", "6"};
    vector<string> suits = {"h", "s", "d", "c"};
    Deck deck = Deck(
            ranks,suits
    );
    shared_ptr<GameTree> game_tree = make_shared<GameTree>("../resources/gametree/part_tree_turn_depthinf.km",deck);
    string player1RangeStr = "AA,KK,QQ,JJ,TT,99,88,77,66,AK,AQ,AJ,AT,A9,A8,A7,A6,KQ,KJ,KT,K9,K8,K7,K6,QJ,QT,Q9,Q8,Q7,Q6,JT,J9,J8,J7,J6,T9,T8,T7,T6,98,97,96,87,86,76";
    string player2RangeStr = "AA,KK,QQ,JJ,TT,99,88,77,66,AK,AQ,AJ,AT,A9,A8,A7,A6,KQ,KJ,KT,K9,K8,K7,K6,QJ,QT,Q9,Q8,Q7,Q6,JT,J9,J8,J7,J6,T9,T8,T7,T6,98,97,96,87,86,76";

    vector<int> initialBoard = vector<int>{
            Card::strCard2int("Kd"),
            Card::strCard2int("Jd"),
            Card::strCard2int("Td"),
            Card::strCard2int("7s"),
    };

    vector<PrivateCards> player1Range = PrivateRangeConverter::rangeStr2Cards(player1RangeStr,initialBoard);
    vector<PrivateCards> player2Range = PrivateRangeConverter::rangeStr2Cards(player2RangeStr,initialBoard);
    string logfile_name = "../resources/outputs/outputs_log.txt";
    PCfrSolver solver = PCfrSolver(
            game_tree
            , player1Range
            , player2Range
            , initialBoard
            , compairer
            , deck
            , 100
            , false
            , 10
            , logfile_name
            , "discounted_cfr"
            , Solver::MonteCarolAlg::NONE
            , n_threads
    );
    solver.train();
    return n_threads;
}

namespace py = pybind11;

PYBIND11_MODULE(bindSolver, m) {
    m.doc() = R"pbdoc(
        Pybind11 example plugin
        -----------------------
        .. currentmodule:: cmake_example
        .. autosummary::
           :toctree: _generate
           add
           subtract
    )pbdoc";

    m.def("cfr_test", &cfr_test, R"pbdoc(test thr cfr algorithm)pbdoc");

    py::class_<PokerSolver>(m, "PokerSolver")
            .def(py::init<std::string,std::string,std::string,int>())
            .def("load_game_tree", &PokerSolver::load_game_tree)
            .def("train", &PokerSolver::train)
            .def("dump_strategy", &PokerSolver::dump_strategy);


#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}

