#ifndef QSOLVERJOB_H
#define QSOLVERJOB_H
#include "include/tools/qdebugstream.h"
#include "stdio.h"
#include <QThread>
#include "include/tools/CommandLineTool.h"
#include <QTextEdit>
#include <QPlainTextEdit>
#include "qstextedit.h"

class QSolverJob : public QThread
{
    Q_OBJECT
private:
    QSTextEdit * textEdit;
public:
    enum Mode{
        HOLDEM,
        SHORTDECK
    };
    Mode mode = Mode::HOLDEM;

    enum MissionType{
        LOADING,
        SOLVING,
        BUILDTREE,
        SAVING
    };
    MissionType current_mission = MissionType::LOADING;
    string resource_dir;
    PokerSolver ps_holdem,ps_shortdeck;
    float oop_commit=5;
    float ip_commit=5;
    int current_round=1;
    int raise_limit=4;
    int thread_number=1;
    float small_blind=0.5;
    float big_blind=1;
    float stack=20 + 5;
    float allin_threshold = 0.67;
    string range_ip;
    string range_oop;
    string board;
    float accuracy;
    int max_iteration=100;
    int use_isomorphism=1;
    int print_interval=10;
    int dump_rounds = 2;
    shared_ptr<GameTreeBuildingSettings> gtbs;

    void run();
    void loading();
    void solving();
    void saving();
    void build_tree();
    void setContext(QSTextEdit * textEdit);
    string savefile;
};
#endif // QSOLVERJOB_H
