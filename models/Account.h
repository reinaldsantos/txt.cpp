#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>

class Account {
private:
    int id;
    std::string accountNumber;
    double balance;
    std::string createdAt;

public:
    Account();
    Account(const std::string& accountNumber, double initialBalance);
    
    // Getters
    int getId() const;
    std::string getAccountNumber() const;
    double getBalance() const;
    std::string getCreatedAt() const;
    
    // Setters
    void setId(int id);
    void setAccountNumber(const std::string& accountNumber);
    void setBalance(double balance);
    void setCreatedAt(const std::string& createdAt);
    
    // Operations
    bool deposit(double amount);
    bool withdraw(double amount);
};

#endif // ACCOUNT_H 