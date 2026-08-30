//
// Created by Xuefeng Huang on 2020/1/31.
//

#ifndef TEXASSOLVER_RIVERRANGEMANAGER_H
#define TEXASSOLVER_RIVERRANGEMANAGER_H

#include "RiverCombs.h"
#include <unordered_map>
#include <compairer/Compairer.h>
#include <compairer/Dic5Compairer.h>
#include <mutex>

class RiverRangeManager {
public:
    RiverRangeManager();
    RiverRangeManager(shared_ptr<Compairer> handEvaluator);
    const vector<RiverCombs>& getRiverCombos(int player, const vector<PrivateCards>& riverCombos, const vector<int>& board);
    const vector<RiverCombs>& getRiverCombos(int player, const vector<PrivateCards>& riverCombos, uint64_t board_long);
    // Marks the cache as complete and immutable: once frozen, getRiverCombos reads the
    // map without taking maplock (concurrent lock-free reads of an unordered_map are
    // safe as long as nothing writes to it concurrently, which freeze() guarantees by
    // contract -- callers must fully populate the cache, single-threaded, before calling
    // this). A miss after freezing throws rather than silently falling back to the locked
    // path, so an incomplete prefetch fails loudly instead of corrupting concurrent reads.
    void freeze();
private:
    unordered_map<uint64_t , vector<RiverCombs>> p1RiverRanges;
    unordered_map<uint64_t , vector<RiverCombs>> p2RiverRanges;
    shared_ptr<Compairer> handEvaluator;
    shared_ptr<mutex> maplock;
    bool cache_frozen = false;
};


#endif //TEXASSOLVER_RIVERRANGEMANAGER_H
