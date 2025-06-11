#include "Company.h"

Company::Company(const std::string& name, const std::string& cnpj, const std::string& location, 
                const std::string& employeeName, double loanAmount)
    : name(name), cnpj(cnpj), location(location), employeeName(employeeName), 
      loanAmount(loanAmount), loanApproved(loanAmount <= 100000.0), balance(0.0) {
} 