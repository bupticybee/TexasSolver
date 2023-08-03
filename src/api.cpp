//Derived from console.cpp
//Enables C FFI (foreign function interface) API calls from languages that support it
//Should be compiled as dynamic linked library (*.dll) on Windows, shared object on linux (*.so) and dynamic library on MacOS (*.dylib)

#if defined(_WIN32) || defined(_WIN64) //Windows
   #define EXPORT __declspec(dllexport)
#else //UNIX
   #define EXPORT extern "C" __attribute__((visibility("default"))) __attribute__((used))
#endif

#include "include/tools/CommandLineTool.h"
#include <string>

EXPORT
int api(const char * input_file, const char * resource_dir = "./resources", const char * mode = "holdem") {
    string input_file_ = input_file;
    string resource_dir_ = resource_dir;
    string mode_ = mode;

    if(mode_ != "holdem" && mode_ != "shortdeck")
        throw runtime_error(tfm::format("mode %s error, not in ['holdem','shortdeck']", mode_));

    if(input_file_.empty()) {
        CommandLineTool clt = CommandLineTool(mode_, resource_dir_);
        clt.startWorking();
    }else{
        cout << "EXEC FROM FILE" << endl;
        CommandLineTool clt = CommandLineTool(mode_, resource_dir_);
        clt.execFromFile(input_file_);
    }
}