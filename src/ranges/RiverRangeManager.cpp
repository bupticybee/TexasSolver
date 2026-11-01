//
// Created by Xuefeng Huang on 2020/1/31.
//

#include "ranges/RiverRangeManager.h"

#include <atomic>
#include <utility>

namespace {
// The map only ever gains entries once the solve is under way and its values never move, so each
// thread keeps its own direct-mapped view of it and takes the lock only for boards it hasn't seen.
struct RiverCacheSlot {
    uint64_t generation;
    uint64_t key;
    const vector<RiverCombs>* value;
};
constexpr int RIVER_CACHE_BITS = 12;
thread_local RiverCacheSlot river_cache[2][1 << RIVER_CACHE_BITS];
std::atomic<uint64_t> next_generation{1};
}

RiverRangeManager::RiverRangeManager() = default;

RiverRangeManager::RiverRangeManager(shared_ptr<Compairer> handEvaluator) {
    this->handEvaluator = std::move(handEvaluator);
    this->maplock = std::make_shared<std::mutex>();
    this->generation = next_generation.fetch_add(1);
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
        throw runtime_error(fmt::format("player {} not found",player));

    uint64_t key = board_long;

    RiverCacheSlot& slot = river_cache[player][(key * 0x9E3779B97F4A7C15ULL) >> (64 - RIVER_CACHE_BITS)];
    if (slot.generation == this->generation && slot.key == key) {
        return *slot.value;
    }

    // Reads and writes of the shared map must happen under the same lock.
    {
        std::lock_guard<std::mutex> guard(*this->maplock);
        auto it = riverRanges->find(key);
        if (it != riverRanges->end()) {
            slot = {this->generation, key, &it->second};
            return it->second;
        }
    }

    int count = 0;

    for (auto one_hand : preflopCombos) {
        if (!Card::boardsHasIntercept(
                one_hand.toBoardLong(), board_long
        ))
            count++;
    }

    int index = 0;
    vector<RiverCombs> riverCombos = vector<RiverCombs>(count);

    for (int hand = 0; hand < preflopCombos.size(); hand++)
    {
        PrivateCards preflopCombo = preflopCombos[hand];


        if (Card::boardsHasIntercept(
                preflopCombo.toBoardLong(), board_long
        )){
            continue;
        }

        int rank = this->handEvaluator->get_rank(preflopCombo.toBoardLong(),board_long);
        RiverCombs riverCombo = RiverCombs(preflopCombo,rank, hand);
        riverCombos[index++] = riverCombo;
    }

    std::sort(riverCombos.begin(),riverCombos.end(),[ ]( const RiverCombs& lhs, const RiverCombs& rhs )
    {
        return lhs.rank > rhs.rank;
    });

    // Two threads can miss on the same key. emplace keeps whichever vector was
    // published first, so a late thread gets the existing object and never
    // destroys a buffer a caller is still reading through.
    std::lock_guard<std::mutex> guard(*this->maplock);
    auto result = riverRanges->emplace(key, std::move(riverCombos));
    slot = {this->generation, key, &result.first->second};
    return result.first->second;
}
