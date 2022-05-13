#ifndef QSOLVERJOB_H
#define QSOLVERJOB_H
#include "include/tools/qdebugstream.h"
#include "stdio.h"
#include <QThread>
#include "include/tools/CommandLineTool.h"
#include <QTextEdit>
#include <QPlainTextEdit>
#include "qstextedit.h"
#include <QDebug>
#include <QSettings>

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
    int use_halffloats=0;
    int print_interval=10;
    int dump_rounds = 2;
    shared_ptr<GameTreeBuildingSettings> gtbs;

    PokerSolver* get_solver();
    void run();
    void loading();
    void solving();
    void stop();
    void saving();
    void build_tree();
    long long estimate_tree_memory(QString range1,QString range2,QString board);
    void setContext(QSTextEdit * textEdit);
    QString savefile;
};
#endif // QSOLVERJOB_H
