// LoginWindow.cpp
#include "WalletApp.h"

LRESULT CALLBACK LoginWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        hLoginEdit = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 100, 30, 150, 20, hwnd, nullptr, nullptr, nullptr);
        hPasswordEdit = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD, 100, 60, 150, 20, hwnd, nullptr, nullptr, nullptr);
        CreateWindow(L"STATIC", L"Логин:", WS_VISIBLE | WS_CHILD, 20, 30, 80, 20, hwnd, nullptr, nullptr, nullptr);
        CreateWindow(L"STATIC", L"Пароль:", WS_VISIBLE | WS_CHILD, 20, 60, 80, 20, hwnd, nullptr, nullptr, nullptr);
        hLoginButton = CreateWindow(L"BUTTON", L"Войти", WS_VISIBLE | WS_CHILD, 100, 100, 100, 30, hwnd, (HMENU)1, nullptr, nullptr);
        CreateWindow(L"BUTTON", L"Регистрация", WS_VISIBLE | WS_CHILD, 100, 140, 100, 30, hwnd, (HMENU)2, nullptr, nullptr);
        hLoginStatus = CreateWindow(L"STATIC", L"", WS_VISIBLE | WS_CHILD, 100, 180, 150, 20, hwnd, nullptr, nullptr, nullptr);
    } break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1) {
            wchar_t login[128], password[128];
            GetWindowText(hLoginEdit, login, 128);
            GetWindowText(hPasswordEdit, password, 128);
            UserRole role = UserRole::None;
            if (CheckUserLogin(login, password, &role)) {
                currentUserName = login;
                currentUserRole = role;
                DestroyWindow(hwnd);
                ShowMainWindow(GetModuleHandle(nullptr));
            }
            else {
                SetWindowText(hLoginStatus, L"Неверный логин или пароль!");
            }
        }
        else if (LOWORD(wParam) == 2) {
            ShowWindow(hwnd, SW_HIDE);
            ShowRegistrationWindow(GetModuleHandle(nullptr));
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

void ShowLoginWindow(HINSTANCE hInstance) {
    const wchar_t CLASS_NAME[] = L"LoginWindowClass";
    WNDCLASS wc = {}; wc.lpfnWndProc = LoginWindowProc; wc.hInstance = hInstance; wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"Вход в аккаунт", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 350, 250, nullptr, nullptr, hInstance, nullptr);
    if (hwnd) ShowWindow(hwnd, SW_SHOWDEFAULT);
}