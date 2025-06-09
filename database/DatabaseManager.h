#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <string>
#include <vector>
#include <sqlite3.h>
#include "../models/Company.h"

class DatabaseManager {
private:
    std::string dbPath;
    sqlite3* db;
    bool isConnected;
    void createTables();

public:
    DatabaseManager(const std::string& dbPath);
    ~DatabaseManager();
    bool connect();
    bool createCompany(const Company& company);
    std::vector<Company> getAllCompanies();
};

#endif // DATABASE_MANAGER_H 