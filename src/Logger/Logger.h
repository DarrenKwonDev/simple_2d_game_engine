
#pragma once

#include <string>
#include <vector>

enum LogType { LOG_INFO, LOG_WARNING, LOG_ERROR };

struct LogEntry {
    LogType type;
    std::string messages;
};

/*
    예시적 구현임.
    콘솔에 로그를 출력하는 것은 게임 엔진의 퍼포먼스에 영향을 미친다.
    io는 무료가 아니다.
*/
class Logger {
private:
public:
    Logger();
    virtual ~Logger();

    static std::vector<LogEntry> mMessages;
    static void Log(const std::string& message);
    static void Err(const std::string& message);
};