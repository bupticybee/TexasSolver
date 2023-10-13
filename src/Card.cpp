//
// Created by Xuefeng Huang on 2020/1/28.
//

#include "include/Card.h"

Card::Card(){this->card = "empty";}
Card::Card(string card,int card_num_in_deck){
    this->card = card;
    this->card_int = Card::strCard2int(this->card);
    this->card_number_in_deck = card_num_in_deck;
}

Card::Card(string card){
    this->card = card;
    this->card_int = Card::strCard2int(this->card);
    this->card_number_in_deck = -1;
}

bool Card::empty(){
    if(this->card == "empty")return true;
    else return false;
}

string Card::getCard() {
    return this->card;
}

int Card::getCardInt() {
    return this->card_int;
}

int Card::getNumberInDeckInt(){
    if(this->card_number_in_deck == -1)throw runtime_error("card number in deck cannot be -1");
    return this->card_number_in_deck;
}

int Card::card2int(Card card) {
    return strCard2int(card.getCard());
}

int Card::strCard2int(string card) {
    char rank = card.at(0);
    char suit = card.at(1);
    if(card.length() != 2){
        throw runtime_error( tfm::format("card %s not found",card));
    }
    return (rankToInt(rank) - 2) * 4 + suitToInt(suit);
}

string Card::intCard2Str(int card){
    int rank = card / 4 + 2;
    int suit = card - (rank-2)*4;
    return Card::rankToString(rank) + Card::suitToString(suit);
}

uint64_t Card::boardCards2long(vector<string> cards) {
    vector<Card> cards_objs(cards.size());
    for(std::size_t i = 0;i < cards.size();i++){
        cards_objs[i] = Card(cards[i]);
    }
    return Card::boardCards2long(cards_objs);
}

uint64_t Card::boardCard2long(Card& card){
    return Card::boardInt2long(card.getCardInt());
}

uint64_t Card::boardCards2long(vector<Card>& cards){
    std::vector<int> board_int(cards.size());
    for(std::size_t i = 0;i < cards.size();i++){
        board_int[i] = Card::card2int(cards[i]);
    }
    return Card::boardInts2long(board_int);
}

QString Card::boardCards2html(vector<Card>& cards){
    QString ret_html = "";
    for(auto one_card:cards){
        if(one_card.empty())continue;
        ret_html += one_card.toFormattedHtml();
    }
    return ret_html;
}

uint64_t Card::boardInt2long(int board){
    // 这里hard code了一副扑克牌是52张
    if(board < 0 || board >= 52){
        throw runtime_error(tfm::format("Card with id %s not found",board));
    }
    // uint64_7 的range 在0 ~ + 2^ 64之间,所以不用太担心溢出问题
    return ((uint64_t)(1) << board);
}

uint64_t Card::boardInts2long(const vector<int>& board){
    if(board.size() < 1 || board.size() > 7){
        throw runtime_error(tfm::format("Card length incorrect: %s",board.size()));
    }
    uint64_t board_long = 0;
    for(int one_card: board){
        // 这里hard code了一副扑克牌是52张
        if(one_card < 0 || one_card >= 52){
            throw runtime_error(tfm::format("Card with id %s not found",one_card));
        }
        // uint64_7 的range 在0 ~ + 2^ 64之间,所以不用太担心溢出问题
        board_long += ((uint64_t)(1) << one_card);
    }
    return board_long;
}

/*
long Card::privateHand2long(PrivateCards one_hand){
    return boardInts2long(new int[]{one_hand.card1,one_hand.card2});
}
 */

vector<int> Card::long2board(uint64_t board_long) {
    vector<int> board;
    board.reserve(7);
    for(int i = 0;i < 52;i ++){
        // 看二进制最后一位是否为1
        if((board_long & 1) == 1){
            board.push_back(i);
        }
        board_long = board_long >> 1;
    }
    if (board.size() < 1 || board.size() > 7){
        throw runtime_error(tfm::format("board length not correct, board length %s",board.size()));
    }
    return board;
}

vector<Card> Card::long2boardCards(uint64_t board_long){
        vector<int> board = long2board(board_long);
        vector<Card> board_cards(board.size());
        for(std::size_t i = 0;i < board.size();i ++){
            int one_board = board[i];
            board_cards[i] = Card(intCard2Str(one_board));
        }
        if (board_cards.size() < 1 || board_cards.size() > 7){
            throw runtime_error(tfm::format("board length not correct, board length %s",board_cards.size()));
        }
        vector<Card> retval(board_cards.size());
        for(std::size_t i = 0;i < board_cards.size();i ++){
            retval[i] = board_cards[i];
        }
        return retval;
}

string Card::suitToString(int suit)
{
    switch(suit)
    {
        case 0: return "c";
        case 1: return "d";
        case 2: return "h";
        case 3: return "s";
        default: return "c";
    }
}

string Card::rankToString(int rank)
{
    switch(rank)
    {
        case 2: return "2";
        case 3: return "3";
        case 4: return "4";
        case 5: return "5";
        case 6: return "6";
        case 7: return "7";
        case 8: return "8";
        case 9: return "9";
        case 10: return "T";
        case 11: return "J";
        case 12: return "Q";
        case 13: return "K";
        case 14: return "A";
        default: return "2";
    }
}

int Card::rankToInt(char rank)
{
    switch(rank)
    {
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        case 'T': return 10;
        case 'J': return 11;
        case 'Q': return 12;
        case 'K': return 13;
        case 'A': return 14;
        default: return 2;
    }
}

int Card::suitToInt(char suit)
{
    switch(suit)
    {
        case 'c': return 0; // 梅花
        case 'd': return 1; // 方块
        case 'h': return 2; // 红桃
        case 's': return 3; // 黑桃
        default: return 0;
    }
}

vector<string> Card::getSuits(){
    return {"c","d","h","s"};
}

string Card::toString() {
    return this->card;
}

string Card::toFormattedString() {
    QString qString = QString::fromStdString(this->card);
    qString = qString.replace("c", "♣️");
    qString = qString.replace("d", "♦️");
    qString = qString.replace("h", "♥️");
    qString = qString.replace("s", "♠️");
    return qString.toStdString();
}

QString Card::toFormattedHtml() {
    QString qString = QString::fromStdString(this->card);
    if(qString.contains("c"))
        qString = qString.replace("c", QString::fromLocal8Bit("<span style=\"color:black;\">&#9827;<\/span>"));
    else if(qString.contains("d"))
        qString = qString.replace("d", QString::fromLocal8Bit("<span style=\"color:red;\">&#9830;<\/span>"));
    else if(qString.contains("h"))
        qString = qString.replace("h", QString::fromLocal8Bit("<span style=\"color:red;\">&#9829;<\/span>"));
    else if(qString.contains("s"))
        qString = qString.replace("s", QString::fromLocal8Bit("<span style=\"color:black;\">&#9824;<\/span>"));
    return qString;
}
