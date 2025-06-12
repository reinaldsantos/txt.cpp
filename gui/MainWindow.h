#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QInputDialog>
#include "CompanyListWidget.h"
#include "TaskListWidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onRefreshClicked();
    void onDepositClicked();
    void onCheckBalanceClicked();
    void onAddTaskClicked();
    void onDeleteTaskClicked();
    void onToggleTaskClicked();
    void onCompanySelected();

private:
    CompanyListWidget* companyList;
    TaskListWidget* taskList;
    QPushButton* refreshButton;
    QPushButton* depositButton;
    QPushButton* checkBalanceButton;
    QPushButton* addTaskButton;
    QPushButton* deleteTaskButton;
    QPushButton* toggleTaskButton;
    
    void setupUI();
    void createConnections();
};

#endif // MAIN_WINDOW_H 