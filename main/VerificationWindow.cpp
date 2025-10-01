// VerificationWindow.cpp
#define _CRT_SECURE_NO_WARNINGS
#include "WalletApp.h"
#include <string>

HWND hVerifyCodeEdit = nullptr;

LRESULT CALLBACK VerificationWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static std::wstring v_login, v_pass, v_email;
    static std::string v_code;

    switch (uMsg) {
    case WM_CREATE: {
        CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
        auto data = reinterpret_cast<std::tuple<std::wstring, std::wstring, std::wstring, std::string>*>(cs->lpCreateParams);
        v_login = std::get<0>(*data);
        v_pass = std::get<1>(*data);
        v_email = std::get<2>(*data);
        v_code = std::get<3>(*data);

        CreateWindow(L"STATIC", L"Введите код из email:", WS_VISIBLE | WS_CHILD, 20, 30, 200, 20, hwnd, nullptr, nullptr, nullptr);
        hVerifyCodeEdit = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 20, 60, 150, 20, hwnd, nullptr, nullptr, nullptr);
        CreateWindow(L"BUTTON", L"Подтвердить", WS_VISIBLE | WS_CHILD, 20, 100, 100, 30, hwnd, (HMENU)1, nullptr, nullptr);
    } break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1) {
            wchar_t input[16];
            GetWindowText(hVerifyCodeEdit, input, 16);
            std::string userInput = wstr_to_utf8(std::wstring(input));

            if (userInput == v_code) {
                std::string sql = "INSERT INTO users (login, password, role, email) VALUES ('" +
                    wstr_to_utf8(v_login) + "', '" +
                    wstr_to_utf8(v_pass) + "', 'user', '" +
                    wstr_to_utf8(v_email) + "')";
                sqlite3_exec(g_db, sql.c_str(), nullptr, nullptr, nullptr);

                MessageBox(hwnd, L"Регистрация завершена!", L"Успех", MB_OK | MB_ICONINFORMATION);
                DestroyWindow(hwnd);
                ShowLoginWindow(GetModuleHandle(nullptr));
            }
            else {
                MessageBox(hwnd, L"Неверный код!", L"Ошибка", MB_OK | MB_ICONWARNING);
            }
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void ShowVerificationWindow(
    HINSTANCE hInstance,
    const wchar_t* login,
    const wchar_t* password,
    const wchar_t* email,
    const std::string& code
) {
    auto* data = new std::tuple<std::wstring, std::wstring, std::wstring, std::string>(
        std::wstring(login),
        std::wstring(password),
        std::wstring(email),
        code
    );

    const wchar_t CLASS_NAME[] = L"VerificationWindowClass";
    WNDCLASS wc = {};
    wc.lpfnWndProc = VerificationWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"Подтверждение", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 200, nullptr, nullptr, hInstance, data);
    if (hwnd) ShowWindow(hwnd, SW_SHOWDEFAULT);
}