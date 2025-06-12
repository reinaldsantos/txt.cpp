#ifndef TASK_LIST_WIDGET_H
#define TASK_LIST_WIDGET_H

#include <QListWidget>
#include <QListWidgetItem>
#include <QString>
#include <vector>
#include "../models/Task.h"
#include "../database/DatabaseManager.h"

class TaskListWidget : public QListWidget {
    Q_OBJECT

public:
    explicit TaskListWidget(QWidget* parent = nullptr);
    void refreshTasks(const std::string& companyNipc = "");
    std::vector<Task> getSelectedTasks() const;
    void addNewTask(const std::string& description, const std::string& companyNipc);
    void deleteSelectedTasks();
    void toggleSelectedTasks();

private:
    DatabaseManager dbManager;
    void setupUI();
    void addTaskToList(const Task& task);
};

#endif // TASK_LIST_WIDGET_H 