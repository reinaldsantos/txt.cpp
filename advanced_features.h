#ifndef ADVANCED_FEATURES_H
#define ADVANCED_FEATURES_H

#include <vector>
#include <string>
#include "models/Company.h"

struct CreditAnalysis {
    int score;
    std::string riskLevel;
    std::string recommendation;
};

struct Installment {
    int number;
    double amount;
    double interest;
    double total;
};

CreditAnalysis calculateCreditScore(const Company& company, const std::vector<Company>& allCompanies);
std::vector<Installment> calculateInstallments(double amount, double interestRate, int months);
void displayCreditAnalysis(const CreditAnalysis& analysis);
void displayInstallments(const std::vector<Installment>& installments);
void analyzeTrends(const std::vector<Company>& companies);
void simulateLoan();
void showAdvancedMenu();

#endif // ADVANCED_FEATURES_H 