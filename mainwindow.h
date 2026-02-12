#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "addedit_employeedialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Page indices
    enum Page {
        LoginPage = 0,
        EmployeesPage = 1
    };

    // Screen switching functions
    void showLoginPage();
    void showEmployeesPage();

private slots:
    // Login page slots
    void onLoginClicked();
    void onForgotPasswordClicked();

    // Employees page slots
    void onLogoutClicked();
    void onAddEmployeeClicked();
    void onEditEmployeeClicked(int row);
    void onDeleteEmployeeClicked(int row);
    void onSearchEmployees(const QString &text);
    void onShowAnalytics();

    // Sorting slots
    void onSortByPositionClicked();
    void onSortBySalaryClicked();

private:
    void setupConnections();
    void setupTableEmployees();
    void updateEmployeeStats();
    void updateSalaryStats();
    void generateAnalyticsPDF();

    // Analytics data storage for PDF generation
    QString analyticsStatsText;
    void* analyticsPieChart = nullptr;     // QChart* - stored as void* to avoid header includes
    void* analyticsBarChart = nullptr;     // QChart* - stored as void* to avoid header includes

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
