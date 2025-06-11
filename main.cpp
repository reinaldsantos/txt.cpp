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
#endif
#include "database/DatabaseManager.h"
#include "models/Company.h"
#include "advanced_features.h"

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
              << std::setw(20) << "Localização"
              << std::setw(30) << "Funcionário"
              << std::setw(15) << "Valor"
              << std::setw(10) << "Status"
              << "\n";
    std::cout << std::string(105, '-') << "\n";
}

void displayCompany(const Company& company) {
    std::cout << std::left
              << std::setw(30) << company.getName()
              << std::setw(20) << company.getLocation()
              << std::setw(30) << company.getEmployeeName()
              << std::setw(15) << std::fixed << std::setprecision(2) << company.getLoanAmount()
              << std::setw(10) << (company.isLoanApproved() ? "Aprovado" : "Rejeitado")
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

void addNewLoan(DatabaseManager& dbManager) {
    std::cout << "\n=== Novo Empréstimo ===\n\n";
    
    std::string name, cnpj, location, employeeName;
    double amount;
    
    std::cout << "Nome da empresa: ";
    std::getline(std::cin, name);
    
    std::cout << "CNPJ: ";
    std::getline(std::cin, cnpj);
    
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
    Company newCompany(name, cnpj, location, employeeName, amount);
    if (dbManager.createCompany(newCompany)) {
        std::cout << "\nEmpréstimo registrado com sucesso!\n";
    } else {
        std::cout << "\nErro ao registrar empréstimo.\n";
    }
}

void checkBalance(DatabaseManager& dbManager) {
    std::cout << "\n=== Consultar Saldo ===\n\n";
    
    std::string cnpj;
    std::cout << "CNPJ da empresa: ";
    std::getline(std::cin, cnpj);
    
    double balance = dbManager.getCompanyBalance(cnpj);
    if (balance >= 0) {
        std::cout << "\nSaldo atual: R$ " << std::fixed << std::setprecision(2) << balance << "\n";
    } else {
        std::cout << "\nEmpresa não encontrada!\n";
    }
}

void depositMoney(DatabaseManager& dbManager) {
    std::cout << "\n=== Depositar Dinheiro ===\n\n";
    
    std::string cnpj;
    double amount;
    
    std::cout << "CNPJ da empresa: ";
    std::getline(std::cin, cnpj);
    
    std::cout << "Valor a depositar: R$ ";
    while (!(std::cin >> amount) || amount <= 0) {
        std::cout << "Valor inválido. Digite novamente: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore();
    
    if (dbManager.updateCompanyBalance(cnpj, amount)) {
        std::cout << "\nDepósito realizado com sucesso!\n";
        std::cout << "Novo saldo: R$ " << std::fixed << std::setprecision(2) 
                  << dbManager.getCompanyBalance(cnpj) << "\n";
    } else {
        std::cout << "\nErro ao realizar depósito. Empresa não encontrada!\n";
    }
}

int main() {
    try {
        // Configura o console para UTF-8
        if (!setupConsole()) {
            std::cerr << "Erro ao configurar o console para UTF-8.\n";
            return 1;
        }

        // Cria o diretório database se não existir
        #ifdef _WIN32
        system("if not exist database mkdir database");
        #else
        system("mkdir -p database");
        #endif

        while (true) {
            std::cout << "\n=== Sistema Bancário ===\n\n";
            std::cout << "1. Ver histórico de empréstimos\n";
            std::cout << "2. Funcionalidades Avançadas\n";
            std::cout << "3. Adicionar novo empréstimo\n";
            std::cout << "4. Consultar saldo\n";
            std::cout << "5. Depositar dinheiro\n";
            std::cout << "0. Sair\n";
            std::cout << "Escolha uma opção: ";

            int choice;
            std::cin >> choice;
            std::cin.ignore();

            DatabaseManager dbManager("database/bank.db");

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
                    checkBalance(dbManager);
                    break;
                case 5:
                    depositMoney(dbManager);
                    break;
                case 0:
                    std::cout << "\nSaindo...\n";
                    return 0;
                default:
                    std::cout << "\nOpção inválida!\n";
            }
        }

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Erro inesperado: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Erro desconhecido ocorreu.\n";
        return 1;
    }
}