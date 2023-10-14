//
// Created by Xuefeng Huang on 2020/1/29.
//

#include "include/compairer/Dic5Compairer.h"

#include <utility>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "time.h"
#include "unistd.h"

#define SUIT_0_MASK   0x1111111111111
#define SUIT_1_MASK   0x2222222222222
#define SUIT_2_MASK   0x4444444444444
#define SUIT_3_MASK   0x8888888888888
#define BIT_SUM_0     0x5555555555555
#define BIT_SUM_1     0x3333333333333

// 牌组合的rank频数
uint64_t ranks_hash(uint64_t cards_hash) {
    cards_hash = (cards_hash & BIT_SUM_0) + ((cards_hash >> 1) & BIT_SUM_0);
    cards_hash = (cards_hash & BIT_SUM_1) + ((cards_hash >> 2) & BIT_SUM_1);
    return cards_hash;
}
// 牌组合是否为同花
bool is_flush(uint64_t hash) {
    int cnt = (hash & SUIT_0_MASK) != 0;
    cnt += (hash & SUIT_1_MASK) != 0;
    if (cnt > 1) return false;
    cnt += (hash & SUIT_2_MASK) != 0;
    if (cnt > 1) return false;
    cnt += (hash & SUIT_3_MASK) != 0;
    return cnt > 1 ? false : true;
}

void FiveCardsStrength::convert(unordered_map<uint64_t, int>& strength_map) {
    flush_map.clear(); other_map.clear();
    auto it = strength_map.begin(), it_end = strength_map.end();
    for (; it != it_end; it++) {
        uint64_t cards_hash = it->first;
        uint64_t hash = ranks_hash(cards_hash);
        if (is_flush(cards_hash)) flush_map[cards_hash] = it->second;
        else {
            /*if (other_map.count(hash) && other_map[hash] != it->second) {
                printf("%zx,%zx:%d != %d\n", cards_hash, hash, it->second, other_map[hash]);
            }*/
            other_map[hash] = it->second;
        }
    }
}
bool FiveCardsStrength::load(const char* file_path) {
    //ifstream file(file_path, ios::binary);
    /*if (!file) {
        file.close();
        return false;
    }*/

    QFile file(QString::fromStdString(file_path));
    if (!file.open(QIODevice::ReadOnly)){
        throw runtime_error("unable to load compairer file");
    }
    flush_map.clear(); other_map.clear();
    int size_key = sizeof(uint64_t), size_int = sizeof(int), val, cnt = 0;
    uint64_t key = 0;
    char* p_key = (char*)&key, * p_val = (char*)&val, * p_cnt = (char*)&cnt;
    file.read(p_cnt, size_int);// 读取行数
    for (int i = 0; i < cnt; i++) {
        file.read(p_key, size_key);
        file.read(p_val, size_int);
        flush_map[key] = val;
    }
    assert(flush_map.size() == cnt);
    file.read(p_cnt, size_int);// 读取行数
    for (int i = 0; i < cnt; i++) {
        file.read(p_key, size_key);
        file.read(p_val, size_int);
        other_map[key] = val;
    }
    assert(other_map.size() == cnt);
    file.close();
    return true;
}
bool FiveCardsStrength::save(const char* file_path) {
    //qDebug() << "a";
    //sleep(10);
    //qDebug() << "b";
    //file_path = "/Users/bytedance/Desktop/card5_dic_zipped_shortdeck.bin";
    ofstream file(file_path, ios::binary);
    if (!file) {
        file.close();
        return false;
    }
    int size_key = sizeof(uint64_t), size_int = sizeof(int), val = flush_map.size();
    uint64_t key = 0;
    char* p_key = (char*)&key, * p_val = (char*)&val;
    file.write(p_val, size_int);// 写入行数
    auto it = flush_map.begin(), it_end = flush_map.end();
    for (; it != it_end; it++) {
        key = it->first; val = it->second;
        file.write(p_key, size_key);
        file.write(p_val, size_int);
    }
    val = other_map.size();
    file.write(p_val, size_int);// 写入行数
    it = other_map.begin(), it_end = other_map.end();
    for (; it != it_end; it++) {
        key = it->first; val = it->second;
        file.write(p_key, size_key);
        file.write(p_val, size_int);
    }
    file.close();
    return true;
}
int FiveCardsStrength::operator[](uint64_t hash) {
    auto it = flush_map.find(hash);
    if (it != flush_map.end()) return it->second;
    hash = ranks_hash(hash);
    return other_map.at(hash);
}
bool FiveCardsStrength::check(unordered_map<uint64_t, int>& strength_map) {
    auto it = strength_map.begin(), it_end = strength_map.end();
    int cnt = 0;
    for (; it != it_end; it++, cnt++) {
        uint64_t key = it->first;
        int val1 = it->second, val2 = operator[](key);
        if (val1 != val2) {
            printf("%zx,%zx:%d != %d\ncheck failed!\n", key, ranks_hash(key), val1, val2);
            return false;
        }
    }
    printf("%d pass!\n", cnt);
    return true;
}

