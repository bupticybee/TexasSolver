#include "qstextedit.h"

QSTextEdit::QSTextEdit(QWidget *parent) : QTextEdit(parent)
{
    connect(this,SIGNAL(message_signal(string)),this,SLOT(message_slot(string)));
}

void QSTextEdit::log_with_signal(string message){
    emit message_signal(message);
}


void QSTextEdit::message_slot(string message){
    this->append(message.c_str());
}
