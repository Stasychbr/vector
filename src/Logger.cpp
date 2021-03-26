#include "Logger.h"

const char* Logger::getMessage(RC code) {
    static const char* msg[(int)RC::AMOUNT + 1] = {
        "Unknown error",
        "Success",
        "Invalid argument",
        "Vectors' dimensions mismatch",
        "Index is out of bound",
        "Result of the operation is too large",
        "Result of the operation is NaN",
        "Unable to allocate memory",
        "Null pointer as argument",
        "Unable to find file",
        "Unable to find vector",
        "Input/Output error",
        "Found intersecting memory while copying instance",
    };
    return msg[(int)code];
}

const char* Logger::getLevel(Level level) {
    static const char* msg[] = {
        "SEVERE",
        "WARNING",
        "INFO",
    };
    if ((int)level >= sizeof(msg)) {
        return nullptr;
    }
    return msg[(int)level];
}

Logger::Logger(const char* const& filename, bool overwrite) {
    if (!filename) {
        stream = stdout;
        return;
    }
    stream = fopen(filename, overwrite ? "w" : "a");
    if (!stream) {
        printf("%s: %s %s\n", getLevel(Level::WARNING), getMessage(RC::FILE_NOT_FOUND), filename);
        stream = stdout;
    }
}

ILogger* ILogger::createLogger() {
    char* filename = nullptr;
    return new Logger(filename);
}

ILogger* ILogger::createLogger(const char* const& filename, bool overwrite) {
    return new Logger(filename, overwrite);
}

RC Logger::log(RC code, Level level, const char* const& srcfile, const char* const& function, int line) {
    fprintf(stream, "#####\n");
    log(code, level);
    fprintf(stream, "File: %s\nLine: %i\nFunction: %s\n", srcfile, line, function);
    fprintf(stream, "#####\n");
    return RC::SUCCESS;
}

RC Logger::log(RC code, Level level) {
    fprintf(stream, "%s: %s\n", getLevel(level), getMessage(code));
    return RC::SUCCESS;
}

Logger::~Logger() {
    fclose(stream);
}

ILogger::~ILogger() {};
