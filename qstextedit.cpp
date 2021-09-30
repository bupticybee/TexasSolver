#include "qstextedit.h"
#include <iostream>

QSTextEdit::QSTextEdit(QWidget *parent) : QTextEdit(parent)
{
    connect(this,SIGNAL(message_signal(const string&)),this,SLOT(message_slot(const string)));
}

void QSTextEdit::log_with_signal(string message){
#if defined(Q_OS_WIN)
    this->append(message.c_str());
#else
    emit message_signal(message);
#endif
}


void QSTextEdit::message_slot(const string& message){
    this->append(message.c_str());
}
