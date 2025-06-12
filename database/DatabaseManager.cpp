#include "DatabaseManager.h"
#include <iostream>
#include <fstream>

DatabaseManager::DatabaseManager(const std::string& path) : dbPath(path), db(nullptr), isConnected(false) {
    initializeDatabase();
}

bool DatabaseManager::initializeDatabase() {
    // Conecta ao banco de dados
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Erro ao abrir banco de dados: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }
    
    isConnected = true;
    
    // Cria as tabelas se não existirem
    if (!createTables()) {
        std::cerr << "Erro ao criar tabelas" << std::endl;
        sqlite3_close(db);
        isConnected = false;
        return false;
    }

    // Migra a coluna cnpj para nipc se necessário
    if (!migrateCnpjToNipc()) {
        std::cerr << "Erro ao migrar coluna cnpj para nipc" << std::endl;
        sqlite3_close(db);
        isConnected = false;
        return false;
    }
    
    return true;
}

bool DatabaseManager::migrateCnpjToNipc() {
    if (!isConnected) return false;

    // Verifica se a coluna cnpj existe
    const char* checkSql = "PRAGMA table_info(companies);";
    sqlite3_stmt* stmt;
    bool hasCnpjColumn = false;
    
    if (sqlite3_prepare_v2(db, checkSql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* columnName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            if (columnName && std::string(columnName) == "cnpj") {
                hasCnpjColumn = true;
                break;
            }
        }
    }
    sqlite3_finalize(stmt);

    if (hasCnpjColumn) {
        // Renomeia a coluna cnpj para nipc
        const char* migrateSql = "ALTER TABLE companies RENAME COLUMN cnpj TO nipc;";
        char* errMsg = nullptr;
        
        if (sqlite3_exec(db, migrateSql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::cerr << "Erro ao migrar coluna: " << errMsg << std::endl;
            sqlite3_free(errMsg);
            return false;
        }
        
        std::cout << "Migração de cnpj para nipc realizada com sucesso!" << std::endl;
    }
    
    return true;
}

DatabaseManager::~DatabaseManager() {
    if (db) {
        sqlite3_close(db);
    }
}

bool DatabaseManager::createTables() {
    if (!isConnected) return false;
    
    const char* sql = "CREATE TABLE IF NOT EXISTS companies ("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                     "name TEXT NOT NULL,"
                     "nipc TEXT NOT NULL UNIQUE,"
                     "location TEXT NOT NULL,"
                     "employee_name TEXT NOT NULL,"
                     "loan_amount REAL NOT NULL,"
                     "loan_approved INTEGER NOT NULL DEFAULT 1,"
                     "balance REAL DEFAULT 0.0,"
                     "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
                     "deleted INTEGER DEFAULT 0"
                     ");"
                     
                     "CREATE TABLE IF NOT EXISTS tasks ("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                     "description TEXT NOT NULL,"
                     "completed INTEGER NOT NULL DEFAULT 0,"
                     "company_nipc TEXT NOT NULL,"
                     "created_at INTEGER NOT NULL,"
                     "completed_at INTEGER,"
                     "FOREIGN KEY (company_nipc) REFERENCES companies(nipc)"
                     ");";
    
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        std::cerr << "Erro ao criar tabelas: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    return true;
}

bool DatabaseManager::createCompany(const Company& company) {
    if (!isConnected) return false;
    
    std::cout << "[DEBUG] Salvando empresa:" << std::endl
              << "  Nome: '" << company.getName() << "'" << std::endl
              << "  NIPC: '" << company.getNIPC() << "'" << std::endl
              << "  Local: '" << company.getLocation() << "'" << std::endl
              << "  Funcionário: '" << company.getEmployeeName() << "'" << std::endl
              << "  Valor: " << company.getLoanAmount() << std::endl;
    
    const char* sql = "INSERT INTO companies (name, nipc, location, employee_name, loan_amount, loan_approved) "
                     "VALUES (?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Erro ao preparar inserção: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, company.getName().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, company.getNIPC().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, company.getLocation().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, company.getEmployeeName().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 5, company.getLoanAmount());
    sqlite3_bind_int(stmt, 6, company.isLoanApproved() ? 1 : 0);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        std::cerr << "Erro ao inserir empresa: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    std::cout << "[DEBUG] Empresa salva com sucesso!" << std::endl;
    return true;
}

std::vector<Company> DatabaseManager::getAllCompanies() {
    std::vector<Company> companies;
    if (!isConnected) return companies;
    
    const char* sql = "SELECT name, nipc, location, employee_name, loan_amount, loan_approved FROM companies WHERE deleted = 0 OR deleted IS NULL ORDER BY created_at DESC;";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Erro ao preparar consulta: " << sqlite3_errmsg(db) << std::endl;
        return companies;
    }
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        const char* nipc = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* location = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        const char* employeeName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        double loanAmount = sqlite3_column_double(stmt, 4);
        bool loanApproved = sqlite3_column_int(stmt, 5) != 0;
        
        if (name && nipc && location && employeeName) {
            Company company(name, nipc, location, employeeName, loanAmount);
            if (!loanApproved) {
                *(bool*)((char*)&company + sizeof(std::string)*4 + sizeof(double)) = false;
            }
            companies.push_back(company);
        }
    }
    
    sqlite3_finalize(stmt);
    return companies;
}

