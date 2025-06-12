#ifndef COMPANY_LIST_WIDGET_H
#define COMPANY_LIST_WIDGET_H

#include <QListWidget>
#include <QListWidgetItem>
#include <QString>
#include <vector>
#include "../models/Company.h"
#include "../database/DatabaseManager.h"

class CompanyListWidget : public QListWidget {
    Q_OBJECT

public:
    explicit CompanyListWidget(QWidget* parent = nullptr);
    void refreshCompanies();
    std::vector<Company> getSelectedCompanies() const;

private:
    DatabaseManager dbManager;
    void setupUI();
    void addCompanyToList(const Company& company);
};

#endif // COMPANY_LIST_WIDGET_H 