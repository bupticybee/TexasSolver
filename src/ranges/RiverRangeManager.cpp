//
// Created by Xuefeng Huang on 2020/1/31.
//

#include "include/ranges/RiverRangeManager.h"

#include <utility>

RiverRangeManager::RiverRangeManager() = default;

RiverRangeManager::RiverRangeManager(shared_ptr<Compairer> handEvaluator) {
    this->handEvaluator = std::move(handEvaluator);
    this->maplock = std::make_shared<std::mutex>();
}

const vector<RiverCombs> &
RiverRangeManager::getRiverCombos(int player, const vector<PrivateCards> &riverCombos, const vector<int> &board) {
    uint64_t board_long = Card::boardInts2long(board);
    return this->getRiverCombos(player,riverCombos,board_long);
}

const vector<RiverCombs> &
RiverRangeManager::getRiverCombos(int player, const vector<PrivateCards> &preflopCombos, uint64_t board_long) {
    unordered_map<uint64_t , vector<RiverCombs>>* riverRanges;

    if (player == 0)
        riverRanges = &p1RiverRanges;
    else if (player == 1)
        riverRanges = &p2RiverRanges;
    else
        throw runtime_error(tfm::format("player %s not found",player));

    uint64_t key = board_long;

    this->maplock->lock();
    if (riverRanges->find(key) != riverRanges->end()) {
        const vector<RiverCombs> &retval = (*riverRanges)[key];
        this->maplock->unlock();
        return retval;
    }
    this->maplock->unlock();

    int count = 0;

    for (auto one_hand : preflopCombos) {
        if (!Card::boardsHasIntercept(
                one_hand.toBoardLong(), board_long
        ))
            count++;
    }

    int index = 0;
    vector<RiverCombs> riverCombos = vector<RiverCombs>(count);

    for (std::size_t hand = 0; hand < preflopCombos.size(); hand++)
    {
        PrivateCards preflopCombo = preflopCombos[hand];


        if (Card::boardsHasIntercept(
                preflopCombo.toBoardLong(), board_long
        )){
            continue;
        }

        int rank = this->handEvaluator->get_rank(preflopCombo.toBoardLong(),board_long);
        RiverCombs riverCombo = RiverCombs(Card::long2board(board_long),preflopCombo,rank, hand);
        riverCombos[index++] = riverCombo;
    }

    std::sort(riverCombos.begin(),riverCombos.end(),[ ]( const RiverCombs& lhs, const RiverCombs& rhs )
    {
        return lhs.rank > rhs.rank;
    });

    this->maplock->lock();
    (*riverRanges)[key] =  std::move(riverCombos);
    this->maplock->unlock();

    return (*riverRanges)[key];
}
