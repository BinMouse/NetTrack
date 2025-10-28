#include <windows.h>
#include <string>
#include "Settings.h"

#define ID_EDIT_PATH      101
#define ID_EDIT_SERVER    102
#define ID_EDIT_TOKEN     103
#define ID_COMBO_INTERVAL 104
#define ID_BUTTON_SAVE    105

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
    {
        // Путь для логов
        CreateWindowW(L"STATIC", L"Путь для логов:",
            WS_VISIBLE | WS_CHILD, 20, 20, 150, 20,
            hwnd, nullptr, nullptr, nullptr);

        CreateWindowW(L"EDIT", g_settings.logPath.c_str(),
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
            180, 20, 250, 25,
            hwnd, (HMENU)ID_EDIT_PATH, nullptr, nullptr);

        // Адрес сервера
        CreateWindowW(L"STATIC", L"Адрес сервера:",
            WS_VISIBLE | WS_CHILD, 20, 60, 150, 20,
            hwnd, nullptr, nullptr, nullptr);

        CreateWindowW(L"EDIT", g_settings.server.c_str(),
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
            180, 60, 250, 25,
            hwnd, (HMENU)ID_EDIT_SERVER, nullptr, nullptr);

        // Токен
        CreateWindowW(L"STATIC", L"Токен:",
            WS_VISIBLE | WS_CHILD, 20, 100, 150, 20,
            hwnd, nullptr, nullptr, nullptr);

        CreateWindowW(L"EDIT", g_settings.token.c_str(),
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
            180, 100, 250, 25,
            hwnd, (HMENU)ID_EDIT_TOKEN, nullptr, nullptr);

        // Интервал сохранения
        HWND combo = CreateWindowW(L"COMBOBOX", L"",
            CBS_DROPDOWNLIST | WS_VISIBLE | WS_CHILD | WS_VSCROLL,
            180, 140, 150, 120,
            hwnd, (HMENU)ID_COMBO_INTERVAL, nullptr, nullptr);

        SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)L"1 минута");
        SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)L"5 минут");
        SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)L"10 минут");

        // По умолчанию выбрать значение из g_settings
        int selIndex = (g_settings.saveIntervalMinutes == 1 ? 0 :
            g_settings.saveIntervalMinutes == 5 ? 1 : 2);
        SendMessageW(combo, CB_SETCURSEL, selIndex, 0);

        // Кнопка "Сохранить настройки"
        CreateWindowW(L"BUTTON", L"Сохранить настройки",
            WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            150, 200, 180, 35,
            hwnd, (HMENU)ID_BUTTON_SAVE, nullptr, nullptr);
        break;
    }
    case WM_COMMAND:
    {
        if (LOWORD(wParam) == ID_BUTTON_SAVE) {
            wchar_t path[260], server[260], token[260];

            GetWindowTextW(GetDlgItem(hwnd, ID_EDIT_PATH), path, 260);
            GetWindowTextW(GetDlgItem(hwnd, ID_EDIT_SERVER), server, 260);
            GetWindowTextW(GetDlgItem(hwnd, ID_EDIT_TOKEN), token, 260);

            g_settings.logPath = path;
            g_settings.server = server;
            g_settings.token = token;

            int sel = SendMessageW(GetDlgItem(hwnd, ID_COMBO_INTERVAL), CB_GETCURSEL, 0, 0);
            g_settings.saveIntervalMinutes = (sel == 0 ? 1 : sel == 1 ? 5 : 10);

            std::wstring msg = L"Путь: " + g_settings.logPath +
                L"\nСервер: " + g_settings.server +
                L"\nТокен: " + g_settings.token +
                L"\nИнтервал: " + (sel == 0 ? L"1 минута" :
                    sel == 1 ? L"5 минут" : L"10 минут");

            MessageBoxW(hwnd, msg.c_str(), L"Настройки сохранены", MB_OK | MB_ICONINFORMATION);
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

DWORD WINAPI ShowSettingsWindow(LPVOID) {
    const wchar_t CLASS_NAME[] = L"SettingsWindowClass";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = CLASS_NAME;

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0, CLASS_NAME, L"Настройки",
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT, 480, 300,
        nullptr, nullptr, GetModuleHandle(nullptr), nullptr
    );

    ShowWindow(hwnd, SW_SHOW);

    MSG msg = {};
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}
