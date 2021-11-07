//
// Created by Xuefeng Huang on 2020/1/31.
//

#ifndef TEXASSOLVER_RIVERRANGEMANAGER_H
#define TEXASSOLVER_RIVERRANGEMANAGER_H

#include "RiverCombs.h"
#include <unordered_map>
#include <include/compairer/Compairer.h>
#include <include/compairer/Dic5Compairer.h>
#include <mutex>
#include <memory>

class RiverRangeManager {
public:
    RiverRangeManager();
    RiverRangeManager(shared_ptr<Compairer> handEvaluator);
    const vector<RiverCombs>& getRiverCombos(int player, const vector<PrivateCards>& riverCombos, const vector<int>& board);
    const vector<RiverCombs>& getRiverCombos(int player, const vector<PrivateCards>& riverCombos, uint64_t board_long);
private:
    unordered_map<uint64_t , vector<RiverCombs>> p1RiverRanges;
    unordered_map<uint64_t , vector<RiverCombs>> p2RiverRanges;
    shared_ptr<Compairer> handEvaluator;
    shared_ptr<mutex> maplock;
};


#endif //TEXASSOLVER_RIVERRANGEMANAGER_H
