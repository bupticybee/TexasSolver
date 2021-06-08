//
// Created by Xuefeng Huang on 2020/1/28.
//
#include <iostream>
#include <GameTree.h>
#include <ranges/PrivateCards.h>
#include "gtest/gtest.h"
#include "Card.h"
#include "fmt/format.h"
#include "compairer/Dic5Compairer.h"
#include "tools/PrivateRangeConverter.h"
#include "solver/CfrSolver.h"
#include "solver/PCfrSolver.h"
#include "runtime/PokerSolver.h"
#include "experimental/TCfrSolver.h"
#include "tools/CommandLineTool.h"
//#include <boost/version.hpp>

using namespace std;
static shared_ptr<Dic5Compairer> compairer;

/*
TEST(TestCase,test_boost_env){
    cout << "Boost版本：" << BOOST_VERSION << endl;
}
*/

/*
TEST(TestCase,test_card ){
    Card card8h("8h");
    int card8h2int = Card::card2int(card8h);
    Card card8h2int2card = Card(Card::intCard2Str(card8h2int));
    int card8h2int2card2int = Card::card2int(card8h2int2card);
    cout << fmt::format("8h int-1 : {} int-2 : {}",card8h2int,card8h2int2card2int) << endl;
    EXPECT_EQ(card8h2int,card8h2int2card2int);
    EXPECT_EQ(card8h.toString(),card8h2int2card.toString());

    Card cardJh("Jh");
    int cardJh2int = Card::card2int(cardJh);
    Card cardJh2int2card = Card(Card::intCard2Str(cardJh2int));
    int cardJh2int2card2int = Card::card2int(cardJh2int2card);
    cout << fmt::format("Jh int-1 : {} int-2 : {}",cardJh2int,cardJh2int2card2int) << endl;
    EXPECT_EQ(cardJh2int,cardJh2int2card2int);
    EXPECT_EQ(cardJh.toString(),cardJh2int2card.toString());

    EXPECT_NE(card8h.toString(),cardJh.toString());
    EXPECT_NE(Card::card2int(card8h),Card::card2int(cardJh));
}

TEST(TestCase,test_card_convert ) {
    vector<Card> board = {
            Card("6c"),
            Card("6d"),
            Card("7c"),
            Card("7d"),
            Card("8s"),
            Card("6h"),
            Card("7s")
    };
    uint64_t board_int = Card::boardCards2long(board);
    vector<Card> board_cards = Card::long2boardCards(board_int);
    uint64_t board_int_rev = Card::boardCards2long(board_cards);

    for(Card i:board)
        cout << (i.getCard()) << endl;
    cout << endl;
    for(Card i:board_cards)
        cout << (i.getCard()) << endl;

    cout << board_int << endl;
    cout << board_int_rev << endl;
    EXPECT_EQ(board_int,board_int_rev);
}

TEST(TestCase,test_card_convert_ne ) {
    vector<Card> board1 = {
            Card("6c"),
            Card("6d"),
            Card("7c"),
            Card("7d"),
            Card("8s"),
            Card("6h"),
            Card("7s")
    };
    vector<Card> board2 = {
            Card("6c"),
            Card("6d"),
            Card("7c"),
            Card("7d"),
            Card("9s"),
            Card("6h"),
            Card("7s")
    };
    uint64_t board_int1 = Card::boardCards2long(board1);
    uint64_t board_int2 = Card::boardCards2long(board2);
    EXPECT_NE(board_int1,board_int2);
}

TEST(TestCase,test_comapirer_load ){
    compairer = make_shared<Dic5Compairer>("../resources/compairer/card5_dic_sorted_shortdeck.txt",376993);
}

TEST(TestCase,test_compairer_lg){
    vector<Card> board = {
            Card("6c"),
            Card("6d"),
            Card("7c"),
            Card("7d"),
            Card("8s")
    };
    vector<Card> private1 = {
            Card("6h"),
            Card("6s")
    };
    vector<Card> private2 = {
            Card("9c"),
            Card("9s")
    };

    Compairer::CompairResult cr = compairer->compair(private1,private2,board);

    EXPECT_EQ(cr,Compairer::CompairResult::LARGER);
}


TEST(TestCase,test_compairer_equivlent){
    vector<Card> board1_public = {
            Card("6c"),
            Card("6d"),
            Card("7c"),
            Card("7d"),
            Card("8s")
    };
    vector<Card> board1_private = {
            Card("6h"),
            Card("7s")
    };
    vector<int> board2_public = {
            (Card("6c").getCardInt()),
            (Card("6d").getCardInt()),
            (Card("7c").getCardInt()),
            (Card("7d").getCardInt()),
            (Card("8s").getCardInt()),
    };
    vector<int> board2_private = {
            (Card("6h").getCardInt()),
            (Card("7s").getCardInt())
    };
    uint64_t board_int1 = compairer->get_rank(board1_private,board1_public);
    uint64_t board_int2 = compairer->get_rank(board2_private,board2_public);
    cout << (board_int1) << endl;
    cout << (board_int2) << endl;
    EXPECT_EQ(board_int1,board_int2);

}

TEST(TestCase,test_compairer_eq){
    vector<Card> board = {
            Card("6c"),
            Card("6d"),
            Card("7c"),
            Card("7d"),
            Card("8s")
    };
    vector<Card> private1 = {
            Card("8h"),
            Card("7s")
    };
    vector<Card> private2 = {
            Card("8d"),
            Card("7h")
    };

    Compairer::CompairResult cr = compairer->compair(private1,private2,board);

    EXPECT_EQ(cr,Compairer::CompairResult::EQUAL);
}

TEST(TestCase,test_compairer_sm){
    vector<Card> board = {
            Card("6c"),
            Card("6d"),
            Card("7c"),
            Card("7d"),
            Card("8s")
    };
    vector<Card> private1 = {
            Card("6h"),
            Card("7s")
    };
    vector<Card> private2 = {
            Card("8d"),
            Card("7h")
    };

    Compairer::CompairResult cr = compairer->compair(private1,private2,board);

    EXPECT_EQ(cr,Compairer::CompairResult::SMALLER);
}

TEST(TestCase,test_compairer_get_rank){
    vector<Card> board = {
            Card("8d"),
            Card("9d"),
            Card("9s"),
            Card("Jd"),
            Card("Jh")
    };
    vector<Card> private_cards = {
            Card("6h"),
            Card("7s")
    };

    int rank = compairer->get_rank(private_cards,board);

    EXPECT_EQ(rank,687);
}

TEST(TestCase,test_tree_print_simple){
    vector<string> ranks = {"A", "K", "Q", "J", "T", "9", "8", "7", "6"};
    vector<string> suits = {"h", "s", "d", "c"};
    Deck deck = Deck(
            ranks,suits
    );
    GameTree game_tree = GameTree("../resources/gametree/simple_part_tree_depthinf.km",deck);
    game_tree.printTree(-1);
}

TEST(TestCase,test_tree_print_complex){
    vector<string> ranks = {"A", "K", "Q", "J", "T", "9", "8", "7", "6"};
    vector<string> suits = {"h", "s", "d", "c"};
    Deck deck = Deck(
            ranks,suits
    );
    GameTree game_tree = GameTree("../resources/gametree/part_tree_depthinf.km",deck);
    game_tree.printTree(2);
}

TEST(TestCase,test_converter_simple){
    vector<int> initialBoard {
            Card::strCard2int("Kd"),
            Card::strCard2int("Jd"),
            Card::strCard2int("Td"),
    };
    string range = "AA,KK,QQ,JJ,TT,99,88,77,66,AK,AQ,AJ,AT,A9,A8,A7,A6,KQ,KJ,KT,K9,K8,K7,K6,QJ,QT,Q9,Q8,Q7,Q6,JT,J9,J8,J7,J6,T9,T8,T7,T6,98,97,96,87,86,76";
    vector<PrivateCards> range_converted = PrivateRangeConverter::rangeStr2Cards(range,initialBoard);
    for(PrivateCards one_private_range:range_converted)
        EXPECT_EQ(one_private_range.weight,1);
}

TEST(TestCase,test_converter_withweight){
    vector<int> initialBoard {
            Card::strCard2int("Kd"),
            Card::strCard2int("Jd"),
            Card::strCard2int("Td"),
    };
    string range = "AA:2,KK:1.5,QQ:0,JJ,TT,99,88,77,66,AK,AQ,AJ,AT,A9,A8,A7,A6,KQ,KJ,KT,K9,K8,K7,K6,QJ,QT,Q9,Q8,Q7,Q6,JT,J9,J8,J7,J6,T9,T8,T7,T6,98,97,96,87,86,76";
    vector<PrivateCards> range_converted = PrivateRangeConverter::rangeStr2Cards(range,initialBoard);
    for(PrivateCards one_private_range:range_converted) {
        EXPECT_NE(one_private_range.weight, 0);
        if(one_private_range.toString() == "AhAs") {
            EXPECT_EQ(one_private_range.weight, 2);
        }else if(one_private_range.toString() == "KhKs") {
            EXPECT_EQ(one_private_range.weight, 1.5);
        }else if(one_private_range.toString() == "AhKs"){
            EXPECT_EQ(one_private_range.weight, 1);
        }
    }
}

TEST(TestCase,test_converter_o){
    vector<int> initialBoard {
            Card::strCard2int("Kd"),
            Card::strCard2int("Jd"),
            Card::strCard2int("Td"),
    };
    string range = "AAo:1,AKs:10";
    vector<PrivateCards> range_converted = PrivateRangeConverter::rangeStr2Cards(range,initialBoard);
    for(PrivateCards one_private_range:range_converted) {
        cout << one_private_range.toString() << " " << one_private_range.weight << endl;
        EXPECT_NE(one_private_range.weight, 0);
        if(one_private_range.toString() == "AhAs") {
            EXPECT_EQ(one_private_range.weight, 1);
        }else if(one_private_range.toString() == "AhKh"){
            EXPECT_EQ(one_private_range.weight, 10);
        }
    }
}
*/

