#ifndef QSTEXTEDIT_H
#define QSTEXTEDIT_H

#include <QObject>
#include <QTextEdit>
#include <string>
using namespace std;

class QSTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit QSTextEdit(QWidget *parent = nullptr);
    void log_with_signal(string message);

signals:
    void message_signal(string message);

public slots:
    void message_slot(string message);
};

#endif // QSTEXTEDIT_H
