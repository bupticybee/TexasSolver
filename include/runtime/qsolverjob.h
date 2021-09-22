#ifndef QSOLVERJOB_H
#define QSOLVERJOB_H
#include "include/tools/qdebugstream.h"
#include "stdio.h"
#include <QThread>
#include "include/tools/CommandLineTool.h"
#include <QTextEdit>
#include <QPlainTextEdit>

class QSolverJob : public QThread
{
    Q_OBJECT
private:
    QTextEdit * textEdit;
public:
    void run();
    void setContext(QTextEdit * textEdit);
};
#endif // QSOLVERJOB_H
