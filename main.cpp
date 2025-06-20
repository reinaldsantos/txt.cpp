/**
 * @file main.cpp
 * @brief Programa para gerenciamento de empréstimos
 * @details Este programa permite visualizar e adicionar empréstimos
 *          em um banco de dados SQLite.
 */

#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <vector>
#include <fstream>
#include <limits>
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#endif
#include "database/DatabaseManager.h"
#include "models/Company.h"
#include "advanced_features.h"
#include "task_list.h"
#include <sstream>
#include <algorithm>

// Função para configurar o console para UTF-8
bool setupConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    return true;
#else
    return true;
#endif
}

void displayHeader() {
    std::cout << std::left
              << std::setw(30) << "Empresa"
              << std::setw(20) << "NIPC"
              << std::setw(20) << "Localização"
              << std::setw(30) << "Funcionário"
              << std::setw(15) << "Valor"
              << std::setw(10) << "Status"
              << std::setw(15) << "Saldo"
              << "\n";
    std::cout << std::string(135, '-') << "\n";
}

void displayCompany(const Company& company) {
    std::cout << std::left
              << std::setw(30) << company.getName()
              << std::setw(20) << company.getNIPC()
              << std::setw(20) << company.getLocation()
              << std::setw(30) << company.getEmployeeName()
              << std::setw(15) << std::fixed << std::setprecision(2) << company.getLoanAmount()
              << std::setw(10) << (company.isLoanApproved() ? "Aprovado" : "Rejeitado")
              << std::setw(15) << std::fixed << std::setprecision(2) << company.getBalance()
              << "\n";
}

void displayLog() {
    std::cout << "\n=== Log de Empréstimos ===\n\n";
    DatabaseManager dbManager("database/bank.db");
    auto companies = dbManager.getAllCompanies();
    
    if (companies.empty()) {
        std::cout << "Nenhum empréstimo registrado.\n";
        return;
    }

    displayHeader();
    for (const auto& company : companies) {
        displayCompany(company);
    }
}

// Protótipo necessário para uso em addNewLoan
Company getCompanyByNipcOrName(DatabaseManager& dbManager, const std::string& input);

// Implementação da função para evitar erro de linkagem
Company getCompanyByNipcOrName(DatabaseManager& dbManager, const std::string& input) {
    return dbManager.getCompanyByNipcOrName(input);
}

