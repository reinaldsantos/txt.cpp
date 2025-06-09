#include "Account.h"

Account::Account() : id(0), balance(0.0) {}

Account::Account(const std::string& accountNumber, double initialBalance)
    : id(0), accountNumber(accountNumber), balance(initialBalance) {}

int Account::getId() const {
    return id;
}

std::string Account::getAccountNumber() const {
    return accountNumber;
}

double Account::getBalance() const {
    return balance;
}

std::string Account::getCreatedAt() const {
    return createdAt;
}

void Account::setId(int id) {
    this->id = id;
}

void Account::setAccountNumber(const std::string& accountNumber) {
    this->accountNumber = accountNumber;
}

void Account::setBalance(double balance) {
    this->balance = balance;
}

void Account::setCreatedAt(const std::string& createdAt) {
    this->createdAt = createdAt;
}

bool Account::deposit(double amount) {
    if (amount <= 0) {
        return false;
    }
    balance += amount;
    return true;
}

bool Account::withdraw(double amount) {
    if (amount <= 0 || amount > balance) {
        return false;
    }
    balance -= amount;
    return true;
} 