/*
TEST(TestCase,test_cfr_river){
    vector<string> ranks = {"A", "K", "Q", "J", "T", "9", "8", "7", "6"};
    vector<string> suits = {"h", "s", "d", "c"};
    Deck deck = Deck(
            ranks,suits
    );
    shared_ptr<GameTree> game_tree = make_shared<GameTree>("../resources/gametree/simple_part_tree_depthinf.km",deck);
    string player1RangeStr = "AA,KK,QQ,JJ,TT,99,88,77,66,AK,AQ,AJ,AT,A9,A8,A7,A6,KQ,KJ,KT,K9,K8,K7,K6,QJ,QT,Q9,Q8,Q7,Q6,JT,J9,J8,J7,J6,T9,T8,T7,T6,98,97,96,87,86,76";
    string player2RangeStr = "AA,KK,QQ,JJ,TT,99,88,77,66,AK,AQ,AJ,AT,A9,A8,A7,A6,KQ,KJ,KT,K9,K8,K7,K6,QJ,QT,Q9,Q8,Q7,Q6,JT,J9,J8,J7,J6,T9,T8,T7,T6,98,97,96,87,86,76";

    vector<int> initialBoard = vector<int>{
            Card::strCard2int("Kd"),
            Card::strCard2int("Jd"),
            Card::strCard2int("Td"),
            Card::strCard2int("7s"),
            Card::strCard2int("8s")
    };

    vector<PrivateCards> player1Range = PrivateRangeConverter::rangeStr2Cards(player1RangeStr,initialBoard);
    vector<PrivateCards> player2Range = PrivateRangeConverter::rangeStr2Cards(player2RangeStr,initialBoard);
    string logfile_name = "../resources/outputs/outputs_log.txt";
    CfrSolver solver = CfrSolver(
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
    );
    solver.train();
}

TEST(TestCase,test_cfr_river_asymmetric){
    vector<string> ranks = {"A", "K", "Q", "J", "T", "9", "8", "7", "6"};
    vector<string> suits = {"h", "s", "d", "c"};
    Deck deck = Deck(
            ranks,suits
    );
    shared_ptr<GameTree> game_tree = make_shared<GameTree>("../resources/gametree/simple_part_tree_depthinf.km",deck);
    string player1RangeStr = "AA,KK,QQ,JJ,TT,99,88,77,66,AK,AQ,AJ,AT,A9,A8,A7,A6,KQ,KJ,KT,K9,K8,K7,K6,QJ,QT,Q9,Q8,Q7,Q6,JT,J9,J8,J7,J6,T9,T8,T7,T6,98,97,96,87,86,76";
    string player2RangeStr = "A9,A8,A7,A6,KQ,KJ,KT,K9,K8,K7,K6,QJ,QT,Q9,Q8,Q7,Q6,JT,J9,J8,J7,J6,T9,T8,T7,T6,98,97,96,87,86,76";

    vector<int> initialBoard = vector<int>{
            Card::strCard2int("Kd"),
            Card::strCard2int("Jd"),
            Card::strCard2int("Td"),
            Card::strCard2int("7s"),
            Card::strCard2int("8s")
    };

    vector<PrivateCards> player1Range = PrivateRangeConverter::rangeStr2Cards(player1RangeStr,initialBoard);
    vector<PrivateCards> player2Range = PrivateRangeConverter::rangeStr2Cards(player2RangeStr,initialBoard);
    string logfile_name = "../resources/outputs/outputs_log.txt";
    CfrSolver solver = CfrSolver(
            game_tree
            , player1Range
            , player2Range
            , initialBoard
            , compairer
            , deck
            , 1000
            , false
            , 100
            , logfile_name
            , "discounted_cfr"
            , Solver::MonteCarolAlg::NONE
    );
    solver.train();

    json dump_json = solver.getTree()->dumps(false);
    ofstream fileWriter;
    fileWriter.open("../resources/outputs/outputs_strategy.json");
    fileWriter << dump_json;
    fileWriter.flush();
    fileWriter.close();
}
*/

