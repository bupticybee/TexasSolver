#include "include/tools/logger.h"
#include <chrono>

void get_localtime(char *buf, size_t n, const char *format) {
    using namespace std::chrono;
    system_clock::time_point tp = system_clock::now();
    time_t now = system_clock::to_time_t(tp);
    // time(&now);
    int ms = duration_cast<milliseconds>(tp.time_since_epoch()).count() - now * 1000;
    tm tm_now;
#ifdef _MSC_VER
    localtime_s(&tm_now, &now);
#else
    localtime_r(&now, &tm_now);
#endif
    // strftime(buf, n, format, &tm_now);
    snprintf(buf, n, format, tm_now.tm_year+1900, tm_now.tm_mon+1, tm_now.tm_mday,
            tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec, ms);
}

string get_localtime() {
    char buf[25];
    get_localtime(buf, sizeof(buf), "%d_%02d_%02d_%02d_%02d_%02d.%03d");
    return string(buf);
}

void Logger::log(const char *format, ...) {
    if(timestamp) log_time();
    va_list args;
    va_start(args, format);
    if(file) {
        vfprintf(file, format, args);
        if((++step) == period) {
            step = 0;
            fflush(file);
        }
        if(new_line) fprintf(file, "\n");
#ifdef __GNUC__
        if(cmd) {
            va_end(args);
            va_start(args, format);
        }
#endif
    }
    if(cmd) {
        vprintf(format, args);
        if(new_line) printf("\n");
    }
    va_end(args);
}

void Logger::log_time() {
    char buf[28];
    get_localtime(buf, sizeof(buf), "%d-%02d-%02d %02d:%02d:%02d.%03d ");
    if(file) fprintf(file, buf);
    if(cmd) printf(buf);
}