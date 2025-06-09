#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>

class Transaction {
private:
    int id;
    int accountId;
    double amount;
    std::string type;
    std::string description;
    std::string createdAt;

public:
    Transaction();
    Transaction(int accountId, double amount, const std::string& type, const std::string& description = "");
    
    // Getters
    int getId() const;
    int getAccountId() const;
    double getAmount() const;
    std::string getType() const;
    std::string getDescription() const;
    std::string getCreatedAt() const;
    
    // Setters
    void setId(int id);
    void setAccountId(int accountId);
    void setAmount(double amount);
    void setType(const std::string& type);
    void setDescription(const std::string& description);
    void setCreatedAt(const std::string& createdAt);
};

#endif // TRANSACTION_H 