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
#include <limits>
#include <fstream>
#include <ctime>
#ifdef _WIN32
#include <windows.h>
#endif
#include "database/DatabaseManager.h"
#include "models/Company.h"

// Função para obter a data e hora atual
std::string getCurrentDateTime() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%d/%m/%Y %H:%M:%S");
    return oss.str();
}

// Função para registrar no arquivo de log
void logActivity(const std::string& activity) {
    std::ofstream logFile("log.txt", std::ios::app);
    if (logFile.is_open()) {
        logFile << getCurrentDateTime() << " | " << activity << "\n";
        logFile.close();
    }
}

// Função para configurar o console para UTF-8
bool setupConsole() {
    #ifdef _WIN32
        return SetConsoleOutputCP(CP_UTF8) != 0;
    #else
        return true;
    #endif
}

// Função para limpar o buffer de entrada
void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Função para exibir o cabeçalho da tabela
void displayHeader() {
    std::cout << std::left
              << std::setw(20) << "Empresa"
              << std::setw(20) << "Local"
              << std::setw(20) << "Funcionário"
              << std::setw(15) << "Valor (€)"
              << std::setw(10) << "Status"
              << "\n";
    std::cout << std::string(85, '-') << "\n";
}

// Função para exibir os dados de uma empresa
void displayCompany(const Company& company) {
    std::cout << std::left
              << std::setw(20) << company.getName()
              << std::setw(20) << company.getLocation()
              << std::setw(20) << company.getEmployeeName()
              << std::fixed << std::setprecision(2)
              << std::setw(15) << company.getLoanAmount()
              << std::setw(10) << (company.isLoanApproved() ? "Aprovado" : "Reprovado")
              << "\n";
}

// Função para exibir o histórico de empréstimos
void displayLoanHistory(DatabaseManager& dbManager) {
    std::cout << "\n=== Histórico de Empréstimos ===\n\n";
    
    auto companies = dbManager.getAllCompanies();
    if (companies.empty()) {
        std::cout << "Nenhum registro encontrado.\n";
        return;
    }

    displayHeader();
    for (const auto& company : companies) {
        displayCompany(company);
    }
}

// Função para adicionar um novo empréstimo
void addNewLoan(DatabaseManager& dbManager) {
    std::cout << "\n=== Novo Empréstimo ===\n\n";
    
    std::string name, location, employeeName;
    double amount;
    
    std::cout << "Nome da empresa: ";
    std::getline(std::cin, name);
    
    std::cout << "Local: ";
    std::getline(std::cin, location);
    
    std::cout << "Nome do funcionário: ";
    std::getline(std::cin, employeeName);
    
    std::cout << "Valor do empréstimo (€): ";
    while (!(std::cin >> amount) || amount <= 0) {
        std::cout << "Valor inválido. Digite novamente: ";
        clearInputBuffer();
    }
    clearInputBuffer();

    // Cria uma nova empresa e salva no banco de dados
    Company newCompany(name, location, employeeName, amount);
    if (dbManager.createCompany(newCompany)) {
        std::cout << "\nEmpréstimo registrado com sucesso!\n";
        // Registra a atividade no log
        std::ostringstream logEntry;
        logEntry << name << " | " << location << " | " << employeeName 
                 << " | € " << std::fixed << std::setprecision(2) << amount 
                 << " | " << (newCompany.isLoanApproved() ? "Aprovado" : "Reprovado");
        logActivity(logEntry.str());
    } else {
        std::cout << "\nErro ao registrar empréstimo.\n";
    }
}

// Função para exibir o menu principal
void displayMenu() {
    std::cout << "\n=== Sistema de Empréstimos ===\n"
              << "1. Visualizar histórico\n"
              << "2. Adicionar empréstimo\n"
              << "0. Sair\n"
              << "Escolha uma opção: ";
}

int main() {
    try {
        // Configura o console para UTF-8
        if (!setupConsole()) {
            std::cerr << "Erro ao configurar o console para UTF-8.\n";
            return 1;
        }

        // Conecta ao banco de dados
        DatabaseManager dbManager("bank.db");
        
        int choice;
        do {
            displayMenu();
            while (!(std::cin >> choice)) {
                std::cout << "Opção inválida. Digite novamente: ";
                clearInputBuffer();
            }
            clearInputBuffer();

            switch (choice) {
                case 1:
                    displayLoanHistory(dbManager);
                    break;
                case 2:
                    addNewLoan(dbManager);
                    break;
                case 0:
                    std::cout << "\nSaindo do sistema...\n";
                    break;
                default:
                    std::cout << "\nOpção inválida!\n";
            }
        } while (choice != 0);

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Erro inesperado: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Erro desconhecido ocorreu.\n";
        return 1;
    }
}