//
// Created by bytedance on 9.6.21.
//
#include "include/tools/CommandLineTool.h"
#include "include/tools/argparse.hpp"

int main(int argc,const char **argv) {
    ArgumentParser parser;

    parser.addArgument("-i", "--input_file", 1, true);
    parser.addArgument("-r", "--resource_dir", 1, true);
    parser.addArgument("-m", "--mode", 1, true);
    parser.addArgument("-l", "--log", 1, true);

    parser.parse(argc, argv);

    string input_file = parser.retrieve<string>("input_file");
    string resource_dir = parser.retrieve<string>("resource_dir");
    string mode = parser.retrieve<string>("mode");
    string log_file = parser.retrieve<string>("log");
    return cmd_api(input_file, resource_dir, mode, log_file);
}