void addNewLoan(DatabaseManager& dbManager) {
    std::cout << "\n=== Novo Empréstimo ===\n\n";
    char jaCadastrada;
    std::cout << "A empresa já está cadastrada? (s/n): ";
    std::cin >> jaCadastrada;
    std::cin.ignore();
    if (jaCadastrada == 's' || jaCadastrada == 'S') {
        std::string input;
        std::cout << "NIPC ou Nome da empresa: ";
        std::getline(std::cin, input);
        Company company = getCompanyByNipcOrName(dbManager, input);
        if (company.getName().empty()) {
            std::cout << "\nEmpresa não encontrada!\n";
            return;
        }
        double novoEmprestimo;
        std::cout << "Valor do novo empréstimo: ";
        while (!(std::cin >> novoEmprestimo) || novoEmprestimo <= 0) {
            std::cout << "Valor inválido. Digite novamente: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cin.ignore();
        // Subtrai o valor do novo empréstimo do saldo (fica mais negativo)
        dbManager.updateCompanyBalance(company.getNIPC(), -novoEmprestimo);
        std::cout << "\nNovo empréstimo registrado para a empresa!\n";
        std::cout << "Novo saldo: R$ " << std::fixed << std::setprecision(2) << dbManager.getCompanyBalance(company.getNIPC()) << "\n";
        return;
    }
    // Fluxo normal para nova empresa
    std::string name, nipc, location, employeeName;
    double amount;
    std::cout << "Nome da empresa: ";
    std::getline(std::cin, name);
    std::cout << "NIPC: ";
    std::getline(std::cin, nipc);
    // Verifica se a empresa já existe
    Company existingCompany = dbManager.getCompany(nipc);
    if (!existingCompany.getName().empty()) {
        std::cout << "\nEmpresa com este NIPC já existe!\n";
        std::cout << "Nome: " << existingCompany.getName() << "\n";
        std::cout << "Local: " << existingCompany.getLocation() << "\n";
        std::cout << "Funcionário: " << existingCompany.getEmployeeName() << "\n";
        std::cout << "Valor atual: R$ " << std::fixed << std::setprecision(2) << existingCompany.getLoanAmount() << "\n";
        std::cout << "Saldo atual: R$ " << std::fixed << std::setprecision(2) << existingCompany.getBalance() << "\n";
        return;
    }
    std::cout << "Local: ";
    std::getline(std::cin, location);
    std::cout << "Nome do funcionário: ";
    std::getline(std::cin, employeeName);
    std::cout << "Valor do empréstimo: ";
    while (!(std::cin >> amount) || amount <= 0) {
        std::cout << "Valor inválido. Digite novamente: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore();
    // Cria uma nova empresa e salva no banco de dados
    Company newCompany(name, nipc, location, employeeName, amount);
    if (dbManager.createCompany(newCompany)) {
        std::cout << "\nEmpréstimo registrado com sucesso!\n";
    } else {
        std::cout << "\nErro ao registrar empréstimo.\n";
    }
}

void checkBalance(DatabaseManager& dbManager) {
    std::cout << "\n=== Consultar Saldo ===\n\n";
    
    std::string nipc;
    std::cout << "NIPC da empresa: ";
    std::getline(std::cin, nipc);
    
    double balance = dbManager.getCompanyBalance(nipc);
    if (balance >= 0) {
        std::cout << "\nSaldo atual: R$ " << std::fixed << std::setprecision(2) << balance << "\n";
    } else {
        std::cout << "\nEmpresa não encontrada!\n";
    }
}

void depositMoney(DatabaseManager& dbManager) {
    std::cout << "\n=== Depositar Dinheiro ===\n\n";
    
    std::string nipc;
    double amount;
    
    std::cout << "NIPC da empresa: ";
    std::getline(std::cin, nipc);
    
    std::cout << "Valor a depositar: R$ ";
    while (!(std::cin >> amount) || amount <= 0) {
        std::cout << "Valor inválido. Digite novamente: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore();
    
    if (dbManager.updateCompanyBalance(nipc, amount)) {
        std::cout << "\nDepósito realizado com sucesso!\n";
        std::cout << "Novo saldo: R$ " << std::fixed << std::setprecision(2) 
                  << dbManager.getCompanyBalance(nipc) << "\n";
    } else {
        std::cout << "\nErro ao realizar depósito. Empresa não encontrada!\n";
    }
}

// Função para criar tabela de usuários se não existir
void createUsersTable(DatabaseManager& dbManager) {
    sqlite3* db;
    sqlite3_open("database/bank.db", &db);
    const char* sql = "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT UNIQUE, password TEXT);";
    char* errMsg = nullptr;
    sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    sqlite3_close(db);
}

// Função para criar usuário admin padrão
void createDefaultAdmin(DatabaseManager& dbManager) {
    dbManager.createUser("admin", "senha123");
}

// Função para ler senha oculta
std::string getPassword() {
    std::string password;
    char ch;
    while ((ch = _getch()) != '\r') { // '\r' é Enter
        if (ch == '\b') { // Backspace
            if (!password.empty()) {
                password.pop_back();
                std::cout << "\b \b";
            }
        } else {
            password.push_back(ch);
            std::cout << "#";
        }
    }
    std::cout << std::endl;
    return password;
}

// Função de autenticação
bool login(DatabaseManager& dbManager) {
    std::string username, password;
    std::cout << "\n=== Login ===\nUsuário: ";
    std::getline(std::cin, username);
    std::cout << "Senha: ";
    password = getPassword();
    return dbManager.authenticateUser(username, password);
}

// Nova função para consultar saldo por NIPC ou nome
void checkBalanceByNipcOrName(DatabaseManager& dbManager) {
    std::cout << "\n=== Consultar Saldo ===\n\n";
    std::string input;
    std::cout << "NIPC ou Nome da empresa: ";
    std::getline(std::cin, input);
    Company company = getCompanyByNipcOrName(dbManager, input);
    if (!company.getName().empty()) {
        std::cout << "\nEmpresa: " << company.getName() << "\nSaldo atual: R$ " << std::fixed << std::setprecision(2) << company.getBalance() << "\n";
    } else {
        std::cout << "\nEmpresa não encontrada!\n";
    }
}

// Nova função para depósito por NIPC ou nome
void depositMoneyByNipcOrName(DatabaseManager& dbManager) {
    std::cout << "\n=== Depositar Dinheiro ===\n\n";
    std::string input;
    double amount;
    std::cout << "NIPC ou Nome da empresa: ";
    std::getline(std::cin, input);
    Company company = getCompanyByNipcOrName(dbManager, input);
    if (company.getName().empty()) {
        std::cout << "\nEmpresa não encontrada!\n";
        return;
    }
    std::cout << "Valor a depositar: R$ ";
    while (!(std::cin >> amount) || amount <= 0) {
        std::cout << "Valor inválido. Digite novamente: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore();
    // Soma o valor ao saldo
    if (dbManager.updateCompanyBalance(company.getNIPC(), amount)) {
        std::cout << "\nDepósito realizado com sucesso!\nNovo saldo: R$ " << std::fixed << std::setprecision(2) << dbManager.getCompanyBalance(company.getNIPC()) << "\n";
    } else {
        std::cout << "\nErro ao realizar depósito. Empresa não encontrada!\n";
    }
}

// Nova função para pagamento por NIPC ou nome
void paymentByNipcOrName(DatabaseManager& dbManager) {
    std::cout << "\n=== Fazer Pagamento ===\n\n";
    std::string input;
    double amount;
    std::cout << "NIPC ou Nome da empresa: ";
    std::getline(std::cin, input);
    Company company = getCompanyByNipcOrName(dbManager, input);
    if (company.getName().empty()) {
        std::cout << "\nEmpresa não encontrada!\n";
        return;
    }
    double saldo = company.getBalance();
    std::cout << "Saldo atual: R$ " << std::fixed << std::setprecision(2) << saldo << "\n";
    if (saldo >= 0) {
        std::cout << "\nA empresa não possui dívida para pagar!\n";
        return;
    }
    double maxPagamento = -saldo;
    std::cout << "Valor do pagamento (máximo permitido: R$ " << std::fixed << std::setprecision(2) << maxPagamento << "): ";
    while (!(std::cin >> amount) || amount <= 0) {
        std::cout << "Valor inválido. Digite novamente: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Valor do pagamento (máximo permitido: R$ " << std::fixed << std::setprecision(2) << maxPagamento << "): ";
    }
    if (amount > maxPagamento) {
        std::cout << "Valor excede a dívida! O pagamento máximo permitido é R$ " << std::fixed << std::setprecision(2) << maxPagamento << "\n";
        std::cout << "Deseja continuar e pagar o valor máximo permitido? (s/n): ";
        char opcao;
        std::cin >> opcao;
        std::cin.ignore();
        if (opcao == 's' || opcao == 'S') {
            amount = maxPagamento;
        } else {
            std::cout << "Operação cancelada.\n";
            return;
        }
    } else {
        std::cin.ignore();
    }
    // Garante que o pagamento nunca ultrapasse o necessário para zerar a dívida
    if (amount > maxPagamento) {
        amount = maxPagamento;
    }
    if (dbManager.updateCompanyBalance(company.getNIPC(), amount)) {
        double novoSaldo = dbManager.getCompanyBalance(company.getNIPC());
        // Se por algum motivo o saldo ficou positivo, ajusta para zero
        if (novoSaldo > 0.0) {
            double ajuste = -novoSaldo;
            dbManager.updateCompanyBalance(company.getNIPC(), ajuste);
            novoSaldo = 0.0;
        }
        std::cout << "\nPagamento realizado com sucesso!\nNovo saldo: R$ " << std::fixed << std::setprecision(2) << novoSaldo << "\n";
    } else {
        std::cout << "\nErro ao realizar pagamento. Empresa não encontrada!\n";
    }
}

// Função para mostrar relatórios
void showReports(DatabaseManager& dbManager) {
    int op;
    do {
        std::cout << "\n=== Relatórios ===\n";
        std::cout << "1. Resumo financeiro\n2. Empresas inadimplentes\n0. Voltar\nEscolha uma opção: ";
        std::cin >> op;
        std::cin.ignore();
        if (op == 1) {
            double totalEmprestado = dbManager.getTotalEmprestado();
            double totalRecebido = dbManager.getTotalRecebido();
            double saldoGeral = dbManager.getSaldoGeral();
            std::cout << "\n--- Resumo Financeiro ---\n";
            std::cout << "Total emprestado: R$ " << std::fixed << std::setprecision(2) << totalEmprestado << "\n";
            std::cout << "Total recebido:   R$ " << std::fixed << std::setprecision(2) << totalRecebido << "\n";
            std::cout << "Saldo geral:      R$ " << std::fixed << std::setprecision(2) << saldoGeral << "\n";
        } else if (op == 2) {
            auto inadimplentes = dbManager.getEmpresasInadimplentes();
            std::cout << "\n--- Empresas Inadimplentes ---\n";
            if (inadimplentes.empty()) {
                std::cout << "Nenhuma empresa inadimplente.\n";
            } else {
                displayHeader();
                for (const auto& company : inadimplentes) {
                    displayCompany(company);
                }
            }
        }
    } while (op != 0);
}

int main() {
    try {
        if (!setupConsole()) {
            std::cerr << "Erro ao configurar o console para UTF-8.\n";
            return 1;
        }
        #ifdef _WIN32
        system("if not exist database mkdir database");
        #else
        system("mkdir -p database");
        #endif
        DatabaseManager dbManager("database/bank.db");
        createUsersTable(dbManager);
        createDefaultAdmin(dbManager);
        // Login antes do menu principal
        while (!login(dbManager)) {
            std::cout << "\nUsuário ou senha incorretos. Tente novamente.\n";
        }
        while (true) {
            std::cout << "\n=== Sistema Bancário ===\n\n";
            std::cout << "1. Ver histórico de empréstimos\n";
            std::cout << "2. Funcionalidades Avançadas\n";
            std::cout << "3. Adicionar novo empréstimo\n";
            std::cout << "4. Consultar saldo (por NIPC ou nome)\n";
            std::cout << "5. Depositar dinheiro (por NIPC ou nome)\n";
            std::cout << "6. Fazer pagamento (por NIPC ou nome)\n";
            std::cout << "7. Gerenciar Tarefas\n";
            std::cout << "8. Relatórios\n";
            std::cout << "0. Sair\n";
            std::cout << "Escolha uma opção: ";
            int choice;
            std::cin >> choice;
            std::cin.ignore();
            switch (choice) {
                case 1: {
                    std::cout << "\n=== Histórico de Empréstimos ===\n\n";
                    auto companies = dbManager.getAllCompanies();
                    if (companies.empty()) {
                        std::cout << "Nenhum registro encontrado.\n";
                    } else {
                        displayHeader();
                        for (const auto& company : companies) {
                            displayCompany(company);
                        }
                    }
                    break;
                }
                case 2:
                    showAdvancedMenu();
                    break;
                case 3:
                    addNewLoan(dbManager);
                    break;
                case 4:
                    checkBalanceByNipcOrName(dbManager);
                    break;
                case 5:
                    depositMoneyByNipcOrName(dbManager);
                    break;
                case 6:
                    paymentByNipcOrName(dbManager);
                    break;
                case 7:
                    taskManagement();
                    break;
                case 8:
                    showReports(dbManager);
                    break;
                case 0:
                    std::cout << "\nSaindo do sistema...\n";
                    return 0;
                default:
                    std::cout << "\nOpção inválida!\n";
            }
            std::cout << "\nPressione Enter para continuar...";
            std::cin.get();
        }
    } catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << std::endl;
        return 1;
    }
}