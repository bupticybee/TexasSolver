//
// Created by bytedance on 2.6.21.
//
#include "include/GameTree.h"
#include "include/tools/GameTreeBuildingSettings.h"

GameTreeBuildingSettings::GameTreeBuildingSettings(
        StreetSetting flop_ip,
        StreetSetting turn_ip,
        StreetSetting river_ip,
        StreetSetting flop_oop,
        StreetSetting turn_oop,
        StreetSetting river_oop):flop_ip(flop_ip),turn_ip(turn_ip),river_ip(river_ip),flop_oop(flop_oop),turn_oop(turn_oop),river_oop(river_oop) {
}

StreetSetting& GameTreeBuildingSettings::get_setting(string player,string round){
    if(player == "ip" && round == "flop") return flop_ip;
    else if(player == "ip" && round == "turn") return turn_ip;
    else if(player == "ip" && round == "river") return river_ip;
    else if(player == "oop" && round == "flop") return flop_oop;
    else if(player == "oop" && round == "turn") return turn_oop;
    else if(player == "oop" && round == "river") return river_oop;
    else throw runtime_error("get_setting not valid");
}
