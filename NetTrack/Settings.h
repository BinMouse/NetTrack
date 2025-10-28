#pragma once
#include <string>

struct Settings {
    std::wstring logPath = L"C:\\logs";
    std::wstring server = L"127.0.0.1";
    std::wstring token;
    int saveIntervalMinutes = 5; // 1, 5 10
};

extern Settings g_settings;
