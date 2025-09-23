// main.cpp
#include "WalletApp.h"

// Определение глобальных переменных
std::vector<Transaction> incomes;
std::vector<Transaction> expenses;
double balance = 0.0;
UserRole currentUserRole = UserRole::None;
std::wstring currentUserName = L"";
sqlite3* g_db = nullptr;

// Глобальные дескрипторы
HWND hBalanceLabel = nullptr;
HWND hIncomeEdit = nullptr;
HWND hIncomeDesc = nullptr;
HWND hExpenseEdit = nullptr;
HWND hExpenseDesc = nullptr;
HWND hHistoryList = nullptr;
HWND hAddIncomeButton = nullptr;
HWND hAddExpenseButton = nullptr;
HWND hLoginButton = nullptr;
HWND hLoginEdit = nullptr;
HWND hPasswordEdit = nullptr;
HWND hLoginStatus = nullptr;

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR lpCmdLine, _In_ int nCmdShow) {
    // Инициализация SQLite
    int rc = sqlite3_open("wallet.db", &g_db);
    if (rc != SQLITE_OK) {
        MessageBox(nullptr, L"Не удалось открыть базу данных!", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Создание таблиц
    const char* create_users =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "login TEXT UNIQUE NOT NULL, "
        "password TEXT NOT NULL, "
        "role TEXT NOT NULL CHECK(role IN ('admin', 'user'))"
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
    rc = sqlite3_exec(g_db, create_users, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        MessageBox(nullptr, L"Ошибка создания таблицы users", L"Ошибка", MB_OK | MB_ICONERROR);
        sqlite3_free(errMsg);
        sqlite3_close(g_db);
        return 1;
    }

    rc = sqlite3_exec(g_db, create_incomes, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        MessageBox(nullptr, L"Ошибка создания таблицы incomes", L"Ошибка", MB_OK | MB_ICONERROR);
        sqlite3_free(errMsg);
        sqlite3_close(g_db);
        return 1;
    }

    rc = sqlite3_exec(g_db, create_expenses, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        MessageBox(nullptr, L"Ошибка создания таблицы expenses", L"Ошибка", MB_OK | MB_ICONERROR);
        sqlite3_free(errMsg);
        sqlite3_close(g_db);
        return 1;
    }

    // Вставка тестовых пользователей, если таблица пуста
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(g_db, "SELECT COUNT(*) FROM users", -1, &stmt, nullptr);
    if (rc == SQLITE_OK && sqlite3_step(stmt) == SQLITE_ROW) {
        if (sqlite3_column_int(stmt, 0) == 0) {
            sqlite3_exec(g_db, "INSERT INTO users (login, password, role) VALUES ('admin', '123', 'admin');", nullptr, nullptr, nullptr);
            sqlite3_exec(g_db, "INSERT INTO users (login, password, role) VALUES ('user', '456', 'user');", nullptr, nullptr, nullptr);
        }
    }
    sqlite3_finalize(stmt);

    ShowLoginWindow(hInstance);

    sqlite3_close(g_db);
    return 0;
}