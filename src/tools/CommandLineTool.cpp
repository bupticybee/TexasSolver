﻿//
// Created by bytedance on 7.6.21.
//
#include "include/tools/CommandLineTool.h"
// #include <QString>
#include <fstream>
#include <unordered_set>
#include <algorithm>

CommandLineTool::CommandLineTool() {
    // string logfile_name = "../resources/outputs/outputs_log.txt";
    // this->ps = PokerSolver(mode, resource_dir);

    // StreetSetting gbs_flop_ip = StreetSetting(vector<float>{},vector<float>{},vector<float>{},true);
    // StreetSetting gbs_turn_ip = StreetSetting(vector<float>{},vector<float>{},vector<float>{},true);
    // StreetSetting gbs_river_ip = StreetSetting(vector<float>{},vector<float>{},vector<float>{},true);

    // StreetSetting gbs_flop_oop = StreetSetting(vector<float>{},vector<float>{},vector<float>{},true);
    // StreetSetting gbs_turn_oop = StreetSetting(vector<float>{},vector<float>{},vector<float>{},true);
    // StreetSetting gbs_river_oop = StreetSetting(vector<float>{},vector<float>{},vector<float>{},true);

    // this->gtbs = make_shared<GameTreeBuildingSettings>(gbs_flop_ip,gbs_turn_ip,gbs_river_ip,gbs_flop_oop,gbs_turn_oop,gbs_river_oop);
    //ps.build_game_tree(oop_commit,ip_commit,current_round,raise_limit,small_blind,big_blind,stack,*gtbs.get(),allin_threshold);
    //cout << "build tree finished" << endl;
    /*
    ps.getGameTree()->recurrentPrintTree(ps.getGameTree()->getRoot(),0,4);
    ps.train(
            //"AA:0.001,KK:0.001,QQ:0.001,JJ:0.001,TT:0.001,99:0.001,88:0.001,77:0.001,66:0.001,55:0.001,44:0.24,33:0.2,22:0.5,AK:0.001,AQ:0.001,AJ:0.001,ATs:0.16,ATo:0.3,A9s,A9o:0.15,A8s:0.15,A8o:0.001,A7s:0.38,A7o:0.001,A6s:0.35,A6o:0.001,A5s:0.35,A5o:0.001,A4s:0.35,A4o:0.001,A3s:0.5,A3o:0.001,A2:0.001,KQs:0.05,KQo:0.001,KJs:0.4,KJo:0.5,KTs:0.6,KTo:0.2,K9s,K9o:0.001,K8s:0.55,K8o:0.001,K7:0.001,K6:0.001,K5:0.001,K4:0.001,K3:0.001,K2:0.001,QJs:0.001,QJo:0.58,QTs:0.1,QTo:0.08,Q9s:0.75,Q9o:0.001,Q8:0.001,Q7:0.001,Q6:0.001,Q5:0.001,Q4:0.001,Q3:0.001,Q2:0.001,JT:0.001,J9s:0.03,J9o:0.001,J8:0.001,J7:0.001,J6:0.001,J5:0.001,J4:0.001,J3:0.001,J2:0.001,T9s:0.06,T9o:0.001,T8s:0.55,T8o:0.001,T7:0.001,T6:0.001,T5:0.001,T4:0.001,T3:0.001,T2:0.001,98s:0.2,98o:0.001,97:0.001,96:0.001,95:0.001,94:0.001,93:0.001,92:0.001,87s:0.3,87o:0.001,86:0.001,85:0.001,84:0.001,83:0.001,82:0.001,76s:0.3,76o:0.001,75:0.001,74:0.001,73:0.001,72:0.001,65s:0.37,65o:0.001,64:0.001,63:0.001,62:0.001,54:0.001,53:0.001,52:0.001,43:0.001,42:0.001,32:0.001",
            //"AA,KK,QQ,JJ,TT,99,88,77,66,55,44:0.925000012,33:0.95,22:0.200050003,AK,AQ,AJ,AT,A9,A8,A7,A6s,A6o:0.849999994,A5s,A5o:0.949999988,A4s,A4o:0.0500500005,A3s,A3o:0.0250500004,A2s,A2o:0.0250500004,KQ,KJ,KT,K9s:0.925000012,K9o,K8s,K8o:0.337499997,K7s,K7o:0.0500500005,K6s,K6o:0.00055000005,K5s,K5o:0.00055000005,K4s,K4o:0.00055000005,K3s,K3o:0.00055000005,K2s:0.200050003,K2o:0.00055000005,QJ,QT,Q9s,Q9o:0.949999988,Q8s,Q8o:0.0500500005,Q7s,Q7o:0.00055000005,Q6s,Q6o:0.00055000005,Q5s,Q5o:0.00055000005,Q4s:0.799999988,Q4o:0.00055000005,Q3s:0.0500500005,Q3o:0.00055000005,Q2s:0.0250500004,Q2o:0.00055000005,JT,J9:0.900000006,J8s,J8o:0.0500500005,J7s,J7o:0.00055000005,J6s,J6o:0.00055000005,J5s:0.524999988,J5o:0.00055000005,J4s:0.0500500005,J4o:0.00055000005,J3s:0.0250500004,J3o:0.00055000005,J2:0.00055000005,T9s,T9o:0.949999988,T8s,T8o:0.0250500004,T7s,T7o:0.00055000005,T6s,T6o:0.00055000005,T5s:0.0500500005,T5o:0.00055000005,T4s:0.0250500004,T4o:0.00055000005,T3:0.00055000005,T2:0.00055000005,98s,98o:0.300000002,97s,97o:0.00055000005,96s,96o:0.00055000005,95s:0.0500500005,95o:0.00055000005,94:0.00055000005,93:0.00055000005,92:0.00055000005,87s,87o:0.00055000005,86s,86o:0.00055000005,85s:0.949999988,85o:0.00055000005,84:0.00055000005,83:0.00055000005,82:0.00055000005,76s,76o:0.00055000005,75s,75o:0.00055000005,74s:0.0500500005,74o:0.00055000005,73:0.00055000005,72:0.00055000005,65s,65o:0.00055000005,64s:0.25005,64o:0.00055000005,63:0.00055000005,62:0.00055000005,54s:0.949999988,54o:0.00055000005,53s:0.0250500004,53o:0.00055000005,52:0.00055000005,43s:0.0500500005,43o:0.00055000005,42:0.00055000005,32:0.00055000005",
            "AA,KK,QQ,JJ,TT,99:0.75,88:0.75,77:0.5,66:0.25,55:0.25,AK,AQs,AQo:0.75,AJs,AJo:0.5,ATs:0.75,A6s:0.25,A5s:0.75,A4s:0.75,A3s:0.5,A2s:0.5,KQs,KQo:0.5,KJs,KTs:0.75,K5s:0.25,K4s:0.25,QJs:0.75,QTs:0.75,Q9s:0.5,JTs:0.75,J9s:0.75,J8s:0.75,T9s:0.75,T8s:0.75,T7s:0.75,98s:0.75,97s:0.75,96s:0.5,87s:0.75,86s:0.5,85s:0.5,76s:0.75,75s:0.5,65s:0.75,64s:0.5,54s:0.75,53s:0.5,43s:0.5",
            "QQ:0.5,JJ:0.75,TT,99,88,77,66,55,44,33,22,AKo:0.25,AQs,AQo:0.75,AJs,AJo:0.75,ATs,ATo:0.75,A9s,A8s,A7s,A6s,A5s,A4s,A3s,A2s,KQ,KJ,KTs,KTo:0.5,K9s,K8s,K7s,K6s,K5s,K4s:0.5,K3s:0.5,K2s:0.5,QJ,QTs,Q9s,Q8s,Q7s,JTs,JTo:0.5,J9s,J8s,T9s,T8s,T7s,98s,97s,96s,87s,86s,76s,75s,65s,64s,54s,53s,43s",
            "Qs,Jh,2h",
            logfile_name,
            300,
            10,
            "discounted_cfr",
            -1,
            true,
            8
    );
     */
}

