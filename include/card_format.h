#if !defined(_CARD_FORMAT_H_)
#define _CARD_FORMAT_H_

#include <QString>
#include "include/Card.h"

string toFormattedString(Card &card);
QString toFormattedHtml(Card &card);
QString boardCards2html(vector<Card>& cards);

#endif // _CARD_FORMAT_H_
