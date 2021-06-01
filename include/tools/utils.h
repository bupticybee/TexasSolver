//
// Created by bytedance on 20.4.21.
//

#ifndef BINDSOLVER_UTILS_H
#define BINDSOLVER_UTILS_H
#include <ranges/PrivateCards.h>
#include <compairer/Compairer.h>
#include <Deck.h>
#include <ranges/RiverRangeManager.h>
#include <ranges/PrivateCardsManager.h>
#include <trainable/CfrPlusTrainable.h>
#include <trainable/DiscountedCfrTrainable.h>
#include <nodes/ChanceNode.h>
#include <nodes/TerminalNode.h>
#include <nodes/ShowdownNode.h>

void exchange_color(vector<float>& value,vector<PrivateCards> range,int rank1,int ran2);

#endif //BINDSOLVER_UTILS_H
