#ifndef COMPANY_H
#define COMPANY_H

#include <string>

class Company {
private:
    std::string name;
    std::string nipc;
    std::string location;
    std::string employeeName;
    double loanAmount;
    bool loanApproved;
    double balance;

public:
    Company() : name(""), nipc(""), location(""), employeeName(""), loanAmount(0.0), loanApproved(true), balance(0.0) {}
    
    Company(const std::string& name, const std::string& nipc, const std::string& location, 
            const std::string& employeeName, double loanAmount);
    
    // Getters
    std::string getName() const { return name; }
    std::string getNIPC() const { return nipc; }
    std::string getLocation() const { return location; }
    std::string getEmployeeName() const { return employeeName; }
    double getLoanAmount() const { return loanAmount; }
    bool isLoanApproved() const { return loanApproved; }
    double getBalance() const { return balance; }
    
    void setBalance(double newBalance) { balance = newBalance; }
};

#endif // COMPANY_H 