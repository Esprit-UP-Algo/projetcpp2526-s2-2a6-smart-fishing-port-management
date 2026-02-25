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
#include <limits>

struct Employe {
    QString id;
    QString nom;
    QString poste;
    QString email;
    QString telephone;
    double salaire;

    Employe() {
        id = QUuid::createUuid().toString().left(8);
        nom = "";
        poste = "Pêcheur";
        email = "";
        telephone = "";
        salaire = 0.0;
    }

    Employe(QString i, QString n, QString p, QString e, QString t, double s) {
        id = i;
        nom = n;
        poste = p;
        email = e;
        telephone = t;
        salaire = s;
    }

    QJsonObject toJson() const {
        QJsonObject obj;
        obj["id"] = id;
        obj["nom"] = nom;
        obj["poste"] = poste;
        obj["email"] = email;
        obj["telephone"] = telephone;
        obj["salaire"] = salaire;
        return obj;
    }

    void fromJson(const QJsonObject &obj) {
        id = obj["id"].toString();
        nom = obj["nom"].toString();
        poste = obj["poste"].toString();
        email = obj["email"].toString();
        telephone = obj["telephone"].toString();
        salaire = obj["salaire"].toDouble();
    }
};

class DialogEmploye : public QDialog
{
    Q_OBJECT

public:
    explicit DialogEmploye(QWidget *parent = nullptr, bool isEditMode = false);
    ~DialogEmploye();

    QString getId() const;
    QString getNom() const;
    QString getPoste() const;
    QString getEmail() const;
    QString getTelephone() const;
    double getSalaire() const;

    void setEmployeData(const QString &id, const QString &nom, const QString &poste,
                        const QString &email, const QString &telephone, double salaire = 0);

private slots:
    void onSaveClicked();
    void onCancelClicked();

private:
    bool validateInputs();

    QLineEdit *lineId;
    QLineEdit *lineNom;
    QComboBox *comboPoste;
    QLineEdit *lineEmail;
    QLineEdit *lineTelephone;
    QDoubleSpinBox *spinSalaire;
    QPushButton *btnSave;
    QPushButton *btnCancel;

    bool isEditMode;
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
    void chargerEmployes();
    void sauvegarderEmployes();
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
    QString m_jsonPath;
};

#endif
