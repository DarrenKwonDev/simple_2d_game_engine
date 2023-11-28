#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

#include <spdlog/spdlog.h>

#include "Logger.h"

// Logger 클래스의 static 멤버 변수 초기화
// static 멤버 변수를 초기화하지 않으면, 링커는 해당 변수에 대한 정의를 찾을 수 없어 "Undefined symbols" 오류를 발생시킵니다.
std::vector<LogEntry> Logger::mMessages;

Logger::Logger() {
}

Logger::~Logger() {
}

std::string CurrentDateTimeToString() {
    std::time_t now_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string output(30, '\0');
    std::tm* tm = std::localtime(&now_c);
    std::strftime(&output[0], output.size(), "%d-%b-%Y %H:%M:%S", tm);
    return output;
}

void Logger::Log(const std::string& message) {
    LogEntry logEntry;
    logEntry.type = LOG_INFO;
    logEntry.messages = "LOG : [" + CurrentDateTimeToString() + "] " + message;

    std::cout << "\x1B[32m" << logEntry.messages << "\033[0m" << std::endl;

    Logger::mMessages.push_back(logEntry);

    // do not reinvent the wheel, especially logger.
    // spdlog::info(message);
}

void Logger::Err(const std::string& message) {
    LogEntry logEntry;
    logEntry.type = LOG_ERROR;
    logEntry.messages = "ERR : [" + CurrentDateTimeToString() + "] " + message;
    std::cout << "\x1B[91m"
              << logEntry.messages
              << "\033[0m" << std::endl;

    Logger::mMessages.push_back(logEntry);

    // do not reinvent the wheel, especially logger.
    // spdlog::error(message);
}
