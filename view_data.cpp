/**
 * @file view_data.cpp
 * @brief Programa para visualização do histórico de empréstimos
 */

#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <vector>
#include <fstream>
#ifdef _WIN32
#include <windows.h>
#endif
#include "database/DatabaseManager.h"
#include "models/Company.h"
#include "advanced_features.cpp"

// Função para configurar o console para UTF-8
bool setupConsole() {
    #ifdef _WIN32
        return SetConsoleOutputCP(CP_UTF8) != 0;
    #else
        return true;
    #endif
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

int main() {
    try {
        // Configura o console para UTF-8
        if (!setupConsole()) {
            std::cerr << "Erro ao configurar o console para UTF-8.\n";
            return 1;
        }

        while (true) {
            std::cout << "\n=== Sistema Bancário ===\n\n";
            std::cout << "1. Ver histórico de empréstimos\n";
            std::cout << "2. Ver log de atividades\n";
            std::cout << "3. Funcionalidades Avançadas\n";
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
                    displayLog();
                    break;
                case 3:
                    showAdvancedMenu();
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