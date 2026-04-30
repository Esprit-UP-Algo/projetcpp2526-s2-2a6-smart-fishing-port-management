#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gestionquai.h"
#include <QTableWidget>
#include <QLabel>
#include <QSystemTrayIcon>

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

    enum Page {
        LoginPage = 0,
        EmployeesPage = 1,
        CapturesPage = 2,
        ShipsPage = 3,
        QuaisPage = 4
    };

    void showLoginPage();
    void showEmployeesPage();
    void showCapturesPage();
    void showShipsPage();
    void showQuaisPage();

private slots:
    void onLoginClicked();
    void onForgotPasswordClicked();

    void onLogoutClicked();
    void onAddEmployeeClicked();
    void onEditEmployeeClicked(int row);
    void onDeleteEmployeeClicked(int row);
    void onSearchEmployees(const QString &text);
    void onShowAnalytics();

    void onSortByPositionClicked();
    void onSortBySalaryClicked();

    // Capture Slots
    void onAddCaptureClicked();
    void onOpenAddCaptureWithExportClicked();
    void onEditCaptureClicked(int row);
    void onDeleteCaptureClicked(int row);
    void onViewCaptureClicked(int row);
    void onSearchCaptures(const QString &text);
    void onSortCapturesByDate();
    void onShowCaptureStats();
    void onDetectAnomalies(); // Advanced 1
    void onComparePeriods();  // Advanced 2
    void onShipDetectionClicked(); // Ship Detection & Barrier Control (Arduino Sync)
    void onRecommendFishingZones(); // Fishing Zone Recommendation
    void onExportCapturesPdf();

    // Ship Slots
    void onAddShipClicked();
    void onEditShipClicked(int row);
    void onDeleteShipClicked(int row);
    void onSearchShips(const QString &text);
    void onSortShipsByCapacity();
    void onSortShipsByStatus();
    void onShowShipStats();
    void onGenerateShipPdf(); // Advanced Doc
    void onShowShipHistory(); // Advanced 1
    void onCheckShipAvailability(); // Advanced 2 (Notification)

    // Quai Slots
    void onAddQuaiClicked();
    void onEditQuaiClicked();
    void onDeleteQuaiClicked();

private:
    void setupConnections();
    void setupTableEmployees();
    void updateEmployeeStats();
    void updateSalaryStats();
    void generateAnalyticsPDF();
    void generateShipAnalyticsPDF();
    
    // Capture Helpers
    void setupCapturesUi();
    void setupTableCaptures();
    void updateCaptureStats();
    void generateRandomCaptures();
    void generateCaptureAnalyticsPDF();
    QWidget* createSidebar(QWidget* parent);

    // Quai Helpers
    void setupQuaisUi();

    QString analyticsStatsText;
    void* analyticsPieChart = nullptr;
    void* analyticsBarChart = nullptr;
    
    // Capture UI Elements
    QTableWidget *tableCaptures = nullptr;
    QLabel *statDiversity = nullptr;
    QLabel *statTotalQuantity = nullptr;
    QLabel *statCaptureAverage = nullptr;
    QLabel *statCaptureMax = nullptr;
    QLabel *statCaptureNavires = nullptr;

    // Ship Helpers & UI
    void setupShipsUi();
    void setupTableShips();
    void updateShipStats();
    QTableWidget *tableShips = nullptr;

    // Quai UI Elements
    GestionQuai *gestionQuai = nullptr;

    Ui::MainWindow *ui;
};
#endif
