// WalletApp.h
#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <vector>
#include <string>
#include <ctime>
#include "sqlite3.h"

#pragma comment(lib, "comctl32.lib")

enum class UserRole { None, User, Admin };

struct Transaction {
    double amount;
    std::wstring description;
    time_t timestamp;
};

// Глобальные переменные
extern std::vector<Transaction> incomes;
extern std::vector<Transaction> expenses;
extern double balance;
extern UserRole currentUserRole;
extern std::wstring currentUserName;
extern sqlite3* g_db;

// GUI дескрипторы
extern HWND hBalanceLabel;
extern HWND hIncomeEdit, hIncomeDesc, hExpenseEdit, hExpenseDesc, hHistoryList;
extern HWND hAddIncomeButton, hAddExpenseButton;
extern HWND hLoginButton, hLoginEdit, hPasswordEdit, hLoginStatus;
extern HWND hRegisterWindow;
extern HWND hRegLoginEdit, hRegPassEdit, hRegConfirmEdit, hRegRoleCombo, hRegStatusLabel;

// Утилиты
std::wstring getCurrentTime();
std::string wstr_to_utf8(const std::wstring& wstr);
std::wstring utf8_to_wstr(const std::string& str);

// Функции данных
void LoadData();
void SaveIncome(const Transaction& t);
void SaveExpense(const Transaction& t);
void SaveBalance();
bool CheckUserLogin(const std::wstring& login, const std::wstring& password, UserRole* role);

// GUI функции
void UpdateUI(HWND hwnd);
void AddIncome(HWND hwnd);
void AddExpense(HWND hwnd);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void ShowMainWindow(HINSTANCE hInstance);

LRESULT CALLBACK LoginWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void ShowLoginWindow(HINSTANCE hInstance);

LRESULT CALLBACK RegistrationWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void ShowRegistrationWindow(HINSTANCE hInstance);