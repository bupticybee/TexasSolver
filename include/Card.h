//
// Created by Xuefeng Huang on 2020/1/28.
//

#ifndef TEXASSOLVER_CARD_H
#define TEXASSOLVER_CARD_H
#include <iostream>
#include <string>
#include <vector>
#include "include/tools/tinyformat.h"
#include <QString>
using namespace std;

class Card {
    private:
        string card;
        int card_int;
        int card_number_in_deck;
    public:
        Card();
        explicit Card(string card,int card_number_in_deck);
        Card(string card);
        string getCard();
        int getCardInt();
        bool empty();
        int getNumberInDeckInt();
        static int card2int(Card card);
        static int strCard2int(string card);
        static string intCard2Str(int card);
        static uint64_t boardCards2long(vector<string> cards);
        static uint64_t boardCard2long(Card& card);
        static uint64_t boardCards2long(vector<Card>& cards);
        static QString boardCards2html(vector<Card>& cards);
        static inline bool boardsHasIntercept(uint64_t board1,uint64_t board2){
            return ((board1 & board2) != 0);
        };
        static uint64_t boardInts2long(const vector<int>& board);
        static uint64_t boardInt2long(int board);
        static vector<int> long2board(uint64_t board_long);
        static vector<Card> long2boardCards(uint64_t board_long);
        static string suitToString(int suit);
        static string rankToString(int rank);
        static int rankToInt(char rank);
        static int suitToInt(char suit);
        static vector<string> getSuits();
        string toString();
        string toFormattedString();
        QString toFormattedHtml();
};

#endif //TEXASSOLVER_CARD_H
