#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#include <cstdbool>

#if !defined (STRINGIFY)
#define STRINGIFY(name_) #name_
#endif // STRINGIFY

#if !defined (TO_STR)
#define TO_STR(name_) #name_
#endif // TO_STR

class LogHelper_ { // dont use explisitly 
  public:
    LogHelper_();
    ~LogHelper_();
};

enum class LoggingStatus {
    kOk            = -1,
    
    kUninitEnum    =  0,

    kNullPassed    =  1,
    kCantOpenFile  =  2,
    kUninitLog     =  3,
    kInternalError =  4,
};

LoggingStatus LoggingSetup(const char* const log_file_name);

#define Log(...) Log_(__FILE__, __LINE__, __func__, __VA_ARGS__)
#define LogFunctionEntry() LogHelper_ log_helper_{}; Log("%s function entry\n", __func__)
#define LogError(error_) Log("Error encountered: %s\n", STRINGIFY(error_))
#define LogVariable(format_, value_) Log(#value_ ": " format_ "\n", value_)

LoggingStatus Log_(const char* const source_file_name,
                        const int source_line_num, 
                        const char* const source_func_name, 
                        const char* const format_str, 
                        ...) __attribute__(( format(printf, 4, 5)) );

const char* LogErrorToStr(const LoggingStatus log_status);

#endif // LOGGER_HPP_

