#include "logger.hpp"

#include <cstdlib>
#include <cstdarg>
#include <cstdio>
#include <cassert>
#include <ctime>
#include <cstdint>

#define DEBUG

// static ---------------------------------------------------------------------

static FILE* log_file = nullptr;
static int64_t indent_level = 0;

static void LoggingDtor(void);

// LogHelper_ -----------------------------------------------------------------

LogHelper_::LogHelper_() {
    indent_level++;
}

LogHelper_::~LogHelper_() {
    indent_level--;
}

// global ---------------------------------------------------------------------

LoggingStatus LoggingSetup(const char* const log_file_name) {
    assert(log_file_name != nullptr); // for debug
    if (log_file_name == nullptr) { return LoggingStatus::kNullPassed; }

    FILE* try_open_log_file = fopen(log_file_name, "w");
    if (try_open_log_file == nullptr) { return LoggingStatus::kCantOpenFile; }

    int set_buf_status = setvbuf(try_open_log_file, nullptr, _IONBF, 0);
    if (set_buf_status) {
        fclose(try_open_log_file);
        return LoggingStatus::kInternalError; 
    }

    int atexit_status = atexit(LoggingDtor);
    if (atexit_status) {
        fclose(try_open_log_file);
        return LoggingStatus::kInternalError;
    }

    log_file = try_open_log_file;

    return LoggingStatus::kOk;
}

LoggingStatus Log_(const char* const source_file_name, 
                        const int source_line_num, 
                        const char* const source_func_name, 
                        const char* const format_str, 
                        ...) 
{
    assert(format_str       != nullptr);
    assert(source_file_name != nullptr);
    assert(source_line_num  >= 0);
    assert(source_func_name != nullptr);

    if ((format_str == nullptr)
        || (source_file_name == nullptr)
        || (source_func_name == nullptr)
        || (source_line_num < 0)) 
    { 
        return LoggingStatus::kNullPassed; 
    }
    
    if (log_file == nullptr) { return LoggingStatus::kUninitLog; }

#if defined (DEBUG)
    time_t current_time = time(nullptr);
    if (current_time == (time_t)-1) {
        return LoggingStatus::kInternalError;
    }
    
    struct tm* current_tm = localtime(&current_time);

    va_list args;
    va_start(args, format_str);

    int printed = 0;
    printed = fprintf(log_file, 
                      "[%d:%d:%d][%s:%s:%d]: ", 
                      current_tm->tm_hour, 
                      current_tm->tm_min, 
                      current_tm->tm_sec, 
                      source_file_name, 
                      source_func_name, 
                      source_line_num);

    const int kAlignment = 90;
    for (int i = printed; i < kAlignment; i++) {
        fputc(' ', log_file);
    }

    fputc('|', log_file);
    fputc(' ', log_file);

    const int kTabWidth = 2;
    for (int i = 0; i < indent_level * kTabWidth; i++) {
        fputc(' ', log_file);
    }

    vfprintf(log_file, format_str, args);
    // fputc('\n', log_file);

    va_end(args);
#else 
    // do nothing    
#endif // DEBUG
    return LoggingStatus::kOk;
}

const char* LogErrorToStr(LoggingStatus log_status) {
    switch (log_status) {
        case LoggingStatus::kOk:            return STRINGIFY(LoggingStatus::kOk);

        case LoggingStatus::kUninitEnum:    return STRINGIFY(LoggingStatus::kUninitEnum);
        
        case LoggingStatus::kNullPassed:    return STRINGIFY(LoggingStatus::kNullPassed);
        case LoggingStatus::kCantOpenFile:  return STRINGIFY(LoggingStatus::kCantOpenFile);
        case LoggingStatus::kUninitLog:     return STRINGIFY(LoggingStatus::kUninitLog);
        case LoggingStatus::kInternalError: return STRINGIFY(LoggingStatus::kInternalError); 
        default: assert(0 && "unknown enum value"); return "<unknown enum value>";
    }
}

// static ---------------------------------------------------------------------

static void LoggingDtor(void) {
    if (log_file == nullptr) { return ; }

    LogFunctionEntry();

    fclose(log_file);
    log_file = nullptr;
}

