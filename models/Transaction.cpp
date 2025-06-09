#include "Transaction.h"

Transaction::Transaction() : id(0), accountId(0), amount(0.0) {}

Transaction::Transaction(int accountId, double amount, const std::string& type, const std::string& description)
    : id(0), accountId(accountId), amount(amount), type(type), description(description) {}

int Transaction::getId() const {
    return id;
}

int Transaction::getAccountId() const {
    return accountId;
}

double Transaction::getAmount() const {
    return amount;
}

std::string Transaction::getType() const {
    return type;
}

std::string Transaction::getDescription() const {
    return description;
}

std::string Transaction::getCreatedAt() const {
    return createdAt;
}

void Transaction::setId(int id) {
    this->id = id;
}

void Transaction::setAccountId(int accountId) {
    this->accountId = accountId;
}

void Transaction::setAmount(double amount) {
    this->amount = amount;
}

void Transaction::setType(const std::string& type) {
    this->type = type;
}

void Transaction::setDescription(const std::string& description) {
    this->description = description;
}

void Transaction::setCreatedAt(const std::string& createdAt) {
    this->createdAt = createdAt;
} 