/*
TEST(TestCase,test_cfr_turn){
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
    CfrSolver solver = CfrSolver(
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
    );
    solver.train();
}
*/

/*
TEST(TestCase,test_cfr_turn_asymmetric){
    vector<string> ranks = {"A", "K", "Q", "J", "T", "9", "8", "7", "6"};
    vector<string> suits = {"h", "s", "d", "c"};
    Deck deck = Deck(
            ranks,suits
    );
    shared_ptr<GameTree> game_tree = make_shared<GameTree>("../resources/gametree/part_tree_turn_depthinf.km",deck);
    string player1RangeStr = "AA,KK,QQ,JJ,TT,99,88,77,66,AK,AQ,AJ,AT,A9,A8,A7,A6,KQ,KJ,KT,K9,K8,K7,K6,QJ,QT,Q9,Q8,Q7,Q6,JT,J9,J8,J7,J6,T9,T8,T7,T6,98,97,96,87,86,76";
    string player2RangeStr = "A9,A8,A7,A6,KQ,KJ,KT,K9,K8,K7,K6,QJ,QT,Q9,Q8,Q7,Q6,JT,J9,J8,J7,J6,T9,T8,T7,T6,98,97,96,87,86,76";

    vector<int> initialBoard = vector<int>{
            Card::strCard2int("Kd"),
            Card::strCard2int("Jd"),
            Card::strCard2int("Td"),
            Card::strCard2int("7s"),
    };

    vector<PrivateCards> player1Range = PrivateRangeConverter::rangeStr2Cards(player1RangeStr,initialBoard);
    vector<PrivateCards> player2Range = PrivateRangeConverter::rangeStr2Cards(player2RangeStr,initialBoard);
    string logfile_name = "../resources/outputs/outputs_log.txt";
    CfrSolver solver = CfrSolver(
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
    );
    solver.train();
}

TEST(TestCase,test_cfr_flop){
    vector<string> ranks = {"A", "K", "Q", "J", "T", "9", "8", "7", "6"};
    vector<string> suits = {"h", "s", "d", "c"};
    Deck deck = Deck(
            ranks,suits
    );
    shared_ptr<GameTree> game_tree = make_shared<GameTree>("../resources/gametree/game_tree_flop.km",deck);
    string player1RangeStr = "AA,KK,QQ,JJ,TT,99,88,77,66,AK,AQ,AJ,AT,A9,A8,A7,A6,KQ,KJ,KT,K9,K8,K7,K6,QJ,QT,Q9,Q8,Q7,Q6,JT,J9,J8,J7,J6,T9,T8,T7,T6,98,97,96,87,86,76";
    string player2RangeStr = "AA,KK,QQ,JJ,TT,99,88,77,66,AK,AQ,AJ,AT,A9,A8,A7,A6,KQ,KJ,KT,K9,K8,K7,K6,QJ,QT,Q9,Q8,Q7,Q6,JT,J9,J8,J7,J6,T9,T8,T7,T6,98,97,96,87,86,76";

    vector<int> initialBoard = vector<int>{
            Card::strCard2int("Kd"),
            Card::strCard2int("Jd"),
            Card::strCard2int("Td"),
    };

    vector<PrivateCards> player1Range = PrivateRangeConverter::rangeStr2Cards(player1RangeStr,initialBoard);
    vector<PrivateCards> player2Range = PrivateRangeConverter::rangeStr2Cards(player2RangeStr,initialBoard);
    string logfile_name = "../resources/outputs/outputs_log.txt";
    CfrSolver solver = CfrSolver(
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
    );
    solver.train();
}

TEST(TestCase,test_cfr_turn_parallel){
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
            , -1
    );
    solver.train();
}

TEST(TestCase,test_poker_solver){
    string ranks = "A,K,Q,J,T,9,8,7,6,5,4,3,2";
    string suits = "h,s,d,c";
    string game_tree = "../resources/gametree/generated_tree.km";
    game_tree = "../install/tree.km";
    string logfile_name = "../resources/outputs/outputs_log.txt";
    PokerSolver ps = PokerSolver(ranks,suits,"../resources/compairer/card5_dic_sorted.txt",2598961);
    ps.load_game_tree(game_tree);
    //ps.getGameTree()->recurrentPrintTree(ps.getGameTree()->getRoot(),0,100);

    ps.train(
            "AA,KK,QQ,JJ,TT,99:0.75,88:0.75,77:0.5,66:0.25,55:0.25,AK,AQs,AQo:0.75,AJs,AJo:0.5,ATs:0.75,A6s:0.25,A5s:0.75,A4s:0.75,A3s:0.5,A2s:0.5,KQs,KQo:0.5,KJs,KTs:0.75,K5s:0.25,K4s:0.25,QJs:0.75,QTs:0.75,Q9s:0.5,JTs:0.75,J9s:0.75,J8s:0.75,T9s:0.75,T8s:0.75,T7s:0.75,98s:0.75,97s:0.75,96s:0.5,87s:0.75,86s:0.5,85s:0.5,76s:0.75,75s:0.5,65s:0.75,64s:0.5,54s:0.75,53s:0.5,43s:0.5",
            "QQ:0.5,JJ:0.75,TT,99,88,77,66,55,44,33,22,AKo:0.25,AQs,AQo:0.75,AJs,AJo:0.75,ATs,ATo:0.75,A9s,A8s,A7s,A6s,A5s,A4s,A3s,A2s,KQ,KJ,KTs,KTo:0.5,K9s,K8s,K7s,K6s,K5s,K4s:0.5,K3s:0.5,K2s:0.5,QJ,QTs,Q9s,Q8s,Q7s,JTs,JTo:0.5,J9s,J8s,T9s,T8s,T7s,98s,97s,96s,87s,86s,76s,75s,65s,64s,54s,53s,43s",
            "Qs,Jh,2h",
            logfile_name,
            91,
            10,
            "discounted_cfr",
            -1,
            8
    );
    //ps.dump_strategy("../resources/outputs/outputs_strategy.json");
}
 */

