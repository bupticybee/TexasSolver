#include "qstextedit.h"
#include <iostream>

QSTextEdit::QSTextEdit(QWidget *parent) : QTextEdit(parent)
{
    connect(this,SIGNAL(message_signal(const QString&)),this,SLOT(message_slot(const QString)));
}

void QSTextEdit::log_with_signal(QString message){
    emit message_signal(message);
}


void QSTextEdit::message_slot(const QString& message){
    this->append(message);
}