Dic5Compairer::Dic5Compairer(string dic_dir,int lines,string dic_dir_bin):Compairer(std::move(dic_dir),lines){
    if(fcs.load(dic_dir_bin.c_str())) return;
    QFile infile(QString::fromStdString(this->dic_dir));
    if (!infile.open(QIODevice::ReadOnly)){
        throw runtime_error("unable to load compairer file");
    }
    QTextStream in(&infile);
    //progressbar bar(lines / 1000);
    int i = 0;
    //while (std::getline(infile, line))
    while (!in.atEnd())
    {
        string line = in.readLine().toStdString();
        vector<string> linesp = string_split(line,',');
        if(linesp.size() != 2){
           throw runtime_error(tfm::format("linesp not correct: %s",line));
        }
        string cards_str = linesp[0];
        int rank = stoi(linesp[1]);
        vector<string> cards = string_split(cards_str,'-');

        if(cards.size() != 5)
            throw runtime_error(
                    tfm::format(
                            "cards not correct: %s length %s",cards_str,cards.size()));

        //set<string> cards_set;
        //for(string one_card:cards) cards_set.insert(one_card);
        //this->card2rank[cards_set] = rank;

        if(this->cardslong2rank.find(Card::boardCards2long(cards)) != this->cardslong2rank.end()){
            throw runtime_error(
                    tfm::format("key repeated: %s",cards_str)
                    );
        }

        this->cardslong2rank[Card::boardCards2long(cards)] = rank;

        i ++;
        if(i % 1000 == 0) {
            //bar.update();
        }
    }
    //cout << endl;
    fcs.convert(this->cardslong2rank);
    if(!fcs.check(this->cardslong2rank)){
        //fcs.save(dic_dir_bin.c_str());
        throw runtime_error("check consistency failed");
    }
    this->cardslong2rank.clear();
}

Compairer::CompairResult Dic5Compairer::compairRanks(int rank_former, int rank_latter) {
    if (rank_former < rank_latter) {
        // rank更小的牌更大，0是同花顺
        return CompairResult::LARGER;
    } else if (rank_former > rank_latter) {
        return CompairResult::SMALLER;
    } else {
        // rank_former == rank_latter
        return CompairResult::EQUAL;
    }
}

Compairer::CompairResult
Dic5Compairer::compair(vector<Card> private_former, vector<Card> private_latter, vector<Card> public_board) {
    if(private_former.size() != 2)
        throw runtime_error(
                tfm::format("private former size incorrect,excepted 2, actually %s",private_former.size())
                );
    if(private_latter.size() != 2)
        throw runtime_error(
                tfm::format("private latter size incorrect,excepted 2, actually %s",private_latter.size())
        );
    if(public_board.size() != 5)
        throw runtime_error(
                tfm::format("public board size incorrect,excepted 2, actually %s",public_board.size())
        );

    vector<Card> former_cards(private_former);
    former_cards.insert(former_cards.end(),public_board.begin(),public_board.end());
    int rank_former = this->getRank(former_cards);

    vector<Card> latter_cards(private_latter);
    latter_cards.insert(latter_cards.end(),public_board.begin(),public_board.end());
    int rank_latter = this->getRank(latter_cards);

    return this->compairRanks(rank_former,rank_latter);
}

Compairer::CompairResult
Dic5Compairer::compair(vector<int> private_former, vector<int> private_latter, vector<int> public_board) {
    if(private_former.size() != 2)
        throw runtime_error(
                tfm::format("private former size incorrect,excepted 2, actually %s",private_former.size())
        );
    if(private_latter.size() != 2)
        throw runtime_error(
                tfm::format("private latter size incorrect,excepted 2, actually %s",private_latter.size())
        );
    if(public_board.size() != 5)
        throw runtime_error(
                tfm::format("public board size incorrect,excepted 2, actually %s",public_board.size())
        );

    vector<int> former_cards(private_former);
    former_cards.insert(former_cards.end(),public_board.begin(),public_board.end());
    int rank_former = this->getRank(former_cards);

    vector<int> latter_cards(private_latter);
    latter_cards.insert(latter_cards.end(),public_board.begin(),public_board.end());
    int rank_latter = this->getRank(latter_cards);

    return this->compairRanks(rank_former,rank_latter);
}

int Dic5Compairer::getRank(vector<Card> cards) {
    vector<int> cards_int(cards.size());
    for(std::size_t i = 0;i < cards.size();i++){
        cards_int[i] = cards[i].getCardInt();
    }
    return this->getRank(cards_int);
}

int Dic5Compairer::getRank(vector<int> cards) {
    Combinations<int> comb_cards(std::move(cards),5);
    int min_rank = numeric_limits<int>::max();
    for (const vector<int>& one_comb : comb_cards)
    {
        if(one_comb.size() != 5)throw runtime_error(tfm::format("card size incorrect: %s should be 5",one_comb.size()));
        uint64_t comb_uint64 = Card::boardInts2long(one_comb);
        //int rank = this->cardslong2rank[comb_uint64];
        int rank = this->fcs[comb_uint64];
        min_rank = min(rank,min_rank);
    }
    return min_rank;
}

int Dic5Compairer::get_rank(vector<Card> private_hand, vector<Card> public_board) {
    vector<Card> concatenate_vec(private_hand);
    concatenate_vec.insert(concatenate_vec.end(),public_board.begin(),public_board.end());
    return this->getRank(concatenate_vec);
}

int Dic5Compairer::get_rank(vector<int> private_hand, vector<int> public_board) {
    vector<int> concatenate_vec(private_hand);
    concatenate_vec.insert(concatenate_vec.end(),public_board.begin(),public_board.end());
    return this->getRank(concatenate_vec);
}

int Dic5Compairer::get_rank(uint64_t private_hand, uint64_t public_board) {
    return this->get_rank(Card::long2board(private_hand),Card::long2board(public_board));
}

