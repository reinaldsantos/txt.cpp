#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <ctime>
#include "models/Task.h"
#include "database/DatabaseManager.h"
#include "task_list.h"

void printHeader() {
    std::cout << "\n=== Gerenciamento de Tarefas ===\n\n";
}

void printMenu() {
    std::cout << "1. Listar todas as tarefas\n";
    std::cout << "2. Listar tarefas de uma empresa\n";
    std::cout << "3. Adicionar nova tarefa\n";
    std::cout << "4. Marcar tarefa como concluída\n";
    std::cout << "5. Excluir tarefa\n";
    std::cout << "0. Voltar ao menu principal\n\n";
    std::cout << "Escolha uma opção: ";
}

void printTask(const Task& task) {
    std::cout << std::setw(4) << task.getId() << " | ";
    std::cout << std::setw(10) << (task.isCompleted() ? "Concluída" : "Pendente") << " | ";
    std::cout << std::setw(40) << task.getDescription() << " | ";
    
    // Converte timestamp para string legível
    char timeStr[20];
    time_t taskTime = task.isCompleted() ? task.getCompletedAt() : task.getCreatedAt();
    std::tm* tm = std::localtime(&taskTime);
    std::strftime(timeStr, sizeof(timeStr), "%d/%m/%Y %H:%M", tm);
    
    std::cout << timeStr << " | ";
    std::cout << task.getCompanyNipc() << "\n";
}

void listAllTasks(DatabaseManager& dbManager) {
    std::cout << "\n=== Lista de Todas as Tarefas ===\n\n";
    std::cout << std::setw(4) << "ID" << " | ";
    std::cout << std::setw(10) << "Status" << " | ";
    std::cout << std::setw(40) << "Descrição" << " | ";
    std::cout << std::setw(16) << "Data" << " | ";
    std::cout << "NIPC\n";
    std::cout << std::string(80, '-') << "\n";
    
    auto tasks = dbManager.getAllTasks();
    for (const auto& task : tasks) {
        printTask(task);
    }
    std::cout << "\n";
}

void listCompanyTasks(DatabaseManager& dbManager) {
    std::string nipc;
    std::cout << "\nDigite o NIPC da empresa: ";
    std::cin.ignore();
    std::getline(std::cin, nipc);
    
    std::cout << "\n=== Tarefas da Empresa " << nipc << " ===\n\n";
    std::cout << std::setw(4) << "ID" << " | ";
    std::cout << std::setw(10) << "Status" << " | ";
    std::cout << std::setw(40) << "Descrição" << " | ";
    std::cout << std::setw(16) << "Data" << " | ";
    std::cout << "NIPC\n";
    std::cout << std::string(80, '-') << "\n";
    
    auto tasks = dbManager.getCompanyTasks(nipc);
    for (const auto& task : tasks) {
        printTask(task);
    }
    std::cout << "\n";
}

void addNewTask(DatabaseManager& dbManager) {
    std::string description, nipc;
    
    std::cout << "\nDigite o NIPC da empresa: ";
    std::cin.ignore();
    std::getline(std::cin, nipc);
    
    std::cout << "Digite a descrição da tarefa: ";
    std::getline(std::cin, description);
    
    Task task(description, nipc);
    if (dbManager.createTask(task)) {
        std::cout << "\nTarefa adicionada com sucesso!\n";
    } else {
        std::cout << "\nErro ao adicionar tarefa.\n";
    }
}

void toggleTaskStatus(DatabaseManager& dbManager) {
    int taskId;
    std::cout << "\nDigite o ID da tarefa: ";
    std::cin >> taskId;
    
    auto tasks = dbManager.getAllTasks();
    for (const auto& task : tasks) {
        if (task.getId() == taskId) {
            if (dbManager.updateTaskStatus(taskId, !task.isCompleted())) {
                std::cout << "\nStatus da tarefa atualizado com sucesso!\n";
            } else {
                std::cout << "\nErro ao atualizar status da tarefa.\n";
            }
            return;
        }
    }
    std::cout << "\nTarefa não encontrada.\n";
}

void deleteTask(DatabaseManager& dbManager) {
    int taskId;
    std::cout << "\nDigite o ID da tarefa: ";
    std::cin >> taskId;
    
    if (dbManager.deleteTask(taskId)) {
        std::cout << "\nTarefa excluída com sucesso!\n";
    } else {
        std::cout << "\nErro ao excluir tarefa.\n";
    }
}

void taskManagement() {
    DatabaseManager dbManager("database/bank.db");
    int choice;
    
    do {
        printHeader();
        printMenu();
        std::cin >> choice;
        
        switch (choice) {
            case 1:
                listAllTasks(dbManager);
                break;
            case 2:
                listCompanyTasks(dbManager);
                break;
            case 3:
                addNewTask(dbManager);
                break;
            case 4:
                toggleTaskStatus(dbManager);
                break;
            case 5:
                deleteTask(dbManager);
                break;
            case 0:
                return;
            default:
                std::cout << "\nOpção inválida!\n";
        }
        
        std::cout << "\nPressione Enter para continuar...";
        std::cin.ignore();
        std::cin.get();
        
    } while (true);
} 