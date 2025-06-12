#include "Company.h"

Company::Company(const std::string& name, const std::string& nipc, const std::string& location, 
                const std::string& employeeName, double loanAmount)
    : name(name), nipc(nipc), location(location), employeeName(employeeName), 
      loanAmount(loanAmount), loanApproved(loanAmount <= 100000.0), balance(0.0) {
} 