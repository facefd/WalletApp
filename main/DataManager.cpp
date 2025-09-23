// DataManager.cpp
#include "WalletApp.h"

void LoadData() {
    incomes.clear();
    expenses.clear();
    balance = 0.0;

    // Загрузка доходов
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(g_db, "SELECT amount, description, timestamp FROM incomes ORDER BY timestamp", -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            double amount = sqlite3_column_double(stmt, 0);
            const char* desc_utf8 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::time_t ts = static_cast<std::time_t>(sqlite3_column_int64(stmt, 2));

            if (desc_utf8) {
                int len = MultiByteToWideChar(CP_UTF8, 0, desc_utf8, -1, nullptr, 0);
                std::wstring wdesc(len, 0);
                MultiByteToWideChar(CP_UTF8, 0, desc_utf8, -1, &wdesc[0], len);
                incomes.push_back({ amount, wdesc, ts });
                balance += amount;
            }
        }
        sqlite3_finalize(stmt);
    }

    // Загрузка расходов
    rc = sqlite3_prepare_v2(g_db, "SELECT amount, description, timestamp FROM expenses ORDER BY timestamp", -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            double amount = sqlite3_column_double(stmt, 0);
            const char* desc_utf8 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::time_t ts = static_cast<std::time_t>(sqlite3_column_int64(stmt, 2));

            if (desc_utf8) {
                int len = MultiByteToWideChar(CP_UTF8, 0, desc_utf8, -1, nullptr, 0);
                std::wstring wdesc(len, 0);
                MultiByteToWideChar(CP_UTF8, 0, desc_utf8, -1, &wdesc[0], len);
                expenses.push_back({ amount, wdesc, ts });
                balance -= amount;
            }
        }
        sqlite3_finalize(stmt);
    }
}

void SaveIncome(const Transaction& t) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO incomes (amount, description, timestamp) VALUES (?, ?, ?);";
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return;

    sqlite3_bind_double(stmt, 1, t.amount);

    int len = WideCharToMultiByte(CP_UTF8, 0, t.description.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string utf8_desc(len, 0);
    WideCharToMultiByte(CP_UTF8, 0, t.description.c_str(), -1, &utf8_desc[0], len, nullptr, nullptr);
    sqlite3_bind_text(stmt, 2, utf8_desc.c_str(), -1, SQLITE_STATIC);

    sqlite3_bind_int64(stmt, 3, static_cast<sqlite3_int64>(t.timestamp));

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void SaveExpense(const Transaction& t) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO expenses (amount, description, timestamp) VALUES (?, ?, ?);";
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return;

    sqlite3_bind_double(stmt, 1, t.amount);

    int len = WideCharToMultiByte(CP_UTF8, 0, t.description.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string utf8_desc(len, 0);
    WideCharToMultiByte(CP_UTF8, 0, t.description.c_str(), -1, &utf8_desc[0], len, nullptr, nullptr);
    sqlite3_bind_text(stmt, 2, utf8_desc.c_str(), -1, SQLITE_STATIC);

    sqlite3_bind_int64(stmt, 3, static_cast<sqlite3_int64>(t.timestamp));

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void SaveBalance() {
    // Баланс вычисляется динамически — не сохраняем отдельно
}

bool CheckUserLogin(const std::wstring& login, const std::wstring& password, UserRole* role) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT role FROM users WHERE login = ? AND password = ?;";
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;

    auto to_utf8 = [](const std::wstring& wstr) -> std::string {
        if (wstr.empty()) return "";
        int len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
        std::string str(len, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], len, nullptr, nullptr);
        return str;
        };

    std::string login_utf8 = to_utf8(login);
    std::string pass_utf8 = to_utf8(password);

    sqlite3_bind_text(stmt, 1, login_utf8.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, pass_utf8.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* role_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        if (role_str) {
            if (strcmp(role_str, "admin") == 0) *role = UserRole::Admin;
            else if (strcmp(role_str, "user") == 0) *role = UserRole::User;
        }
        sqlite3_finalize(stmt);
        return true;
    }

    sqlite3_finalize(stmt);
    return false;
}