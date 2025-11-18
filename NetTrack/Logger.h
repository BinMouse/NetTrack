#pragma once
#include <chrono>
#include <ctime>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <string>
#include <atomic>
#include <iostream>
#include <iomanip>

class Logger
{
private:
    std::ofstream logFile;
    std::queue<std::string> messageQueue;
    std::mutex mtx;
    std::condition_variable cv;
    std::thread worker;
    std::atomic<bool> running;

    Logger();
    ~Logger();

    //Запрет копирования
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void process();
public:
    static Logger& getInstance();
    void log(const std::string& message);
};

