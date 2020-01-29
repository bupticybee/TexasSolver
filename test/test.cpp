//
// Created by Xuefeng Huang on 2020/1/28.
//
#include <iostream>
#include "gtest/gtest.h"
#include "Card.h"
#include "fmt/format.h"
#include "compairer/Dic5Compairer.h"

using namespace std;
static Dic5Compairer compairer;

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

TEST(TestCase,test_comapirer_load ){
    compairer = Dic5Compairer("../resources/compairer/card5_dic_sorted_shortdeck.txt",376993);
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

    Compairer::CompairResult cr = compairer.compair(private1,private2,board);

    EXPECT_EQ(cr,Compairer::CompairResult::LARGER);
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

    Compairer::CompairResult cr = compairer.compair(private1,private2,board);

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

    Compairer::CompairResult cr = compairer.compair(private1,private2,board);

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

    int rank = compairer.get_rank(private_cards,board);

    EXPECT_EQ(rank,687);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
