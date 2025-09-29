// DataManager.cpp
#include "WalletApp.h"
#include <ctime>

void LoadData() {
    incomes.clear(); expenses.clear(); balance = 0.0;

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(g_db, "SELECT amount, description, timestamp FROM incomes", -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            double amount = sqlite3_column_double(stmt, 0);
            const char* desc = (const char*)sqlite3_column_text(stmt, 1);
            time_t ts = sqlite3_column_int64(stmt, 2);
            incomes.push_back({ amount, utf8_to_wstr(std::string(desc)), ts });
            balance += amount;
        }
        sqlite3_finalize(stmt);
    }

    if (sqlite3_prepare_v2(g_db, "SELECT amount, description, timestamp FROM expenses", -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            double amount = sqlite3_column_double(stmt, 0);
            const char* desc = (const char*)sqlite3_column_text(stmt, 1);
            time_t ts = sqlite3_column_int64(stmt, 2);
            expenses.push_back({ amount, utf8_to_wstr(std::string(desc)), ts });
            balance -= amount;
        }
        sqlite3_finalize(stmt);
    }
}

void SaveIncome(const Transaction& t) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO incomes (amount, description, timestamp) VALUES (?, ?, ?);";
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_double(stmt, 1, t.amount);
        std::string utf8_desc = wstr_to_utf8(t.description);
        sqlite3_bind_text(stmt, 2, utf8_desc.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int64(stmt, 3, static_cast<sqlite3_int64>(t.timestamp));
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}

void SaveExpense(const Transaction& t) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO expenses (amount, description, timestamp) VALUES (?, ?, ?);";
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_double(stmt, 1, t.amount);
        std::string utf8_desc = wstr_to_utf8(t.description);
        sqlite3_bind_text(stmt, 2, utf8_desc.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int64(stmt, 3, static_cast<sqlite3_int64>(t.timestamp));
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}

void SaveBalance() {}

bool CheckUserLogin(const std::wstring& login, const std::wstring& password, UserRole* role) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT role FROM users WHERE login = ? AND password = ?";
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    std::string login_utf8 = wstr_to_utf8(login);
    std::string pass_utf8 = wstr_to_utf8(password);

    sqlite3_bind_text(stmt, 1, login_utf8.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, pass_utf8.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* role_str = (const char*)sqlite3_column_text(stmt, 0);
        *role = (strcmp(role_str, "admin") == 0) ? UserRole::Admin : UserRole::User;
        sqlite3_finalize(stmt);
        return true;
    }
    sqlite3_finalize(stmt);
    return false;
}