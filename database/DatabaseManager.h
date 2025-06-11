#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <string>
#include <vector>
#include <sqlite3.h>
#include "../models/Company.h"

class DatabaseManager {
private:
    sqlite3* db;
    bool isConnected;
    std::string dbPath;

    bool createTables();
    bool initializeDatabase();

public:
    DatabaseManager(const std::string& dbPath);
    ~DatabaseManager();
    
    bool createCompany(const Company& company);
    bool deleteCompany(const std::string& name);
    std::vector<Company> getAllCompanies();
    Company getCompany(const std::string& name);
    bool updateCompanyBalance(const std::string& name, double amount);
    double getCompanyBalance(const std::string& name);
    bool isConnectedToDatabase() const { return isConnected; }
};

#endif // DATABASE_MANAGER_H 