// Utils.cpp
#include "WalletApp.h"

std::wstring getCurrentTime() {
    std::time_t now = std::time(0);
    tm t;
    localtime_s(&t, &now);
    wchar_t buffer[20];
    wcsftime(buffer, sizeof(buffer) / sizeof(buffer[0]), L"%Y-%m-%d %H:%M:%S", &t);
    return std::wstring(buffer);
}