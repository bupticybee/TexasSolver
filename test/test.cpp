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
//#include <boost/version.hpp>

using namespace std;
static shared_ptr<Dic5Compairer> compairer;

/*
TEST(TestCase,test_boost_env){
    cout << "Boost版本：" << BOOST_VERSION << endl;
}
*/

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
            , 100
            , false
            , 10
            , logfile_name
            , "discounted_cfr"
            , Solver::MonteCarolAlg::NONE
    );
    solver.train();
}

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

 */
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
    string ranks = "A,K,Q,J,T,9,8,7,6";
    string suits = "h,s,d,c";
    string game_tree = "../resources/gametree/generated_tree.km";
    string logfile_name = "../resources/outputs/outputs_log.txt";
    PokerSolver ps = PokerSolver(ranks,suits,"../resources/compairer/card5_dic_sorted_shortdeck.txt",376993);
    ps.load_game_tree(game_tree);
    ps.train(
            "AA,KK,QQ,JJ,TT,99,88,77,66,AK,AQ,AJ,AT,A9,A8,A7,A6,KQ,KJ,KT,K9,K8,K7,K6,QJ,QT,Q9,Q8,Q7,Q6,JT,J9,J8,J7,J6,T9,T8,T7,T6,98,97,96,87,86,76",
            "AA,KK,QQ,JJ,TT,99,88,77,66,AK,AQ,AJ,AT,A9,A8,A7,A6,KQ,KJ,KT,K9,K8,K7,K6,QJ,QT,Q9,Q8,Q7,Q6,JT,J9,J8,J7,J6,T9,T8,T7,T6,98,97,96,87,86,76",
             "Kd,Jd,Td",
            logfile_name,
            20,
            10,
            "discounted_cfr",
            -1
    );
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
