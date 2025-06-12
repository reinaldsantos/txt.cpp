#ifndef TASK_H
#define TASK_H

#include <string>
#include <ctime>

class Task {
private:
    int id;
    std::string description;
    bool completed;
    std::string companyNipc;
    time_t createdAt;
    time_t completedAt;

public:
    Task(const std::string& description, const std::string& companyNipc);
    Task(int id, const std::string& description, bool completed, 
         const std::string& companyNipc, time_t createdAt, time_t completedAt);
    
    // Getters
    int getId() const { return id; }
    std::string getDescription() const { return description; }
    bool isCompleted() const { return completed; }
    std::string getCompanyNipc() const { return companyNipc; }
    time_t getCreatedAt() const { return createdAt; }
    time_t getCompletedAt() const { return completedAt; }
    
    // Setters
    void setCompleted(bool status) { 
        completed = status;
        if (status) {
            completedAt = time(nullptr);
        } else {
            completedAt = 0;
        }
    }
    void setId(int newId) { id = newId; }
};

#endif // TASK_H 