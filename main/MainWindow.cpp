// MainWindow.cpp
#define _CRT_SECURE_NO_WARNINGS
#include "WalletApp.h"
#include <ctime>

void UpdateUI(HWND hwnd) {
    wchar_t buffer[128]; swprintf_s(buffer, L"Баланс: %.2f", balance);
    SetWindowText(hBalanceLabel, buffer);

    SendMessage(hHistoryList, LB_RESETCONTENT, 0, 0);
    for (auto& inc : incomes) {
        wchar_t fullItem[1024]; tm t = {}; localtime_s(&t, &inc.timestamp);
        wchar_t timeStr[20]; wcsftime(timeStr, 20, L"%Y-%m-%d %H:%M:%S", &t);
        swprintf_s(fullItem, L"Доход +%0.2f | %s | %s", inc.amount, inc.description.c_str(), timeStr);
        SendMessage(hHistoryList, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(fullItem));
    }
    for (auto& exp : expenses) {
        wchar_t fullItem[1024]; tm t = {}; localtime_s(&t, &exp.timestamp);
        wchar_t timeStr[20]; wcsftime(timeStr, 20, L"%Y-%m-%d %H:%M:%S", &t);
        swprintf_s(fullItem, L"Расход %0.2f | %s | %s", exp.amount, exp.description.c_str(), timeStr);
        SendMessage(hHistoryList, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(fullItem));
    }

    if (currentUserRole == UserRole::User) {
        ShowWindow(hIncomeEdit, SW_HIDE);
        ShowWindow(hIncomeDesc, SW_HIDE);
        ShowWindow(hExpenseEdit, SW_HIDE);
        ShowWindow(hExpenseDesc, SW_HIDE);
        ShowWindow(hAddIncomeButton, SW_HIDE);
        ShowWindow(hAddExpenseButton, SW_HIDE);
    }
}

void AddIncome(HWND hwnd) {
    wchar_t buffer[256], desc[256];
    GetWindowText(hIncomeEdit, buffer, 256);
    GetWindowText(hIncomeDesc, desc, 256);
    double amount = _wtof(buffer);
    if (amount <= 0 || wcslen(desc) == 0) {
        MessageBox(hwnd, L"Введите данные!", L"Ошибка", MB_OK);
        return;
    }
    balance += amount;
    incomes.push_back({ amount, desc, time(nullptr) });
    SaveIncome(incomes.back());
    SaveBalance();
    UpdateUI(hwnd);
}

void AddExpense(HWND hwnd) {
    wchar_t buffer[256], desc[256];
    GetWindowText(hExpenseEdit, buffer, 256);
    GetWindowText(hExpenseDesc, desc, 256);
    double amount = _wtof(buffer);
    if (amount <= 0 || wcslen(desc) == 0) {
        MessageBox(hwnd, L"Введите данные!", L"Ошибка", MB_OK);
        return;
    }
    if (amount > balance) {
        MessageBox(hwnd, L"Недостаточно средств!", L"Ошибка", MB_OK);
        return;
    }
    balance -= amount;
    expenses.push_back({ amount, desc, time(nullptr) });
    SaveExpense(expenses.back());
    SaveBalance();
    UpdateUI(hwnd);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        hBalanceLabel = CreateWindow(L"STATIC", L"Баланс: 0.00", WS_VISIBLE | WS_CHILD, 20, 20, 500, 20, hwnd, nullptr, nullptr, nullptr);
        // ... остальные элементы ...
        UpdateUI(hwnd);
    } break;
        // ... остальное
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void ShowMainWindow(HINSTANCE hInstance) {
    LoadData();
    const wchar_t CLASS_NAME[] = L"WalletWindowClass";
    WNDCLASS wc = {}; wc.lpfnWndProc = WindowProc; wc.hInstance = hInstance; wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);
    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"Кошелёк", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, nullptr, nullptr, hInstance, nullptr);
    if (hwnd) ShowWindow(hwnd, SW_SHOWDEFAULT);
}