void CommandLineTool::startWorking(PokerSolver *ps) {
    string input_line;
    while(cin) {
        getline(cin, input_line);
        this->processCommand(input_line, ps);
    };
}

void CommandLineTool::execFromFile(const char *input_file, PokerSolver *ps) {
    std::ifstream infile(input_file);
    std::string input_line;
    while (std::getline(infile, input_line))
    {
        this->processCommand(input_line, ps);
    }

}

void split(const string& s, char delimiter, vector<string>& v) {
    size_t i = s.find_first_not_of(delimiter), j = 0;
    while (i != string::npos) {
        j = s.find_first_of(delimiter, i+1);
        if(j == string::npos) j = s.size();
        v.push_back(s.substr(i, j-i));
        i = s.find_first_not_of(delimiter, j+1);
    }
}

template<class T>
string tostring(T val) {
    string s = to_string(val);
    for(size_t i = s.size() - 1; i > 0; i--) {
        if(s[i] == '0') s.pop_back();
        else if(s[i] == '.') {
            s.pop_back();
            break;
        }
        else break;
    }
    return s;
}

template<class T>
string tostring_oss(T val) {
    ostringstream oss;
    oss << val;
    return oss.str();
}

void join(const vector<float> &vec, char delimiter, string &out) {
    size_t n = vec.size();
    if(n) out += tostring(vec[0]);
    for(int i = 1; i < n; i++) {
        out += delimiter;
        out += tostring(vec[i]);
    }
}

