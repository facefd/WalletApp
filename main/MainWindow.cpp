// MainWindow.cpp
#include "WalletApp.h"

void UpdateUI(HWND hwnd) {
    wchar_t buffer[128];
    swprintf_s(buffer, L"Баланс: %.2f", balance);
    SetWindowText(hBalanceLabel, buffer);

    SendMessage(hHistoryList, LB_RESETCONTENT, 0, 0);

    for (const auto& inc : incomes) {
        wchar_t fullItem[1024];
        tm t = {};
        localtime_s(&t, &inc.timestamp);
        wchar_t timeStr[20];
        wcsftime(timeStr, 20, L"%Y-%m-%d %H:%M:%S", &t);
        swprintf_s(fullItem, L"Доход +%0.2f | %s | %s", inc.amount, inc.description.c_str(), timeStr);
        SendMessage(hHistoryList, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(fullItem));
    }

    for (const auto& exp : expenses) {
        wchar_t fullItem[1024];
        tm t = {};
        localtime_s(&t, &exp.timestamp);
        wchar_t timeStr[20];
        wcsftime(timeStr, 20, L"%Y-%m-%d %H:%M:%S", &t);
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
    GetWindowText(hIncomeEdit, buffer, _countof(buffer));
    GetWindowText(hIncomeDesc, desc, _countof(desc));

    double amount = _wtof(buffer);
    if (amount <= 0 || wcslen(desc) == 0) {
        MessageBox(hwnd, L"Введите корректные данные!", L"Ошибка", MB_OK | MB_ICONWARNING);
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
    GetWindowText(hExpenseEdit, buffer, _countof(buffer));
    GetWindowText(hExpenseDesc, desc, _countof(desc));

    double amount = _wtof(buffer);
    if (amount <= 0 || wcslen(desc) == 0) {
        MessageBox(hwnd, L"Введите корректные данные!", L"Ошибка", MB_OK | MB_ICONWARNING);
        return;
    }

    if (amount > balance) {
        MessageBox(hwnd, L"Недостаточно средств!", L"Ошибка", MB_OK | MB_ICONERROR);
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

        CreateWindow(L"STATIC", L"Сумма дохода:", WS_VISIBLE | WS_CHILD, 20, 60, 100, 20, hwnd, nullptr, nullptr, nullptr);
        hIncomeEdit = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 130, 60, 100, 20, hwnd, nullptr, nullptr, nullptr);
        CreateWindow(L"STATIC", L"Описание:", WS_VISIBLE | WS_CHILD, 240, 60, 70, 20, hwnd, nullptr, nullptr, nullptr);
        hIncomeDesc = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 310, 60, 150, 20, hwnd, nullptr, nullptr, nullptr);
        hAddIncomeButton = CreateWindow(L"BUTTON", L"Добавить доход", WS_VISIBLE | WS_CHILD, 470, 60, 110, 20, hwnd, (HMENU)1, nullptr, nullptr);

        CreateWindow(L"STATIC", L"Сумма расхода:", WS_VISIBLE | WS_CHILD, 20, 100, 100, 20, hwnd, nullptr, nullptr, nullptr);
        hExpenseEdit = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 130, 100, 100, 20, hwnd, nullptr, nullptr, nullptr);
        CreateWindow(L"STATIC", L"Описание:", WS_VISIBLE | WS_CHILD, 240, 100, 70, 20, hwnd, nullptr, nullptr, nullptr);
        hExpenseDesc = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 310, 100, 150, 20, hwnd, nullptr, nullptr, nullptr);
        hAddExpenseButton = CreateWindow(L"BUTTON", L"Добавить расход", WS_VISIBLE | WS_CHILD, 470, 100, 110, 20, hwnd, (HMENU)2, nullptr, nullptr);

        hHistoryList = CreateWindow(L"LISTBOX", L"", WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_NOINTEGRALHEIGHT, 20, 140, 560, 220, hwnd, nullptr, nullptr, nullptr);

        UpdateUI(hwnd);
    } break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1) {
            AddIncome(hwnd);
        }
        else if (LOWORD(wParam) == 2) {
            AddExpense(hwnd);
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

void ShowMainWindow(HINSTANCE hInstance) {
    LoadData();

    const wchar_t CLASS_NAME[] = L"WalletWindowClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Кошелёк",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
        nullptr, nullptr, hInstance, nullptr
    );

    if (!hwnd) return;

    ShowWindow(hwnd, SW_SHOWDEFAULT);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}