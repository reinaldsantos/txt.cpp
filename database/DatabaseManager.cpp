#include "DatabaseManager.h"
#include <iostream>

DatabaseManager::DatabaseManager(const std::string& dbPath) : dbPath(dbPath), db(nullptr), isConnected(false) {
    connect();
}

DatabaseManager::~DatabaseManager() {
    if (isConnected && db) {
        sqlite3_close(db);
    }
}

bool DatabaseManager::connect() {
    if (isConnected) return true;
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Erro ao abrir banco de dados: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        db = nullptr;
        isConnected = false;
        return false;
    }
    isConnected = true;
    createTables();
    return true;
}

void DatabaseManager::createTables() {
    if (!isConnected) return;
    const char* sql = "CREATE TABLE IF NOT EXISTS companies ("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                     "name TEXT NOT NULL,"
                     "location TEXT NOT NULL,"
                     "employee_name TEXT NOT NULL,"
                     "loan_amount REAL NOT NULL,"
                     "loan_approved INTEGER NOT NULL,"
                     "created_at DATETIME DEFAULT CURRENT_TIMESTAMP);";
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "Erro ao criar tabela: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}

bool DatabaseManager::createCompany(const Company& company) {
    if (!isConnected) return false;
    const char* sql = "INSERT INTO companies (name, location, employee_name, loan_amount, loan_approved) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Erro ao preparar statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    sqlite3_bind_text(stmt, 1, company.getName().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, company.getLocation().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, company.getEmployeeName().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 4, company.getLoanAmount());
    sqlite3_bind_int(stmt, 5, company.isLoanApproved() ? 1 : 0);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

std::vector<Company> DatabaseManager::getAllCompanies() {
    std::vector<Company> companies;
    if (!isConnected) return companies;
    const char* sql = "SELECT name, location, employee_name, loan_amount, loan_approved FROM companies ORDER BY created_at DESC;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return companies;
    }
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string location = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string employeeName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        double loanAmount = sqlite3_column_double(stmt, 3);
        bool loanApproved = sqlite3_column_int(stmt, 4) != 0;
        Company company(name, location, employeeName, loanAmount);
        if (!loanApproved) {
            // Força o status negado se não aprovado
            *(bool*)((char*)&company + sizeof(std::string)*3 + sizeof(double)) = false;
        }
        companies.push_back(company);
    }
    sqlite3_finalize(stmt);
    return companies;
} 