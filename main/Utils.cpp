// Utils.cpp
#include "WalletApp.h"
#include <cstdlib>
#include <codecvt>

std::wstring getCurrentTime() {
    std::time_t now = std::time(nullptr);
    tm t;
    localtime_s(&t, &now);
    wchar_t buffer[20];
    wcsftime(buffer, sizeof(buffer) / sizeof(wchar_t), L"%Y-%m-%d %H:%M:%S", &t);
    return std::wstring(buffer);
}

std::string wstr_to_utf8(const std::wstring& wstr) {
    if (wstr.empty()) return "";
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string str(size - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size, nullptr, nullptr);
    return str;
}

std::wstring utf8_to_wstr(const std::string& str) {
    if (str.empty()) return L"";
    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(size - 1, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size);
    return wstr;
}

std::string generateCode() {
    static bool seeded = false;
    if (!seeded) {
        srand(static_cast<unsigned>(time(nullptr)));
        seeded = true;
    }
    return std::to_string(100000 + rand() % 900000);
}

bool sendVerificationEmail(const std::wstring& toEmailW, const std::string& code) {
    std::string to = wstr_to_utf8(toEmailW);
    std::string from = "face_shot@mail.ru";           // ← Замените
    std::string password = "pygc gzne wjdf sltt";     // ← App Password

    std::string cmd =
        "powershell -command \""
        "$pwd = ConvertTo-SecureString '" + password + "' -AsPlainText -Force; "
        "$cred = New-Object System.Management.Automation.PSCredential('" + from + "', $pwd); "
        "Send-MailMessage "
        "-SmtpServer 'smtp.gmail.com' "
        "-Port 587 "
        "-UseSsl "
        "-From '" + from + "' "
        "-To '" + to + "' "
        "-Subject 'Код подтверждения' "
        "-Body 'Ваш код: " + code + "' "
        "-Credential $cred\"";

    int result = system(cmd.c_str());
    return result == 0;
}