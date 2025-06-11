#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <map>
#include <ctime>
#include "models/Company.h"
#include "database/DatabaseManager.h"

// Estrutura para análise de crédito
struct CreditAnalysis {
    double creditScore;
    double interestRate;
    double maxLoanAmount;
    std::string riskLevel;
};

// Estrutura para parcelas
struct Installment {
    int number;
    double value;
    double principal;
    double interest;
    double remainingBalance;
};

// Função para calcular score de crédito
CreditAnalysis calculateCreditScore(const Company& company, const std::vector<Company>& history) {
    CreditAnalysis analysis;
    double baseScore = 700.0; // Score base
    
    // Análise do valor do empréstimo
    if (company.getLoanAmount() > 10000) {
        baseScore -= 50;
    } else if (company.getLoanAmount() < 5000) {
        baseScore += 30;
    }
    
    // Análise de histórico
    int previousLoans = 0;
    int paidLoans = 0;
    for (const auto& hist : history) {
        if (hist.getName() == company.getName()) {
            previousLoans++;
            if (hist.isLoanApproved()) {
                paidLoans++;
            }
        }
    }
    
    if (previousLoans > 0) {
        double paymentRate = (double)paidLoans / previousLoans;
        baseScore += paymentRate * 100;
    }
    
    // Definir taxa de juros baseada no score
    analysis.creditScore = baseScore;
    if (baseScore >= 800) {
        analysis.interestRate = 0.05; // 5%
        analysis.riskLevel = "BAIXO";
        analysis.maxLoanAmount = company.getLoanAmount() * 2;
    } else if (baseScore >= 700) {
        analysis.interestRate = 0.08; // 8%
        analysis.riskLevel = "MÉDIO";
        analysis.maxLoanAmount = company.getLoanAmount() * 1.5;
    } else {
        analysis.interestRate = 0.12; // 12%
        analysis.riskLevel = "ALTO";
        analysis.maxLoanAmount = company.getLoanAmount();
    }
    
    return analysis;
}

// Função para calcular parcelas
std::vector<Installment> calculateInstallments(double amount, double interestRate, int months) {
    std::vector<Installment> installments;
    double monthlyRate = interestRate / 12;
    double payment = amount * (monthlyRate * pow(1 + monthlyRate, months)) / (pow(1 + monthlyRate, months) - 1);
    double remainingBalance = amount;
    
    for (int i = 1; i <= months; i++) {
        Installment inst;
        inst.number = i;
        inst.value = payment;
        inst.interest = remainingBalance * monthlyRate;
        inst.principal = payment - inst.interest;
        remainingBalance -= inst.principal;
        inst.remainingBalance = remainingBalance;
        installments.push_back(inst);
    }
    
    return installments;
}

// Função para mostrar análise de crédito
void displayCreditAnalysis(const CreditAnalysis& analysis) {
    std::cout << "\n=== Análise de Crédito ===\n";
    std::cout << "Score de Crédito: " << std::fixed << std::setprecision(0) << analysis.creditScore << "\n";
    std::cout << "Nível de Risco: " << analysis.riskLevel << "\n";
    std::cout << "Taxa de Juros: " << std::fixed << std::setprecision(2) << (analysis.interestRate * 100) << "%\n";
    std::cout << "Valor Máximo de Empréstimo: €" << std::fixed << std::setprecision(2) << analysis.maxLoanAmount << "\n";
}

// Função para mostrar tabela de parcelas
void displayInstallments(const std::vector<Installment>& installments) {
    std::cout << "\n=== Tabela de Parcelas ===\n";
    std::cout << std::left
              << std::setw(8) << "Parcela"
              << std::setw(12) << "Valor"
              << std::setw(12) << "Principal"
              << std::setw(12) << "Juros"
              << std::setw(15) << "Saldo Restante"
              << "\n";
    std::cout << std::string(60, '-') << "\n";
    
    for (const auto& inst : installments) {
        std::cout << std::left
                  << std::setw(8) << inst.number
                  << std::fixed << std::setprecision(2)
                  << std::setw(12) << inst.value
                  << std::setw(12) << inst.principal
                  << std::setw(12) << inst.interest
                  << std::setw(15) << inst.remainingBalance
                  << "\n";
    }
}

