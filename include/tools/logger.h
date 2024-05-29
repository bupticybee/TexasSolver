#if !defined(_LOGGER_H_)
#define _LOGGER_H_

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string>

using std::string;

void get_localtime(char *buf, size_t n, const char *format);
string get_localtime();

class Logger {
public:
    Logger(bool cmd, const char *path, const char *mode = "w", bool timestamp = false, bool new_line = true, int period = 10)
        :cmd(cmd), timestamp(timestamp), new_line(new_line), period(period) {
        if(path) {
            errno_t err = fopen_s(&file, path, mode);
            if(err) printf("%d\n", err);
            if(!file) printf("create file %s failed\n", path);
        }
    }
    virtual ~Logger() {
        if(file) {
            fflush(file);
            fclose(file);
        }
    }
    virtual void log(const char *format, ...);
    void flush() {
        if(file) fflush(file);
    }
protected:
    void log_time();
    int step = 0, period = 10;
    FILE *file = nullptr;
    bool timestamp = false;
    bool cmd = true;
    bool new_line = true;
};

#endif // _LOGGER_H_
