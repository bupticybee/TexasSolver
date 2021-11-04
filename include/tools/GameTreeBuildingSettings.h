//
// Created by bytedance on 2.6.21.
//

#ifndef BINDSOLVER_GAMETREEBUILDINGSETTINGS_H
#define BINDSOLVER_GAMETREEBUILDINGSETTINGS_H
#include "include/tools/StreetSetting.h"

class GameTreeBuildingSettings {
public:
    GameTreeBuildingSettings(
            StreetSetting flop_ip,
            StreetSetting turn_ip,
            StreetSetting river_ip,
            StreetSetting flop_oop,
            StreetSetting turn_oop,
            StreetSetting river_oop) ;
    StreetSetting flop_ip;
    StreetSetting turn_ip;
    StreetSetting river_ip;
    StreetSetting flop_oop;
    StreetSetting turn_oop;
    StreetSetting river_oop;
    StreetSetting& get_setting(string player,string round);
};

#endif //BINDSOLVER_GAMETREEBUILDINGSETTINGS_H
