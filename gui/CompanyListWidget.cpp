#include "CompanyListWidget.h"
#include <QString>
#include <QListWidgetItem>

CompanyListWidget::CompanyListWidget(QWidget* parent)
    : QListWidget(parent), dbManager("database/bank.db") {
    setupUI();
    refreshCompanies();
}

void CompanyListWidget::setupUI() {
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setAlternatingRowColors(true);
}

void CompanyListWidget::refreshCompanies() {
    clear();
    auto companies = dbManager.getAllCompanies();
    for (const auto& company : companies) {
        addCompanyToList(company);
    }
}

void CompanyListWidget::addCompanyToList(const Company& company) {
    QString displayText = QString("%1 - NIPC: %2 - Saldo: R$ %3")
        .arg(QString::fromStdString(company.getName()))
        .arg(QString::fromStdString(company.getNIPC()))
        .arg(company.getBalance(), 0, 'f', 2);

    QListWidgetItem* item = new QListWidgetItem(displayText);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Unchecked);
    
    // Armazena o NIPC como dados do item para referência
    item->setData(Qt::UserRole, QString::fromStdString(company.getNIPC()));
    
    addItem(item);
}

std::vector<Company> CompanyListWidget::getSelectedCompanies() const {
    std::vector<Company> selectedCompanies;
    for (int i = 0; i < count(); ++i) {
        QListWidgetItem* item = this->item(i);
        if (item->checkState() == Qt::Checked) {
            QString nipc = item->data(Qt::UserRole).toString();
            Company company = dbManager.getCompany(nipc.toStdString());
            selectedCompanies.push_back(company);
        }
    }
    return selectedCompanies;
} 