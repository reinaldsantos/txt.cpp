#ifndef COMPANY_H
#define COMPANY_H

#include <string>

class Company {
private:
    std::string name;
    std::string location;
    std::string employeeName;
    double loanAmount;
    bool loanApproved;

public:
    Company(const std::string& name, const std::string& location, 
            const std::string& employeeName, double loanAmount);
    
    // Getters
    std::string getName() const { return name; }
    std::string getLocation() const { return location; }
    std::string getEmployeeName() const { return employeeName; }
    double getLoanAmount() const { return loanAmount; }
    bool isLoanApproved() const { return loanApproved; }
};

#endif // COMPANY_H 