#include "include/card_format.h"

string toFormattedString(Card &card) {
    QString qString = QString::fromStdString(card.getCard());
    qString = qString.replace("c", "♣️");
    qString = qString.replace("d", "♦️");
    qString = qString.replace("h", "♥️");
    qString = qString.replace("s", "♠️");
    return qString.toStdString();
}

QString toFormattedHtml(Card &card) {
    QString qString = QString::fromStdString(card.getCard());
    if(qString.contains("c"))
        qString = qString.replace("c", QString::fromLocal8Bit("<span style=\"color:black;\">&#9827;<\/span>"));
    else if(qString.contains("d"))
        qString = qString.replace("d", QString::fromLocal8Bit("<span style=\"color:red;\">&#9830;<\/span>"));
    else if(qString.contains("h"))
        qString = qString.replace("h", QString::fromLocal8Bit("<span style=\"color:red;\">&#9829;<\/span>"));
    else if(qString.contains("s"))
        qString = qString.replace("s", QString::fromLocal8Bit("<span style=\"color:black;\">&#9824;<\/span>"));
    return qString;
}

QString boardCards2html(vector<Card>& cards){
    QString ret_html = "";
    for(auto one_card:cards){
        if(one_card.empty())continue;
        ret_html += toFormattedHtml(one_card);
    }
    return ret_html;
}