bool CommandLineTool::set_board(string &str) {
    board = str;
    vector<string> board_str_arr = string_split(board,',');
    if(board_str_arr.size() == 3){
        this->current_round = 1;
    }else if(board_str_arr.size() == 4){
        this->current_round = 2;
    }else if(board_str_arr.size() == 5){
        this->current_round = 3;
    }else{
        // throw runtime_error(tfm::format("board %s not recognized",this->board));
        return false;
    }
    return true;
}

bool CommandLineTool::set_bet_sizes(string &str, char delimiter, vector<float> *sizes) {
    vector<string> params;
    split(str, delimiter, params);
    int start = (sizes != nullptr ? 0 : 3);
    if(params.size() < start) {
        // throw runtime_error("param number error");
        return false;
    }
    if(sizes == nullptr) {
        // oop,turn,bet,30,70,100
        StreetSetting& streetSetting = gtbs.get_setting(params[0], params[1]);
        string &bet_type = params[2];
        if(bet_type == "allin") {
            if(params.size() == start) streetSetting.allin = true;
            else streetSetting.allin = stoi(params[start]);
        }
        else if(bet_type == "bet") sizes = &(streetSetting.bet_sizes);
        else if(bet_type == "raise") sizes = &(streetSetting.raise_sizes);
        else if(bet_type == "donk") sizes = &(streetSetting.donk_sizes);
        else return false;
    }
    if(sizes != nullptr) {
        sizes->clear();
        std::unordered_set<float> seen;
        for(std::size_t i = start; i < params.size(); i++) {
            float val = stof(params[i]);
            if(seen.count(val)) continue;
            sizes->push_back(val);
            seen.insert(val);
        }
        std::sort(sizes->begin(), sizes->end());
    }
    return true;
}

// void show_bet_sizes(std::ofstream &out, const char *player, const char *round, const char *type, vector<float> &sizes) {
//     string s;
//     join(sizes, ',', s);
//     out << "set_bet_sizes " << player << ',' << round << ',' << type;
//     if(s.size()) out << ',' << s;
//     out << endl;
// }
// void show_bet_sizes(std::ofstream &out, const char *player, const char *round, const char *type, bool allin) {
//     out << "set_bet_sizes " << player << ',' << round << ',' << type << ',' << allin;
// }

void CommandLineTool::processCommand(string &input, PokerSolver *ps) {
    vector<string> contents;
    if(input.empty() || input[0] == '#') return;
    split(input,' ',contents);
    if(contents.size() == 0) contents = {input};
    if(contents.size() > 2 || contents.size() < 1)throw runtime_error(tfm::format("command not valid: %s",input));
    string command = contents[0];
    string paramstr = contents.size() == 1 ? "" : contents[1];
    if(command == "set_pot"){
        set_pot(stof(paramstr));
    }else if(command == "set_effective_stack"){
        set_effective_stack(stof(paramstr));
    }else if(command == "set_board"){
        set_board(paramstr);
    }else if(command == "set_range_ip"){
        this->range_ip = paramstr;
    }else if(command == "set_range_oop"){
        this->range_oop = paramstr;
    }else if(command == "set_bet_sizes"){
        set_bet_sizes(paramstr);
    }else if(command == "set_raise_limit"){
        this->raise_limit = stoi(paramstr);
    }else if(command == "set_accuracy"){
        this->accuracy = stof(paramstr);
    }else if(command == "set_allin_threshold"){
        this->allin_threshold = stof(paramstr);
    }else if(command == "set_thread_num"){
        this->thread_num = stoi(paramstr);
    }else if(command == "build_tree"){
        build_tree(ps);
    }else if(command == "set_max_iteration"){
        this->max_iteration = stoi(paramstr);
    }else if(command == "set_use_isomorphism"){
        this->use_isomorphism = stoi(paramstr);
    }else if(command == "set_print_interval"){
        this->print_interval = stoi(paramstr);
    }else if(command == "start_solve"){
        start_solve(ps);
    }else if(command == "dump_setting"){
        dump_setting(paramstr.c_str());
    }else if(command == "dump_result"){
        res_file = paramstr;
        if(!ps) return;
        ps->dump_strategy(res_file, this->dump_rounds);
    }else if(command == "set_dump_rounds"){
        this->dump_rounds = stoi(paramstr);
    }else if(command == "estimate_tree_memory"){
        if(!ps) return;
        if(range_ip.empty() || range_oop.empty() || board.empty()) {
            // cout << "Please set range_ip, range_oop and board first." << endl;
            logger->log("Please set range_ip, range_oop and board first.");
            return;
        }
        shared_ptr<GameTree> game_tree = ps->get_game_tree();
        if(game_tree == nullptr) {
            // cout << "Please buld tree first." << endl;
            logger->log("Please buld tree first.");
            return;
        }
        long long size = ps->estimate_tree_memory(range_ip, range_oop, board);
        size *= sizeof(float);
        // cout << (float)size / (1024*1024) << " MB" << endl;
        logger->log("estimate_tree_memory: %f MB", (float)size / (1024*1024));
    }else if(command == "set_slice_cfr"){
        slice_cfr = stoi(paramstr);
    }else{
        // cout << "command not recognized: " << command << endl;
        logger->log("command not recognized: %s", command.c_str());
    }
}