bool DatabaseManager::deleteCompany(const std::string& companyName) {
    if (!isConnected) return false;

    // Primeiro, remove a empresa
    std::string sql = "UPDATE companies SET deleted = 1 WHERE name = ?;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Erro ao preparar delete: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, companyName.c_str(), -1, SQLITE_STATIC);
    
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);

    if (success) {
        // Se a empresa foi removida com sucesso, limpa o histórico
        std::ofstream logFile("log.txt", std::ios::trunc);
        if (logFile.is_open()) {
            logFile.close();
            std::cout << "\nHistórico de acessos limpo com sucesso!\n";
        } else {
            std::cout << "\nErro ao limpar histórico de acessos.\n";
        }
    }
    
    return success;
}

Company DatabaseManager::getCompany(const std::string& nipc) {
    Company company("", "", "", "", 0.0);
    std::string sql = "SELECT name, nipc, location, employee_name, loan_amount, loan_approved, balance "
                     "FROM companies WHERE nipc = ? AND deleted = 0;";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, nipc.c_str(), -1, SQLITE_STATIC);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string nipc = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::string location = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            std::string employeeName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            double loanAmount = sqlite3_column_double(stmt, 4);
            bool loanApproved = sqlite3_column_int(stmt, 5) != 0;
            double balance = sqlite3_column_double(stmt, 6);
            
            company = Company(name, nipc, location, employeeName, loanAmount);
            company.setBalance(balance);
        }
    }
    
    sqlite3_finalize(stmt);
    return company;
}

bool DatabaseManager::updateCompanyBalance(const std::string& nipc, double amount) {
    std::string sql = "UPDATE companies SET balance = balance + ? WHERE nipc = ? AND deleted = 0;";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    
    if (rc == SQLITE_OK) {
        sqlite3_bind_double(stmt, 1, amount);
        sqlite3_bind_text(stmt, 2, nipc.c_str(), -1, SQLITE_STATIC);
        
        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        
        return rc == SQLITE_DONE;
    }
    
    sqlite3_finalize(stmt);
    return false;
}

double DatabaseManager::getCompanyBalance(const std::string& nipc) {
    std::string sql = "SELECT balance FROM companies WHERE nipc = ? AND deleted = 0;";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, nipc.c_str(), -1, SQLITE_STATIC);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            double balance = sqlite3_column_double(stmt, 0);
            sqlite3_finalize(stmt);
            return balance;
        }
    }
    
    sqlite3_finalize(stmt);
    return -1.0; // Retorna -1 se a empresa não for encontrada
}

bool DatabaseManager::createTask(const Task& task) {
    if (!isConnected) return false;
    
    const char* sql = "INSERT INTO tasks (description, completed, company_nipc, created_at, completed_at) "
                     "VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Erro ao preparar inserção de tarefa: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, task.getDescription().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, task.isCompleted() ? 1 : 0);
    sqlite3_bind_text(stmt, 3, task.getCompanyNipc().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 4, task.getCreatedAt());
    sqlite3_bind_int64(stmt, 5, task.getCompletedAt());
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE;
}

bool DatabaseManager::deleteTask(int taskId) {
    if (!isConnected) return false;
    
    const char* sql = "DELETE FROM tasks WHERE id = ?;";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Erro ao preparar deleção de tarefa: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, taskId);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE;
}

bool DatabaseManager::updateTaskStatus(int taskId, bool completed) {
    if (!isConnected) return false;
    
    const char* sql = "UPDATE tasks SET completed = ?, completed_at = ? WHERE id = ?;";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Erro ao preparar atualização de tarefa: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, completed ? 1 : 0);
    sqlite3_bind_int64(stmt, 2, completed ? time(nullptr) : 0);
    sqlite3_bind_int(stmt, 3, taskId);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE;
}

std::vector<Task> DatabaseManager::getCompanyTasks(const std::string& companyNipc) {
    std::vector<Task> tasks;
    if (!isConnected) return tasks;
    
    const char* sql = "SELECT id, description, completed, created_at, completed_at "
                     "FROM tasks WHERE company_nipc = ? ORDER BY created_at DESC;";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Erro ao preparar consulta de tarefas: " << sqlite3_errmsg(db) << std::endl;
        return tasks;
    }
    
    sqlite3_bind_text(stmt, 1, companyNipc.c_str(), -1, SQLITE_STATIC);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        bool completed = sqlite3_column_int(stmt, 2) != 0;
        time_t createdAt = sqlite3_column_int64(stmt, 3);
        time_t completedAt = sqlite3_column_int64(stmt, 4);
        
        if (description) {
            Task task(id, description, completed, companyNipc, createdAt, completedAt);
            tasks.push_back(task);
        }
    }
    
    sqlite3_finalize(stmt);
    return tasks;
}

std::vector<Task> DatabaseManager::getAllTasks() {
    std::vector<Task> tasks;
    if (!isConnected) return tasks;
    
    const char* sql = "SELECT id, description, completed, company_nipc, created_at, completed_at "
                     "FROM tasks ORDER BY created_at DESC;";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Erro ao preparar consulta de tarefas: " << sqlite3_errmsg(db) << std::endl;
        return tasks;
    }
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        bool completed = sqlite3_column_int(stmt, 2) != 0;
        const char* companyNipc = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        time_t createdAt = sqlite3_column_int64(stmt, 4);
        time_t completedAt = sqlite3_column_int64(stmt, 5);
        
        if (description && companyNipc) {
            Task task(id, description, completed, companyNipc, createdAt, completedAt);
            tasks.push_back(task);
        }
    }
    
    sqlite3_finalize(stmt);
    return tasks;
} 