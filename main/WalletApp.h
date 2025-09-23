// WalletApp.h
#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>
#include "sqlite3.h"

#pragma comment(linker, "-mwindows")

// Перечисление ролей пользователя
enum class UserRole { None, User, Admin };

// Структура транзакции
struct Transaction {
    double amount;
    std::wstring description;
    std::time_t timestamp;
};

// Глобальные переменные
extern std::vector<Transaction> incomes;
extern std::vector<Transaction> expenses;
extern double balance;
extern UserRole currentUserRole;
extern std::wstring currentUserName;
extern sqlite3* g_db;

// Глобальные дескрипторы элементов интерфейса
extern HWND hBalanceLabel;
extern HWND hIncomeEdit;
extern HWND hIncomeDesc;
extern HWND hExpenseEdit;
extern HWND hExpenseDesc;
extern HWND hHistoryList;
extern HWND hAddIncomeButton;
extern HWND hAddExpenseButton;
extern HWND hLoginButton;
extern HWND hLoginEdit;
extern HWND hPasswordEdit;
extern HWND hLoginStatus;

// Прототипы функций
std::wstring getCurrentTime();
void LoadData();
void SaveIncome(const Transaction& t);
void SaveExpense(const Transaction& t);
void SaveBalance();
bool CheckUserLogin(const std::wstring& login, const std::wstring& password, UserRole* role);
void UpdateUI(HWND hwnd);
void AddIncome(HWND hwnd);
void AddExpense(HWND hwnd);

// Функции отображения окон
void ShowMainWindow(HINSTANCE hInstance);
void ShowLoginWindow(HINSTANCE hInstance);

// Объявления оконных процедур
LRESULT CALLBACK LoginWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);