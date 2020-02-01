#include "../include/library.h"

#include <iostream>
#include <vector>



vector<string> string_split(string strin,char split){
    vector<string> retval;
    stringstream ss(strin);
    string token;
    while (getline(ss,token, split))
    {
        retval.push_back(token);
    }
    return retval;
};

uint64_t timeSinceEpochMillisec() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
};

int random(int min, int max) //range : [min, max)
{
    return min + rand() % (( max ) - min);
}
