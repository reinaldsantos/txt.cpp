#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <string>
#include <vector>
#include <sqlite3.h>
#include "../models/Company.h"
#include "../models/Task.h"

class DatabaseManager {
private:
    sqlite3* db;
    bool isConnected;
    std::string dbPath;

    bool createTables();
    bool initializeDatabase();
    bool migrateCnpjToNipc();

public:
    DatabaseManager(const std::string& dbPath);
    ~DatabaseManager();
    
    // Funções de gerenciamento de empresas
    bool createCompany(const Company& company);
    bool deleteCompany(const std::string& name);
    std::vector<Company> getAllCompanies();
    Company getCompany(const std::string& nipc);
    bool updateCompanyBalance(const std::string& nipc, double amount);
    double getCompanyBalance(const std::string& nipc);
    
    // Funções de gerenciamento de tarefas
    bool createTask(const Task& task);
    bool deleteTask(int taskId);
    bool updateTaskStatus(int taskId, bool completed);
    std::vector<Task> getCompanyTasks(const std::string& companyNipc);
    std::vector<Task> getAllTasks();
    
    bool isConnectedToDatabase() const { return isConnected; }
};

#endif // DATABASE_MANAGER_H 