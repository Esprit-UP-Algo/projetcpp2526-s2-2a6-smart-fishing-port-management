#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QStackedLayout>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

class GestionQuai;
class GestionNavires;
class GestionCaptures;
class GestionUtilisateurs;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLoginClicked();
    void onLogoutClicked();
    void showGestionQuais();
    void showGestionNavires();
    void showGestionCaptures();
    void showGestionUtilisateurs();

private:
    void setupLoginPage();
    void setupMainInterface();

    QStackedLayout *stackedLayout;
    QWidget *loginPage;
    QWidget *mainPage;

    QWidget *menuWidget;
    QPushButton *btnQuais;
    QPushButton *btnNavires;
    QPushButton *btnCaptures;
    QPushButton *btnUtilisateurs;
    QPushButton *btnLogout;

    QStackedWidget *contentStack;

    GestionQuai *gestionQuaiWidget;
    GestionNavires *gestionNaviresWidget;
    GestionCaptures *gestionCapturesWidget;
    GestionUtilisateurs *gestionUtilisateursWidget;

    QLineEdit *userEdit;
    QLineEdit *passEdit;
};

#endif
