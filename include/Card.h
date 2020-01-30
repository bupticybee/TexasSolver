//
// Created by Xuefeng Huang on 2020/1/28.
//

#ifndef TEXASSOLVER_CARD_H
#define TEXASSOLVER_CARD_H
#include <iostream>
#include <string>
#include <vector>
#include "fmt/format.h"

using namespace std;

class Card {
    private:
        string card;
        int card_int;
    public:
        Card();
        Card(string card);
        string getCard();
        int getCardInt();
        static int card2int(Card card);
        static int strCard2int(string card);
        static string intCard2Str(int card);
        static long boardCards2long(vector<string> cards);
        static long boardCard2long(Card card);
        static long boardCards2long(vector<Card> cards);
        static bool boardsHasIntercept(long board1,long board2);
        //static long privateHand2long(PrivateCards one_hand);
        static long boardInts2long(vector<int> board);
        static vector<int> long2board(long board_long);
        static vector<Card> long2boardCards(long board_long);
        static string suitToString(int suit);
        static string rankToString(int rank);
        static int rankToInt(char rank);
        static int suitToInt(char suit);
        static vector<string> getSuits();
        string toString();
};

#endif //TEXASSOLVER_CARD_H
