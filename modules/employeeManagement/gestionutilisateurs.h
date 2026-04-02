#ifndef GESTIONUTILISATEURS_H
#define GESTIONUTILISATEURS_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QStackedWidget>
#include <QList>
#include <QMessageBox>
#include <QString>
#include <QVariant>
#include <QDateTime>
#include <QRandomGenerator>
#include <QUuid>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDir>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QTextStream>
#include <QDesktopServices>
#include <QRegularExpression>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <limits>

#include "employe.h"
#include "connection.h"

class DialogEmploye : public QDialog
{
    Q_OBJECT

public:
    explicit DialogEmploye(QWidget *parent = nullptr, bool isEditMode = false, const QList<Employe>* existingEmployes = nullptr);
    ~DialogEmploye();

    QString getId() const;
    QString getNom() const;
    QString getPoste() const;
    QString getEmail() const;
    QString getTelephone() const;
    double getSalaire() const;
    QString getGenre() const;

    void setEmployeData(const QString &id, const QString &nom, const QString &poste,
                        const QString &email, const QString &telephone, double salaire = 0, const QString &genre = "M");

private slots:
    void onSaveClicked();
    void onCancelClicked();

private:
    bool validateInputs();
    bool isValidEmail(const QString &email) const;
    bool isValidPhone(const QString &phone) const;
    bool isValidNom(const QString &nom) const;
    bool isValidSalaire(double salaire) const;
    bool isEmailUnique(const QString &email) const;
    bool isPhoneUnique(const QString &phone) const;

    QLineEdit *lineId;
    QLineEdit *lineNom;
    QComboBox *comboPoste;
    QComboBox *comboGenre;
    QLineEdit *lineEmail;
    QLineEdit *lineTelephone;
    QDoubleSpinBox *spinSalaire;
    QPushButton *btnSave;
    QPushButton *btnCancel;

    bool isEditMode;
    const QList<Employe>* existingEmployes;
    QString originalEmail;
    QString originalPhone;
};

class GestionUtilisateurs : public QWidget
{
    Q_OBJECT

public:
    explicit GestionUtilisateurs(QWidget *parent = nullptr);
    ~GestionUtilisateurs();
    void loadFromDb();
    struct UsersStats { int total; int pecheur; int matelot; int docker; int chefQuai; int superviseur; double avgSalaire; };
    UsersStats getStats() const;

private slots:
    void ajouterEmploye();
    void modifierEmploye();
    void supprimerEmploye();
    void rechercherEmploye(const QString &texte);
    void actualiserTable();
    void exporterCSV();

private:
    void configurerInterface();
    void configurerTableauBord();
    void appliquerStyles();
    void mettreAJourStatistiques();
    void mettreAJourStatistiquesSalaires();

    QStackedWidget *pages;
    QWidget *pageTableauBord;

    QPushButton *btnAjouter;
    QPushButton *btnModifier;
    QPushButton *btnSupprimer;
    QPushButton *btnConsulter;
    QPushButton *btnExporter;

    QTableWidget *tableEmployes;
    QLineEdit *lineRecherche;

    QLabel *statPecheur;
    QLabel *statMatelot;
    QLabel *statDocker;
    QLabel *statChefQuai;
    QLabel *statSuperviseur;
    QLabel *salaryAverage;
    QLabel *salaryHighest;
    QLabel *salaryLowest;
    QLabel *salaryTotal;

    QList<Employe> listeEmployes;
};

#endif
