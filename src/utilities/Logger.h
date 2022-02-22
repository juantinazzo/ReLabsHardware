#include <Arduino.h>

#define LOG(Message, Level, System, ...) (Logger::Log(Message, Level, System, ##__VA_ARGS__))
#define LOG_NOTAG(Message, Level, System, ...)              \
    {                                                       \
        Logger::SetTag(false);                              \
        Logger::Log(Message, Level, System, ##__VA_ARGS__); \
        Logger::SetTag(true);                               \
    }

#ifndef LOGGER_H
#define LOGGER_H
extern char *eLogLevelName[6];
enum eLogLevel
{
    NONE,
    Info,
    Debug,
    Warning,
    Error,
    Critical
};

class Logger
{
private:
    eLogLevel logLevel = Info;
    bool tag = true;

public:
    static void
    SetPriority(eLogLevel new_logLevel)
    {
        get_instance().logLevel = new_logLevel;
    }
    static void SetTag(bool new_tag)
    {
        get_instance().tag = new_tag;
    }

    template <typename... Args>
    static void Log(const char *message, eLogLevel level, const char *sys, Args... args)
    {
        if ((level >= get_instance().logLevel) & (level > 0))
        {
            char buff[256];
            sprintf(buff, message, args...);
            get_instance().log(level, sys, buff);
        }
    }

    template <typename... Args>
    static void Log(String message, eLogLevel level, const char *sys, Args... args)
    {
        if ((level >= get_instance().logLevel) & (level > 0))
        {
            char buff[256];
            sprintf(buff, message.c_str(), args...);
            get_instance().log(level, sys, buff);
        }
    }

private:
    Logger()
    {
        Serial.begin(115200);
    }

    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    ~Logger()
    {
    }

    static Logger &get_instance()
    {
        static Logger logger;
        return logger;
    }

    template <typename... Args>
    void log(eLogLevel level, const char *sys, const char *message)
    {
        if (tag)
        {
            Serial.print(eLogLevelName[level]);
            Serial.print("[");
            Serial.print(sys);
            Serial.print("]\t");
        }
        Serial.print(message);
        if (tag)
            Serial.print("\n");
    }
};

#endif