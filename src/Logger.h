
#pragma once

#include <string>
#include <vector>

enum LogType {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
};

struct LogEntry {
    LogType type;
    std::string messages;
};

class Logger {
private:
public:
    Logger();
    virtual ~Logger();

    static std::vector<LogEntry> mMessages;
    static void Log(const std::string& message);
    static void Err(const std::string& message);
};