#include "Task.h"
#include <ctime>

namespace {
    time_t getCurrentTime() {
        return time(nullptr);
    }
}

Task::Task(const std::string& description, const std::string& companyNipc) {
    this->id = 0;
    this->description = description;
    this->completed = false;
    this->companyNipc = companyNipc;
    this->createdAt = time(nullptr);
    this->completedAt = 0;
}

Task::Task(int id, const std::string& description, bool completed,
           const std::string& companyNipc, time_t createdAt, time_t completedAt) {
    this->id = id;
    this->description = description;
    this->completed = completed;
    this->companyNipc = companyNipc;
    this->createdAt = createdAt;
    this->completedAt = completedAt;
} 