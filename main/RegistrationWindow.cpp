// RegistrationWindow.cpp
#include "WalletApp.h"
#include <commctrl.h>

LRESULT CALLBACK RegistrationWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        CreateWindow(L"STATIC", L"Логин:", WS_VISIBLE | WS_CHILD, 20, 30, 80, 20, hwnd, nullptr, nullptr, nullptr);
        hRegLoginEdit = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 100, 30, 150, 20, hwnd, nullptr, nullptr, nullptr);

        CreateWindow(L"STATIC", L"Пароль:", WS_VISIBLE | WS_CHILD, 20, 60, 80, 20, hwnd, nullptr, nullptr, nullptr);
        hRegPassEdit = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD, 100, 60, 150, 20, hwnd, nullptr, nullptr, nullptr);

        CreateWindow(L"STATIC", L"Подтвердить:", WS_VISIBLE | WS_CHILD, 20, 90, 80, 20, hwnd, nullptr, nullptr, nullptr);
        hRegConfirmEdit = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD, 100, 90, 150, 20, hwnd, nullptr, nullptr, nullptr);

        CreateWindow(L"STATIC", L"Роль:", WS_VISIBLE | WS_CHILD, 20, 120, 80, 20, hwnd, nullptr, nullptr, nullptr);
        hRegRoleCombo = CreateWindow(WC_COMBOBOX, L"", CBS_DROPDOWNLIST | WS_VISIBLE | WS_CHILD,
            100, 120, 150, 100, hwnd, nullptr, nullptr, nullptr);
        SendMessage(hRegRoleCombo, CB_ADDSTRING, 0, (LPARAM)L"user");
        SendMessage(hRegRoleCombo, CB_ADDSTRING, 0, (LPARAM)L"admin");

        if (currentUserRole != UserRole::Admin) {
            EnableWindow(hRegRoleCombo, FALSE);
            SendMessage(hRegRoleCombo, CB_SETCURSEL, 0, 0);
        }

        CreateWindow(L"BUTTON", L"Зарегистрировать", WS_VISIBLE | WS_CHILD, 100, 160, 140, 30, hwnd, (HMENU)1, nullptr, nullptr);
        hRegStatusLabel = CreateWindow(L"STATIC", L"", WS_VISIBLE | WS_CHILD, 100, 200, 150, 20, hwnd, nullptr, nullptr, nullptr);
    } break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1) {
            wchar_t login[128], pass[128], confirm[128];
            GetWindowText(hRegLoginEdit, login, 128);
            GetWindowText(hRegPassEdit, pass, 128);
            GetWindowText(hRegConfirmEdit, confirm, 128);

            if (wcslen(login) == 0 || wcslen(pass) == 0) {
                SetWindowText(hRegStatusLabel, L"Заполните все поля!");
                return 0;
            }
            if (wcscmp(pass, confirm) != 0) {
                SetWindowText(hRegStatusLabel, L"Пароли не совпадают!");
                return 0;
            }

            sqlite3_stmt* stmt;
            std::string login_utf8 = wstr_to_utf8(std::wstring(login));
            const char* check_sql = "SELECT COUNT(*) FROM users WHERE login = ?;";
            if (sqlite3_prepare_v2(g_db, check_sql, -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_text(stmt, 1, login_utf8.c_str(), -1, SQLITE_STATIC);
                if (sqlite3_step(stmt) == SQLITE_ROW && sqlite3_column_int(stmt, 0) > 0) {
                    sqlite3_finalize(stmt);
                    SetWindowText(hRegStatusLabel, L"Логин занят!");
                    return 0;
                }
                sqlite3_finalize(stmt);
            }

            std::string pass_utf8 = wstr_to_utf8(std::wstring(pass));
            int selected = (int)SendMessage(hRegRoleCombo, CB_GETCURSEL, 0, 0);
            std::string role = (selected == 0) ? "user" : "admin";

            std::string sql = "INSERT INTO users (login, password, role) VALUES ('" + login_utf8 + "', '" + pass_utf8 + "', '" + role + "');";
            char* errMsg = nullptr;
            if (sqlite3_exec(g_db, sql.c_str(), nullptr, nullptr, &errMsg) == SQLITE_OK) {
                MessageBox(hwnd, L"Успешно зарегистрирован!", L"Готово", MB_OK | MB_ICONINFORMATION);
                DestroyWindow(hwnd);
                ShowLoginWindow(GetModuleHandle(nullptr));
            }
            else {
                SetWindowText(hRegStatusLabel, L"Ошибка БД");
                sqlite3_free(errMsg);
            }
        }
        break;

    case WM_DESTROY:
        hRegisterWindow = nullptr;
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void ShowRegistrationWindow(HINSTANCE hInstance) {
    const wchar_t CLASS_NAME[] = L"RegistrationWindowClass";
    WNDCLASS wc = {}; wc.lpfnWndProc = RegistrationWindowProc; wc.hInstance = hInstance; wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"Регистрация", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 350, 300, nullptr, nullptr, hInstance, nullptr);
    if (hwnd) {
        ShowWindow(hwnd, SW_SHOWDEFAULT);
        hRegisterWindow = hwnd;
    }
}