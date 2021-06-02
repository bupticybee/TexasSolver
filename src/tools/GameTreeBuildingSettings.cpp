//
// Created by bytedance on 2.6.21.
//
#include "GameTree.h"
#include "tools/GameTreeBuildingSettings.h"

GameTreeBuildingSettings::GameTreeBuildingSettings(
        StreetSetting flop_ip,
        StreetSetting turn_ip,
        StreetSetting river_ip,
        StreetSetting flop_oop,
        StreetSetting turn_oop,
        StreetSetting river_oop):flop_ip(flop_ip),turn_ip(turn_ip),river_ip(river_ip),flop_oop(flop_oop),turn_oop(turn_oop),river_oop(river_oop) {
}