// Função para análise de tendências
void analyzeTrends(const std::vector<Company>& companies) {
    std::cout << "\n=== Análise de Tendências ===\n";
    
    // Análise por localização
    std::map<std::string, int> locationCount;
    std::map<std::string, double> locationTotal;
    
    for (const auto& company : companies) {
        locationCount[company.getLocation()]++;
        locationTotal[company.getLocation()] += company.getLoanAmount();
    }
    
    std::cout << "\nEmpréstimos por Localização:\n";
    for (const auto& loc : locationCount) {
        std::cout << loc.first << ": " << loc.second << " empréstimos"
                  << " (Total: €" << std::fixed << std::setprecision(2) 
                  << locationTotal[loc.first] << ")\n";
    }
    
    // Análise de valores
    double totalAmount = 0;
    double maxAmount = 0;
    double minAmount = std::numeric_limits<double>::max();
    
    for (const auto& company : companies) {
        totalAmount += company.getLoanAmount();
        maxAmount = std::max(maxAmount, company.getLoanAmount());
        minAmount = std::min(minAmount, company.getLoanAmount());
    }
    
    std::cout << "\nEstatísticas de Valores:\n";
    std::cout << "Média: €" << std::fixed << std::setprecision(2) 
              << (totalAmount / companies.size()) << "\n";
    std::cout << "Maior: €" << maxAmount << "\n";
    std::cout << "Menor: €" << minAmount << "\n";
}

// Função para simulação de empréstimo
void simulateLoan() {
    double amount;
    int months;
    
    std::cout << "\n=== Simulação de Empréstimo ===\n";
    std::cout << "Valor do empréstimo: €";
    std::cin >> amount;
    std::cout << "Número de parcelas: ";
    std::cin >> months;
    
    // Simulação com diferentes taxas
    std::cout << "\nSimulação com diferentes taxas de juros:\n";
    std::vector<double> rates = {0.05, 0.08, 0.12}; // 5%, 8%, 12%
    
    for (double rate : rates) {
        std::cout << "\nTaxa de " << (rate * 100) << "% ao ano:\n";
        auto installments = calculateInstallments(amount, rate, months);
        displayInstallments(installments);
    }
}

// Função para menu de funcionalidades avançadas
void showAdvancedMenu() {
    while (true) {
        std::cout << "\n=== Funcionalidades Avançadas ===\n";
        std::cout << "1. Análise de Crédito\n";
        std::cout << "2. Simulação de Empréstimo\n";
        std::cout << "3. Análise de Tendências\n";
        std::cout << "0. Voltar ao Menu Principal\n";
        std::cout << "Escolha uma opção: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1: {
                std::cout << "\n=== Análise de Crédito ===\n\n";
                
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
                
                std::cout << "Valor do empréstimo desejado: ";
                while (!(std::cin >> amount) || amount <= 0) {
                    std::cout << "Valor inválido. Digite novamente: ";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
                std::cin.ignore();

                // Cria uma empresa temporária para análise
                Company tempCompany(name, cnpj, location, employeeName, amount);
                
                // Obtém todas as empresas do banco para análise comparativa
                DatabaseManager dbManager("database/bank.db");
                auto allCompanies = dbManager.getAllCompanies();
                
                // Realiza a análise de crédito
                CreditAnalysis analysis = calculateCreditScore(tempCompany, allCompanies);
                
                // Exibe o resultado da análise
                displayCreditAnalysis(analysis);
                break;
            }
            case 2:
                simulateLoan();
                break;
            case 3: {
                DatabaseManager dbManager("database/bank.db");
                auto companies = dbManager.getAllCompanies();
                analyzeTrends(companies);
                break;
            }
            case 0:
                return;
            default:
                std::cout << "\nOpção inválida!\n";
        }
    }
} 