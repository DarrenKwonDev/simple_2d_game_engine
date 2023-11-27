#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

#include <spdlog/spdlog.h>

#include "Logger.h"

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
    // do not reinvent the wheel, especially logger.
    std::cout << "\x1B[32m"
              << "LOG : ["
              << CurrentDateTimeToString()
              << "] " << message << "\033[0m" << std::endl;

    // spdlog::info(message);
}

void Logger::Err(const std::string& message) {
    std::cout << "\x1B[91m"
              << "ERR : ["
              << CurrentDateTimeToString()
              << "] " << message << "\033[0m" << std::endl;

    // spdlog::error(message);
}
