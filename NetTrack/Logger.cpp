#include "Logger.h"

Logger::Logger() : running(true)
{
    logFile.open("log.txt", std::ios::trunc);
    if (!logFile.is_open()) {
        std::cerr << "Не удалось открыть log.txt для записи!" << std::endl;
    }
        
    worker = std::thread([this]() { this->process(); });
}

Logger::~Logger() {
    running = false;
    cv.notify_all();
    if (worker.joinable())
        worker.join();

    if (logFile.is_open())
        logFile.close();
}

void Logger::process() {
    while (running || !messageQueue.empty()) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]() { return !messageQueue.empty() || !running; });

        while (!messageQueue.empty()) {
            std::string msg = messageQueue.front();
            messageQueue.pop();
            lock.unlock();

            if (logFile.is_open()) {
                auto now = std::chrono::system_clock::now();
                std::time_t t = std::chrono::system_clock::to_time_t(now);
                std::tm tm;
                localtime_s(&tm, &t);

                logFile << "[" << std::put_time(&tm, "%d/%m/%y %H:%M:%S") << "] "
                    << msg << std::endl;
            }

            lock.lock();
        }
    }
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::log(const std::string& message) {
    {
        std::lock_guard<std::mutex> lock(mtx);
        messageQueue.push(message);
    }
    cv.notify_one();
}