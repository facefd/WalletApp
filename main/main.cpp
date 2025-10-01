// main.cpp
#include "WalletApp.h"

// Определение глобальных переменных
std::vector<Transaction> incomes;
std::vector<Transaction> expenses;
double balance = 0.0;
UserRole currentUserRole = UserRole::None;
std::wstring currentUserName = L"";
sqlite3* g_db = nullptr;

HWND hBalanceLabel = nullptr;
HWND hIncomeEdit = nullptr, hIncomeDesc = nullptr;
HWND hExpenseEdit = nullptr, hExpenseDesc = nullptr;
HWND hHistoryList = nullptr;
HWND hAddIncomeButton = nullptr, hAddExpenseButton = nullptr;
HWND hLoginButton = nullptr, hLoginEdit = nullptr, hPasswordEdit = nullptr, hLoginStatus = nullptr;
HWND hRegisterWindow = nullptr;
HWND hRegLoginEdit = nullptr, hRegPassEdit = nullptr, hRegConfirmEdit = nullptr, hRegEmailEdit = nullptr, hRegRoleCombo = nullptr, hRegStatusLabel = nullptr;
HWND hVerifyCodeEdit = nullptr; // ← Единственное определение!

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int) {
    if (sqlite3_open("wallet.db", &g_db) != SQLITE_OK) {
        MessageBox(nullptr, L"Не удалось открыть базу данных!", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }

    const char* create_users =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "login TEXT UNIQUE NOT NULL, "
        "password TEXT NOT NULL, "
        "role TEXT NOT NULL CHECK(role IN ('admin', 'user')), "
        "email TEXT"
        ");";

    const char* create_incomes =
        "CREATE TABLE IF NOT EXISTS incomes ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "amount REAL NOT NULL, "
        "description TEXT NOT NULL, "
        "timestamp INTEGER NOT NULL"
        ");";

    const char* create_expenses =
        "CREATE TABLE IF NOT EXISTS expenses ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "amount REAL NOT NULL, "
        "description TEXT NOT NULL, "
        "timestamp INTEGER NOT NULL"
        ");";

    char* errMsg = nullptr;
    sqlite3_exec(g_db, create_users, nullptr, nullptr, &errMsg); sqlite3_free(errMsg);
    sqlite3_exec(g_db, create_incomes, nullptr, nullptr, &errMsg); sqlite3_free(errMsg);
    sqlite3_exec(g_db, create_expenses, nullptr, nullptr, &errMsg); sqlite3_free(errMsg);

    sqlite3_exec(g_db, "INSERT OR IGNORE INTO users (login, password, role) VALUES ('admin', '123', 'admin');", nullptr, nullptr, nullptr);
    sqlite3_exec(g_db, "INSERT OR IGNORE INTO users (login, password, role) VALUES ('user', '456', 'user');", nullptr, nullptr, nullptr);

    ShowLoginWindow(hInstance);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    sqlite3_close(g_db);
    return 0;
}