TEST(TestCase,test_poker_solver_bench){
    string ranks = "A,K,Q,J,T,9,8,7,6,5,4,3,2";
    string suits = "h,s,d,c";
    string game_tree = "../resources/gametree/generated_tree.km";
    game_tree = "../install/tree.km";
    string logfile_name = "../resources/outputs/outputs_log.txt";
    PokerSolver ps = PokerSolver(ranks,suits,"../resources/compairer/card5_dic_sorted.txt",2598961);
    ps.load_game_tree(game_tree);
    ps.getGameTree()->recurrentPrintTree(ps.getGameTree()->getRoot(),0,4);

    ps.train(
            //"AA:0.001,KK:0.001,QQ:0.001,JJ:0.001,TT:0.001,99:0.001,88:0.001,77:0.001,66:0.001,55:0.001,44:0.24,33:0.2,22:0.5,AK:0.001,AQ:0.001,AJ:0.001,ATs:0.16,ATo:0.3,A9s,A9o:0.15,A8s:0.15,A8o:0.001,A7s:0.38,A7o:0.001,A6s:0.35,A6o:0.001,A5s:0.35,A5o:0.001,A4s:0.35,A4o:0.001,A3s:0.5,A3o:0.001,A2:0.001,KQs:0.05,KQo:0.001,KJs:0.4,KJo:0.5,KTs:0.6,KTo:0.2,K9s,K9o:0.001,K8s:0.55,K8o:0.001,K7:0.001,K6:0.001,K5:0.001,K4:0.001,K3:0.001,K2:0.001,QJs:0.001,QJo:0.58,QTs:0.1,QTo:0.08,Q9s:0.75,Q9o:0.001,Q8:0.001,Q7:0.001,Q6:0.001,Q5:0.001,Q4:0.001,Q3:0.001,Q2:0.001,JT:0.001,J9s:0.03,J9o:0.001,J8:0.001,J7:0.001,J6:0.001,J5:0.001,J4:0.001,J3:0.001,J2:0.001,T9s:0.06,T9o:0.001,T8s:0.55,T8o:0.001,T7:0.001,T6:0.001,T5:0.001,T4:0.001,T3:0.001,T2:0.001,98s:0.2,98o:0.001,97:0.001,96:0.001,95:0.001,94:0.001,93:0.001,92:0.001,87s:0.3,87o:0.001,86:0.001,85:0.001,84:0.001,83:0.001,82:0.001,76s:0.3,76o:0.001,75:0.001,74:0.001,73:0.001,72:0.001,65s:0.37,65o:0.001,64:0.001,63:0.001,62:0.001,54:0.001,53:0.001,52:0.001,43:0.001,42:0.001,32:0.001"
            //"AA,KK,QQ,JJ,TT,99,88,77,66,55,44:0.925000012,33:0.95,22:0.200050003,AK,AQ,AJ,AT,A9,A8,A7,A6s,A6o:0.849999994,A5s,A5o:0.949999988,A4s,A4o:0.0500500005,A3s,A3o:0.0250500004,A2s,A2o:0.0250500004,KQ,KJ,KT,K9s:0.925000012,K9o,K8s,K8o:0.337499997,K7s,K7o:0.0500500005,K6s,K6o:0.00055000005,K5s,K5o:0.00055000005,K4s,K4o:0.00055000005,K3s,K3o:0.00055000005,K2s:0.200050003,K2o:0.00055000005,QJ,QT,Q9s,Q9o:0.949999988,Q8s,Q8o:0.0500500005,Q7s,Q7o:0.00055000005,Q6s,Q6o:0.00055000005,Q5s,Q5o:0.00055000005,Q4s:0.799999988,Q4o:0.00055000005,Q3s:0.0500500005,Q3o:0.00055000005,Q2s:0.0250500004,Q2o:0.00055000005,JT,J9:0.900000006,J8s,J8o:0.0500500005,J7s,J7o:0.00055000005,J6s,J6o:0.00055000005,J5s:0.524999988,J5o:0.00055000005,J4s:0.0500500005,J4o:0.00055000005,J3s:0.0250500004,J3o:0.00055000005,J2:0.00055000005,T9s,T9o:0.949999988,T8s,T8o:0.0250500004,T7s,T7o:0.00055000005,T6s,T6o:0.00055000005,T5s:0.0500500005,T5o:0.00055000005,T4s:0.0250500004,T4o:0.00055000005,T3:0.00055000005,T2:0.00055000005,98s,98o:0.300000002,97s,97o:0.00055000005,96s,96o:0.00055000005,95s:0.0500500005,95o:0.00055000005,94:0.00055000005,93:0.00055000005,92:0.00055000005,87s,87o:0.00055000005,86s,86o:0.00055000005,85s:0.949999988,85o:0.00055000005,84:0.00055000005,83:0.00055000005,82:0.00055000005,76s,76o:0.00055000005,75s,75o:0.00055000005,74s:0.0500500005,74o:0.00055000005,73:0.00055000005,72:0.00055000005,65s,65o:0.00055000005,64s:0.25005,64o:0.00055000005,63:0.00055000005,62:0.00055000005,54s:0.949999988,54o:0.00055000005,53s:0.0250500004,53o:0.00055000005,52:0.00055000005,43s:0.0500500005,43o:0.00055000005,42:0.00055000005,32:0.00055000005"
            "AA,KK,QQ,JJ,TT,99:0.75,88:0.75,77:0.5,66:0.25,55:0.25,AK,AQs,AQo:0.75,AJs,AJo:0.5,ATs:0.75,A6s:0.25,A5s:0.75,A4s:0.75,A3s:0.5,A2s:0.5,KQs,KQo:0.5,KJs,KTs:0.75,K5s:0.25,K4s:0.25,QJs:0.75,QTs:0.75,Q9s:0.5,JTs:0.75,J9s:0.75,J8s:0.75,T9s:0.75,T8s:0.75,T7s:0.75,98s:0.75,97s:0.75,96s:0.5,87s:0.75,86s:0.5,85s:0.5,76s:0.75,75s:0.5,65s:0.75,64s:0.5,54s:0.75,53s:0.5,43s:0.5",
            "QQ:0.5,JJ:0.75,TT,99,88,77,66,55,44,33,22,AKo:0.25,AQs,AQo:0.75,AJs,AJo:0.75,ATs,ATo:0.75,A9s,A8s,A7s,A6s,A5s,A4s,A3s,A2s,KQ,KJ,KTs,KTo:0.5,K9s,K8s,K7s,K6s,K5s,K4s:0.5,K3s:0.5,K2s:0.5,QJ,QTs,Q9s,Q8s,Q7s,JTs,JTo:0.5,J9s,J8s,T9s,T8s,T7s,98s,97s,96s,87s,86s,76s,75s,65s,64s,54s,53s,43s",
            "Qs,Jh,2h",
            logfile_name,
            300,
            10,
            "discounted_cfr",
            -1,
            0.5,
            true,
            8
    );
    //ps.dump_strategy("../resources/outputs/outputs_strategy.json");
}

 /*

TEST(TestCase,test_build_tree_and_solve){
    string ranks = "A,K,Q,J,T,9,8,7,6,5,4,3,2";
    string suits = "h,s,d,c";
    string logfile_name = "../resources/outputs/outputs_log.txt";
    PokerSolver ps = PokerSolver(ranks,suits,"../resources/compairer/card5_dic_sorted.txt",2598961);

    float oop_commit=25;
    float ip_commit=25;
    int current_round=1;
    int raise_limit=4;
    float small_blind=0.5;
    float big_blind=1;
    float stack=230 + 25;
    float allin_threshold = 0.67;
    StreetSetting gbs_flop_ip = StreetSetting(vector<float>{30,70,100},vector<float>{40,120},vector<float>{},true);
    StreetSetting gbs_turn_ip = StreetSetting(vector<float>{30,70,100},vector<float>{40,120},vector<float>{},true);
    StreetSetting gbs_river_ip = StreetSetting(vector<float>{30,70,100},vector<float>{55},vector<float>{},true);

    StreetSetting gbs_flop_oop = StreetSetting(vector<float>{30,70,100},vector<float>{40,120},vector<float>{},true);
    StreetSetting gbs_turn_oop = StreetSetting(vector<float>{30,70,100},vector<float>{40,120},vector<float>{50,100},true);
    StreetSetting gbs_river_oop = StreetSetting(vector<float>{30,70,100},vector<float>{55},vector<float>{50,100},true);

    GameTreeBuildingSettings gtbs = GameTreeBuildingSettings(gbs_flop_ip,gbs_turn_ip,gbs_river_ip,gbs_flop_oop,gbs_turn_oop,gbs_river_oop);
    ps.build_game_tree(oop_commit,ip_commit,current_round,raise_limit,small_blind,big_blind,stack,gtbs,allin_threshold);
    cout << "build tree finished" << endl;
    ps.getGameTree()->recurrentPrintTree(ps.getGameTree()->getRoot(),0,4);
    ps.train(
            //"AA:0.001,KK:0.001,QQ:0.001,JJ:0.001,TT:0.001,99:0.001,88:0.001,77:0.001,66:0.001,55:0.001,44:0.24,33:0.2,22:0.5,AK:0.001,AQ:0.001,AJ:0.001,ATs:0.16,ATo:0.3,A9s,A9o:0.15,A8s:0.15,A8o:0.001,A7s:0.38,A7o:0.001,A6s:0.35,A6o:0.001,A5s:0.35,A5o:0.001,A4s:0.35,A4o:0.001,A3s:0.5,A3o:0.001,A2:0.001,KQs:0.05,KQo:0.001,KJs:0.4,KJo:0.5,KTs:0.6,KTo:0.2,K9s,K9o:0.001,K8s:0.55,K8o:0.001,K7:0.001,K6:0.001,K5:0.001,K4:0.001,K3:0.001,K2:0.001,QJs:0.001,QJo:0.58,QTs:0.1,QTo:0.08,Q9s:0.75,Q9o:0.001,Q8:0.001,Q7:0.001,Q6:0.001,Q5:0.001,Q4:0.001,Q3:0.001,Q2:0.001,JT:0.001,J9s:0.03,J9o:0.001,J8:0.001,J7:0.001,J6:0.001,J5:0.001,J4:0.001,J3:0.001,J2:0.001,T9s:0.06,T9o:0.001,T8s:0.55,T8o:0.001,T7:0.001,T6:0.001,T5:0.001,T4:0.001,T3:0.001,T2:0.001,98s:0.2,98o:0.001,97:0.001,96:0.001,95:0.001,94:0.001,93:0.001,92:0.001,87s:0.3,87o:0.001,86:0.001,85:0.001,84:0.001,83:0.001,82:0.001,76s:0.3,76o:0.001,75:0.001,74:0.001,73:0.001,72:0.001,65s:0.37,65o:0.001,64:0.001,63:0.001,62:0.001,54:0.001,53:0.001,52:0.001,43:0.001,42:0.001,32:0.001",
            //"AA,KK,QQ,JJ,TT,99,88,77,66,55,44:0.925000012,33:0.95,22:0.200050003,AK,AQ,AJ,AT,A9,A8,A7,A6s,A6o:0.849999994,A5s,A5o:0.949999988,A4s,A4o:0.0500500005,A3s,A3o:0.0250500004,A2s,A2o:0.0250500004,KQ,KJ,KT,K9s:0.925000012,K9o,K8s,K8o:0.337499997,K7s,K7o:0.0500500005,K6s,K6o:0.00055000005,K5s,K5o:0.00055000005,K4s,K4o:0.00055000005,K3s,K3o:0.00055000005,K2s:0.200050003,K2o:0.00055000005,QJ,QT,Q9s,Q9o:0.949999988,Q8s,Q8o:0.0500500005,Q7s,Q7o:0.00055000005,Q6s,Q6o:0.00055000005,Q5s,Q5o:0.00055000005,Q4s:0.799999988,Q4o:0.00055000005,Q3s:0.0500500005,Q3o:0.00055000005,Q2s:0.0250500004,Q2o:0.00055000005,JT,J9:0.900000006,J8s,J8o:0.0500500005,J7s,J7o:0.00055000005,J6s,J6o:0.00055000005,J5s:0.524999988,J5o:0.00055000005,J4s:0.0500500005,J4o:0.00055000005,J3s:0.0250500004,J3o:0.00055000005,J2:0.00055000005,T9s,T9o:0.949999988,T8s,T8o:0.0250500004,T7s,T7o:0.00055000005,T6s,T6o:0.00055000005,T5s:0.0500500005,T5o:0.00055000005,T4s:0.0250500004,T4o:0.00055000005,T3:0.00055000005,T2:0.00055000005,98s,98o:0.300000002,97s,97o:0.00055000005,96s,96o:0.00055000005,95s:0.0500500005,95o:0.00055000005,94:0.00055000005,93:0.00055000005,92:0.00055000005,87s,87o:0.00055000005,86s,86o:0.00055000005,85s:0.949999988,85o:0.00055000005,84:0.00055000005,83:0.00055000005,82:0.00055000005,76s,76o:0.00055000005,75s,75o:0.00055000005,74s:0.0500500005,74o:0.00055000005,73:0.00055000005,72:0.00055000005,65s,65o:0.00055000005,64s:0.25005,64o:0.00055000005,63:0.00055000005,62:0.00055000005,54s:0.949999988,54o:0.00055000005,53s:0.0250500004,53o:0.00055000005,52:0.00055000005,43s:0.0500500005,43o:0.00055000005,42:0.00055000005,32:0.00055000005",
            "AA,KK,QQ,JJ,TT,99:0.75,88:0.75,77:0.5,66:0.25,55:0.25,AK,AQs,AQo:0.75,AJs,AJo:0.5,ATs:0.75,A6s:0.25,A5s:0.75,A4s:0.75,A3s:0.5,A2s:0.5,KQs,KQo:0.5,KJs,KTs:0.75,K5s:0.25,K4s:0.25,QJs:0.75,QTs:0.75,Q9s:0.5,JTs:0.75,J9s:0.75,J8s:0.75,T9s:0.75,T8s:0.75,T7s:0.75,98s:0.75,97s:0.75,96s:0.5,87s:0.75,86s:0.5,85s:0.5,76s:0.75,75s:0.5,65s:0.75,64s:0.5,54s:0.75,53s:0.5,43s:0.5",
            "QQ:0.5,JJ:0.75,TT,99,88,77,66,55,44,33,22,AKo:0.25,AQs,AQo:0.75,AJs,AJo:0.75,ATs,ATo:0.75,A9s,A8s,A7s,A6s,A5s,A4s,A3s,A2s,KQ,KJ,KTs,KTo:0.5,K9s,K8s,K7s,K6s,K5s,K4s:0.5,K3s:0.5,K2s:0.5,QJ,QTs,Q9s,Q8s,Q7s,JTs,JTo:0.5,J9s,J8s,T9s,T8s,T7s,98s,97s,96s,87s,86s,76s,75s,65s,64s,54s,53s,43s",
            "Qs,Jh,2h",
            logfile_name,
            300,
            10,
            "discounted_cfr",
            -1,
            0.5,
            true,
            8
    );
    //ps.dump_strategy("../resources/outputs/outputs_strategy.json");
}

TEST(TestCase,test_command_line_tool){
    CommandLineTool clt = CommandLineTool();
    clt.startWorking();
}
 */


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