void CommandLineTool::dump_setting(const char *file) {
    static vector<string> player {"oop","ip"};
    static vector<string> round {"flop","turn","river"};
    static vector<string> type {"bet","raise","donk","allin"};
    std::ofstream out(file);
    out << "set_pot " << get_pot() << endl;
    out << "set_effective_stack " << get_effective_stack() << endl;
    out << "set_board " << board << endl;
    out << "set_range_oop " << range_oop << endl;
    out << "set_range_ip " << range_ip << endl;

    for(size_t i = 0; i < player.size(); i++) {
        for(size_t j = 0; j < round.size(); j++) {
            for(size_t k = 0; k < type.size(); k++) {
                if(k == 2 && (i == 1 || j == 0)) continue;// no donk:ip, oop flop
                out << "set_bet_sizes " << player[i] << ',' << round[j] << ',' << type[k] << ',';
                StreetSetting& st = gtbs.get_setting(player[i], round[j]);
                if(k == 3) out << st.allin;
                else {
                    vector<float> &vec = (k == 0 ? st.bet_sizes : (k == 1 ? st.raise_sizes : st.donk_sizes));
                    string str;
                    join(vec, ',', str);
                    out << str;
                }
                out << endl;
            }
        }
    }
    out << "set_allin_threshold " << allin_threshold << endl;
    out << "set_raise_limit " << raise_limit << endl;
    out << "build_tree" << endl;
    out << "set_thread_num " << thread_num << endl;
    out << "set_accuracy " << accuracy << endl;
    out << "set_max_iteration " << max_iteration << endl;
    out << "set_print_interval " << print_interval << endl;
    out << "set_use_isomorphism " << use_isomorphism << endl;
    out << "set_slice_cfr " << slice_cfr << endl;
    out << "start_solve" << endl;
    out << "set_dump_rounds " << dump_rounds << endl;
    out << "dump_result " << res_file << endl;
    out.close();
}

int cmd_api(string &input_file, string &resource_dir, string &mode, string &log_file) {
    if(resource_dir.empty()){
        resource_dir = "./resources";
    }
    if(log_file.empty()) log_file = get_localtime() + ".txt";
    Logger logger(true, log_file.c_str(), "w+", true, true, 1);
    PokerMode poker_mode = PokerMode::UNKNOWN;
    if(mode.empty() || mode == "holdem") poker_mode = PokerMode::HOLDEM;
    else if(mode == "shortdeck") poker_mode = PokerMode::SHORTDECK;
    else throw runtime_error(tfm::format("mode %s error, not in ['holdem','shortdeck']", mode));
    PokerSolver ps = PokerSolver(poker_mode, resource_dir);
    CommandLineTool clt;
    clt.logger = &logger;
    ps.logger = &logger;
    if(input_file.empty()) {
        clt.startWorking(&ps);
    }else{
        // cout << "EXEC FROM FILE" << endl;
        logger.log("EXEC FROM FILE");
        clt.execFromFile(input_file.c_str(), &ps);
    }
    return 0;
}