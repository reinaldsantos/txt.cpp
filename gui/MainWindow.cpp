#include "MainWindow.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QDoubleValidator>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setupUI();
    createConnections();
    setWindowTitle("Sistema Bancário");
    resize(1000, 800);
}

void MainWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    
    // Painel esquerdo (Empresas)
    QVBoxLayout* leftPanel = new QVBoxLayout();
    
    QLabel* companyTitle = new QLabel("Lista de Empresas", this);
    companyTitle->setStyleSheet("font-size: 18px; font-weight: bold; margin: 10px;");
    leftPanel->addWidget(companyTitle);
    
    companyList = new CompanyListWidget(this);
    leftPanel->addWidget(companyList);
    
    QHBoxLayout* companyButtons = new QHBoxLayout();
    refreshButton = new QPushButton("Atualizar", this);
    depositButton = new QPushButton("Depositar", this);
    checkBalanceButton = new QPushButton("Verificar Saldo", this);
    
    companyButtons->addWidget(refreshButton);
    companyButtons->addWidget(depositButton);
    companyButtons->addWidget(checkBalanceButton);
    
    leftPanel->addLayout(companyButtons);
    
    // Painel direito (Tarefas)
    QVBoxLayout* rightPanel = new QVBoxLayout();
    
    QLabel* taskTitle = new QLabel("Lista de Tarefas", this);
    taskTitle->setStyleSheet("font-size: 18px; font-weight: bold; margin: 10px;");
    rightPanel->addWidget(taskTitle);
    
    taskList = new TaskListWidget(this);
    rightPanel->addWidget(taskList);
    
    QHBoxLayout* taskButtons = new QHBoxLayout();
    addTaskButton = new QPushButton("Nova Tarefa", this);
    deleteTaskButton = new QPushButton("Excluir Tarefa", this);
    toggleTaskButton = new QPushButton("Alternar Status", this);
    
    taskButtons->addWidget(addTaskButton);
    taskButtons->addWidget(deleteTaskButton);
    taskButtons->addWidget(toggleTaskButton);
    
    rightPanel->addLayout(taskButtons);
    
    // Adiciona os painéis ao layout principal
    mainLayout->addLayout(leftPanel, 1);
    mainLayout->addLayout(rightPanel, 1);
}

void MainWindow::createConnections() {
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::onRefreshClicked);
    connect(depositButton, &QPushButton::clicked, this, &MainWindow::onDepositClicked);
    connect(checkBalanceButton, &QPushButton::clicked, this, &MainWindow::onCheckBalanceClicked);
    connect(addTaskButton, &QPushButton::clicked, this, &MainWindow::onAddTaskClicked);
    connect(deleteTaskButton, &QPushButton::clicked, this, &MainWindow::onDeleteTaskClicked);
    connect(toggleTaskButton, &QPushButton::clicked, this, &MainWindow::onToggleTaskClicked);
    connect(companyList, &CompanyListWidget::itemSelectionChanged, this, &MainWindow::onCompanySelected);
}

void MainWindow::onRefreshClicked() {
    companyList->refreshCompanies();
}

void MainWindow::onDepositClicked() {
    auto selectedCompanies = companyList->getSelectedCompanies();
    if (selectedCompanies.empty()) {
        QMessageBox::warning(this, "Aviso", "Selecione pelo menos uma empresa para depositar.");
        return;
    }
    
    bool ok;
    double amount = QInputDialog::getDouble(this, "Depósito",
                                          "Valor a depositar:",
                                          0.0, 0.0, 1000000.0, 2, &ok);
    if (!ok) return;
    
    DatabaseManager dbManager("database/bank.db");
    for (const auto& company : selectedCompanies) {
        if (dbManager.updateCompanyBalance(company.getNIPC(), amount)) {
            QMessageBox::information(this, "Sucesso",
                QString("Depósito realizado com sucesso para %1!")
                .arg(QString::fromStdString(company.getName())));
        } else {
            QMessageBox::warning(this, "Erro",
                QString("Erro ao realizar depósito para %1!")
                .arg(QString::fromStdString(company.getName())));
        }
    }
    
    companyList->refreshCompanies();
}

void MainWindow::onCheckBalanceClicked() {
    auto selectedCompanies = companyList->getSelectedCompanies();
    if (selectedCompanies.empty()) {
        QMessageBox::warning(this, "Aviso", "Selecione pelo menos uma empresa para verificar o saldo.");
        return;
    }
    
    DatabaseManager dbManager("database/bank.db");
    QString message;
    for (const auto& company : selectedCompanies) {
        double balance = dbManager.getCompanyBalance(company.getNIPC());
        message += QString("%1 - Saldo: R$ %2\n")
            .arg(QString::fromStdString(company.getName()))
            .arg(balance, 0, 'f', 2);
    }
    
    QMessageBox::information(this, "Saldos", message);
}

void MainWindow::onAddTaskClicked() {
    auto selectedCompanies = companyList->getSelectedCompanies();
    if (selectedCompanies.empty()) {
        QMessageBox::warning(this, "Aviso", "Selecione uma empresa para adicionar a tarefa.");
        return;
    }
    
    bool ok;
    QString description = QInputDialog::getText(this, "Nova Tarefa",
                                              "Descrição da tarefa:",
                                              QLineEdit::Normal,
                                              "", &ok);
    if (!ok || description.isEmpty()) return;
    
    taskList->addNewTask(description.toStdString(), selectedCompanies[0].getNIPC());
}

void MainWindow::onDeleteTaskClicked() {
    auto selectedTasks = taskList->getSelectedTasks();
    if (selectedTasks.empty()) {
        QMessageBox::warning(this, "Aviso", "Selecione pelo menos uma tarefa para excluir.");
        return;
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirmar Exclusão",
        "Tem certeza que deseja excluir as tarefas selecionadas?",
        QMessageBox::Yes | QMessageBox::No);
        
    if (reply == QMessageBox::Yes) {
        taskList->deleteSelectedTasks();
    }
}

void MainWindow::onToggleTaskClicked() {
    auto selectedTasks = taskList->getSelectedTasks();
    if (selectedTasks.empty()) {
        QMessageBox::warning(this, "Aviso", "Selecione pelo menos uma tarefa para alterar o status.");
        return;
    }
    
    taskList->toggleSelectedTasks();
}

void MainWindow::onCompanySelected() {
    auto selectedCompanies = companyList->getSelectedCompanies();
    if (!selectedCompanies.empty()) {
        taskList->refreshTasks(selectedCompanies[0].getNIPC());
    } else {
        taskList->refreshTasks();
    }
} 