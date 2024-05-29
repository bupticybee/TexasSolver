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
int api(const char * input_file, const char * resource_dir = "./resources", const char * mode = "holdem", const char *log_file = "") {
    string input_file_ = input_file;
    string resource_dir_ = resource_dir;
    string mode_ = mode;
    string log_file_ = log_file;

    return cmd_api(input_file_, resource_dir_, mode_, log_file_);
}