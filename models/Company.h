#ifndef COMPANY_H
#define COMPANY_H

#include <string>

class Company {
private:
    std::string name;
    std::string cnpj;
    std::string location;
    std::string employeeName;
    double loanAmount;
    bool loanApproved;
    double balance;

public:
    Company(const std::string& name, const std::string& cnpj, const std::string& location, 
            const std::string& employeeName, double loanAmount);
    
    // Getters
    std::string getName() const { return name; }
    std::string getCNPJ() const { return cnpj; }
    std::string getLocation() const { return location; }
    std::string getEmployeeName() const { return employeeName; }
    double getLoanAmount() const { return loanAmount; }
    bool isLoanApproved() const { return loanApproved; }
    double getBalance() const { return balance; }
    
    void setBalance(double newBalance) { balance = newBalance; }
};

#endif // COMPANY_H 