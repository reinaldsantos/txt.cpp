#include "TaskListWidget.h"
#include <QString>
#include <QListWidgetItem>
#include <ctime>

TaskListWidget::TaskListWidget(QWidget* parent)
    : QListWidget(parent), dbManager("database/bank.db") {
    setupUI();
    refreshTasks();
}

void TaskListWidget::setupUI() {
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setAlternatingRowColors(true);
}

void TaskListWidget::refreshTasks(const std::string& companyNipc) {
    clear();
    std::vector<Task> tasks;
    
    if (companyNipc.empty()) {
        tasks = dbManager.getAllTasks();
    } else {
        tasks = dbManager.getCompanyTasks(companyNipc);
    }
    
    for (const auto& task : tasks) {
        addTaskToList(task);
    }
}

void TaskListWidget::addTaskToList(const Task& task) {
    QString status = task.isCompleted() ? "✓ Concluída" : "○ Pendente";
    QString date = task.isCompleted() ? 
        QDateTime::fromSecsSinceEpoch(task.getCompletedAt()).toString("dd/MM/yyyy hh:mm") :
        QDateTime::fromSecsSinceEpoch(task.getCreatedAt()).toString("dd/MM/yyyy hh:mm");
    
    QString displayText = QString("%1 - %2 (%3)")
        .arg(status)
        .arg(QString::fromStdString(task.getDescription()))
        .arg(date);

    QListWidgetItem* item = new QListWidgetItem(displayText);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(task.isCompleted() ? Qt::Checked : Qt::Unchecked);
    
    // Armazena o ID da tarefa como dados do item para referência
    item->setData(Qt::UserRole, task.getId());
    
    addItem(item);
}

std::vector<Task> TaskListWidget::getSelectedTasks() const {
    std::vector<Task> selectedTasks;
    for (int i = 0; i < count(); ++i) {
        QListWidgetItem* item = this->item(i);
        if (item->checkState() == Qt::Checked) {
            int taskId = item->data(Qt::UserRole).toInt();
            // Busca a tarefa no banco de dados
            auto allTasks = dbManager.getAllTasks();
            for (const auto& task : allTasks) {
                if (task.getId() == taskId) {
                    selectedTasks.push_back(task);
                    break;
                }
            }
        }
    }
    return selectedTasks;
}

void TaskListWidget::addNewTask(const std::string& description, const std::string& companyNipc) {
    Task task(description, companyNipc);
    if (dbManager.createTask(task)) {
        refreshTasks(companyNipc);
    }
}

void TaskListWidget::deleteSelectedTasks() {
    auto selectedTasks = getSelectedTasks();
    for (const auto& task : selectedTasks) {
        dbManager.deleteTask(task.getId());
    }
    refreshTasks();
}

void TaskListWidget::toggleSelectedTasks() {
    auto selectedTasks = getSelectedTasks();
    for (const auto& task : selectedTasks) {
        dbManager.updateTaskStatus(task.getId(), !task.isCompleted());
    }
    refreshTasks();
} 