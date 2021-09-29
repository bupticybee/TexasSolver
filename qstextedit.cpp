#include "qstextedit.h"
#include <iostream>

QSTextEdit::QSTextEdit(QWidget *parent) : QTextEdit(parent)
{
    connect(this,SIGNAL(message_signal(const string&)),this,SLOT(message_slot(const string)),Qt::DirectConnection);
}

void QSTextEdit::log_with_signal(string message){
    emit message_signal(message);
}


void QSTextEdit::message_slot(const string& message){
    this->append(message.c_str());
}
