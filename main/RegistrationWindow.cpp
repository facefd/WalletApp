// RegistrationWindow.cpp
#include "WalletApp.h"
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")
std::string code = generateCode(); // должна находиться

LRESULT CALLBACK RegistrationWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        // Логин
        CreateWindow(L"STATIC", L"Логин:", WS_VISIBLE | WS_CHILD, 20, 30, 80, 20, hwnd, nullptr, nullptr, nullptr);
        hRegLoginEdit = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 100, 30, 150, 20, hwnd, nullptr, nullptr, nullptr);

        // Пароль
        CreateWindow(L"STATIC", L"Пароль:", WS_VISIBLE | WS_CHILD, 20, 60, 80, 20, hwnd, nullptr, nullptr, nullptr);
        hRegPassEdit = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD, 100, 60, 150, 20, hwnd, nullptr, nullptr, nullptr);

        // Подтверждение пароля
        CreateWindow(L"STATIC", L"Подтвердить:", WS_VISIBLE | WS_CHILD, 20, 90, 80, 20, hwnd, nullptr, nullptr, nullptr);
        hRegConfirmEdit = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD, 100, 90, 150, 20, hwnd, nullptr, nullptr, nullptr);

        // ?? Поле Email — новое!
        CreateWindow(L"STATIC", L"Email:", WS_VISIBLE | WS_CHILD, 20, 120, 80, 20, hwnd, nullptr, nullptr, nullptr);
        hRegEmailEdit = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 100, 120, 150, 20, hwnd, nullptr, nullptr, nullptr);

        // Роль (если есть)
        CreateWindow(L"STATIC", L"Роль:", WS_VISIBLE | WS_CHILD, 20, 150, 80, 20, hwnd, nullptr, nullptr, nullptr);
        hRegRoleCombo = CreateWindow(WC_COMBOBOX, L"", CBS_DROPDOWNLIST | WS_VISIBLE | WS_CHILD,
            100, 150, 150, 100, hwnd, nullptr, nullptr, nullptr);
        SendMessage(hRegRoleCombo, CB_ADDSTRING, 0, (LPARAM)L"user");
        SendMessage(hRegRoleCombo, CB_ADDSTRING, 0, (LPARAM)L"admin");

        // Кнопка
        CreateWindow(L"BUTTON", L"Зарегистрировать", WS_VISIBLE | WS_CHILD, 100, 190, 140, 30, hwnd, (HMENU)1, nullptr, nullptr);
        hRegStatusLabel = CreateWindow(L"STATIC", L"", WS_VISIBLE | WS_CHILD, 100, 230, 150, 20, hwnd, nullptr, nullptr, nullptr);
    } break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1) {
            wchar_t login[128], pass[128], confirm[128], email[128];
            GetWindowText(hRegLoginEdit, login, 128);
            GetWindowText(hRegPassEdit, pass, 128);
            GetWindowText(hRegConfirmEdit, confirm, 128);
            GetWindowText(hRegEmailEdit, email, 128);

            if (wcslen(login) == 0 || wcslen(pass) == 0 || wcslen(email) == 0) {
                SetWindowText(hRegStatusLabel, L"Заполните все поля!");
                return 0;
            }
            if (wcscmp(pass, confirm) != 0) {
                SetWindowText(hRegStatusLabel, L"Пароли не совпадают!");
                return 0;
            }

            std::string code = generateCode();
            std::wstring wemail(email);
            bool sent = sendVerificationEmail(wemail, code);

            if (!sent) {
                SetWindowText(hRegStatusLabel, L"Ошибка отправки email!");
                return 0;
            }

            DestroyWindow(hwnd);
            ShowVerificationWindow(GetModuleHandle(nullptr), login, pass, email, code);
        }
        break;

    case WM_DESTROY:
        hRegisterWindow = nullptr;
        PostQuitMessage(0);  // Можно оставить, если это последнее окно
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void ShowRegistrationWindow(HINSTANCE hInstance) {
    const wchar_t CLASS_NAME[] = L"RegistrationWindowClass";
    WNDCLASS wc = {};
    wc.lpfnWndProc = RegistrationWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Регистрация",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 350, 300,
        nullptr, nullptr, hInstance, nullptr
    );

    if (hwnd) {
        ShowWindow(hwnd, SW_SHOWDEFAULT);
        hRegisterWindow = hwnd;
    }
}