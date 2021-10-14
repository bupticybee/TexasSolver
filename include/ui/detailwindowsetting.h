#ifndef DETAILWINDOWSETTING_H
#define DETAILWINDOWSETTING_H

class DetailWindowSetting{
public:
    enum DetailWindowMode{
        RANGE_OOP,
        RANGE_IP,
        EV,
        EV_ONLY,
        STRATEGY
    };
    DetailWindowMode mode;
    int grid_i = -1;
    int grid_j = -1;
    DetailWindowSetting();
};

#endif // DETAILWINDOWSETTING_H
