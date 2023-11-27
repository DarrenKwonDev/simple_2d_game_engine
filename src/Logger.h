#pragma once

#include <string>

class Logger {
private:
public:
    Logger();
    virtual ~Logger();

    static void Log(const std::string& message);
    static void Err(const std::string& message);
};