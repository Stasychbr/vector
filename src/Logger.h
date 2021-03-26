#pragma once
#include <cstdio>
#include "../include/ILogger.h"

class Logger : public ILogger {
private:
    FILE* stream;
    const char* getMessage(RC code);
    const char* getLevel(Level level);
public:
    Logger(const char* const& filename, bool overwrite = true);
    virtual RC log(RC code, Level level, const char* const& srcfile, const char* const& function, int line) override;
    virtual RC log(RC code, Level level) override;
    ~Logger();
};
