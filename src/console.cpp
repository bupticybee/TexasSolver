//
// Created by bytedance on 9.6.21.
//
#include "include/tools/CommandLineTool.h"
#include "include/tools/argparse.hpp"

int main_backup(int argc,const char **argv) {
    ArgumentParser parser;

    parser.addArgument("-i", "--input_file", 1, true);
    parser.addArgument("-r", "--resource_dir", 1, true);
    parser.addArgument("-m", "--mode", 1, true);

    parser.parse(argc, argv);

    string input_file = parser.retrieve<string>("input_file");
    string resource_dir = parser.retrieve<string>("resource_dir");
    if(resource_dir.empty()){
        resource_dir = "./resources";
    }
    string mode = parser.retrieve<string>("mode");
    if(mode.empty()){mode = "holdem";}
    if(mode != "holdem" && mode != "shortdeck")
        throw runtime_error(tfm::format("mode %s error, not in ['holdem','shortdeck']",mode));

    if(input_file.empty()) {
        CommandLineTool clt = CommandLineTool(mode,resource_dir);
        clt.startWorking();
    }else{
        cout << "EXEC FROM FILE" << endl;
        CommandLineTool clt = CommandLineTool(mode,resource_dir);
        clt.execFromFile(input_file);
    }
}
