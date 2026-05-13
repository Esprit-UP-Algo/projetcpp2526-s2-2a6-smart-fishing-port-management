#ifndef QUAI_H
#define QUAI_H

#include <QWidget>
#include <QStackedWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QVector>
#include <QPair>
#include <QList>
#include <QDate>
#include <QString>
#include "planning.h"
#include "affectation.h"
#include "arduino.h"
#include "BarriereWidget.h"
// =====================================================================
// CLASSE QUAI (modèle de données)
// =====================================================================
class Quai
{
public:
    Quai() : placesTotales(0), naviresPresents(0), travaux(false), tirantEauMax(0.0) {}
    Quai(const QString &id, const QString &nom, int places, int navires,
         const QString &adresse, bool travaux, double tirantEau = 0.0, const QString &equipements = "",
         const QDate &dateDebutTravaux = QDate(), const QDate &dateFinTravaux = QDate())
        : id(id), nom(nom), placesTotales(places), naviresPresents(navires),
        adresse(adresse), travaux(travaux), tirantEauMax(tirantEau), equipements(equipements),
        dateDebutTravaux(dateDebutTravaux), dateFinTravaux(dateFinTravaux) {}

    QString getId()              const { return id; }
    QString getNom()             const { return nom; }
    int     getPlacesTotales()   const { return placesTotales; }
    int     getNaviresPresents() const { return naviresPresents; }
    QString getAdresse()         const { return adresse; }
    bool    getTravaux()         const { return travaux; }
    double  getTirantEauMax()    const { return tirantEauMax; }
    QString getEquipements()     const { return equipements; }
    QDate   getDateDebutTravaux() const { return dateDebutTravaux; }
    QDate   getDateFinTravaux()   const { return dateFinTravaux; }
    QString getEtat()            const {
        if (travaux)                         return "En travaux";
        if (naviresPresents >= placesTotales) return "Complet";
        return "Disponible";
    }

    void setNom(const QString &v)           { nom = v; }
    void setPlacesTotales(int v)            { placesTotales = v; }
    void setNaviresPresents(int v)          { naviresPresents = v; }
    void setAdresse(const QString &v)       { adresse = v; }
    void setTravaux(bool v)                 { travaux = v; }
    void setTirantEauMax(double v)          { tirantEauMax = v; }
    void setEquipements(const QString &v)   { equipements = v; }
    void setDateDebutTravaux(const QDate &v){ dateDebutTravaux = v; }
    void setDateFinTravaux(const QDate &v)  { dateFinTravaux = v; }

private:
    QString id;
    QString nom;
    int     placesTotales;
    int     naviresPresents;
    QString adresse;
    bool    travaux;
    double  tirantEauMax;
    QString equipements;
    QDate   dateDebutTravaux;
    QDate   dateFinTravaux;
};

// =====================================================================
// CLASSE CAMEMBERT
// =====================================================================
class MonCamembert : public QWidget {
    Q_OBJECT
public:
    explicit MonCamembert(QWidget *parent = nullptr);
    void setValeurs(int occupe, int dispo);
protected:
    void paintEvent(QPaintEvent *) override;
private:
    int valeurOccupe, valeurDispo;
};

// =====================================================================
// CLASSE HISTOGRAMME (garde pour statistiques)
// =====================================================================
class MonHistogramme : public QWidget {
    Q_OBJECT
public:
    explicit MonHistogramme(QWidget *parent = nullptr);
    void setDonnees(const QVector<QPair<QString, int>> &data);
    void setOptions(bool grid, bool values);
protected:
    void paintEvent(QPaintEvent *) override;
private:
    QVector<QPair<QString, int>> donnees;
    bool showGrid;
    bool showValues;
};

// =====================================================================
// PAGE GESTION DES QUAIS
// =====================================================================
class PageQuai : public QWidget
{
    Q_OBJECT
public:
    explicit PageQuai(QWidget *parent = nullptr);
    void loadFromDb();
    struct QuaisStats { int totalQuais; int totalNaviresPresent; };
    QuaisStats getStats() const;

private slots:
    void showAjouter();
    void showModifier();
    void showSupprimer();
    void showConsulter();
    void showStatistiques();
    void showPlanning();
    void showAffectation();
    void showExportCSV();

    void ajouterQuai();
    void supprimerQuai();
    void remplirFormulaireDepuisTable(int row, int col);
    void onTableConsulterDoubleClicked(int row, int col);
    void modifierQuai();

    void rechercherQuai(const QString &texte);
    void trierQuais(int index);

    void exporterCSV();

    void annulerAjout();
    void annulerModif();

    void onPlacesChanged(const QString &);
    void onNaviresChanged(const QString &);
    void onTravauxChanged(int);
    void update_label();

private:
    void    syncTables();
    void    updateStats();
    QString genererContenuCSV();
    QString calculerEtat(int places, int navires, bool travaux);
    void    updateTravauxDateVisibility(bool formulaireModification);

    QList<Quai>               listeQuais;

    QStackedWidget *pages;
    QWidget *pageAjouter;
    QWidget *pageModifier;
    QWidget *pageSupprimer;
    QWidget *pageConsulter;
    QWidget *pageStatistiques;
    QWidget *pagePlanning;
    QWidget *pageAffectation;
    QWidget *pageExportCSV;

    QTableWidget *tableConsulter;
    QTableWidget *tableSupprimer;
    QTableWidget *tableModifier;

    QLineEdit *searchEdit;
    QComboBox *sortBox;
    MonCamembert *monGraphique;

    // Formulaire Ajouter
    QLineEdit   *idEdit;
    QLineEdit   *nomEdit;
    QLineEdit   *placesEdit;
    QLineEdit   *naviresEdit;
    QLineEdit   *adresseEdit;
    QCheckBox   *travauxCheck;
    QLabel      *dateDebutLabel;
    QDateEdit   *dateDebutEdit;
    QLabel      *dateFinLabel;
    QDateEdit   *dateFinEdit;
    QDoubleSpinBox *tirantEdit;
    QLineEdit      *equipementsEdit;
    QPushButton *btnAnnulerAjout;

    // Formulaire Modifier
    QLineEdit   *idEditM;
    QLineEdit   *nomEditM;
    QLineEdit   *placesEditM;
    QLineEdit   *naviresEditM;
    QLineEdit   *adresseEditM;
    QCheckBox   *travauxCheckM;
    QLabel      *dateDebutLabelM;
    QDateEdit   *dateDebutEditM;
    QLabel      *dateFinLabelM;
    QDateEdit   *dateFinEditM;
    QDoubleSpinBox *tirantEditM;
    QLineEdit      *equipementsEditM;
    QPushButton *btnAnnulerModif;

    PlanningWidget *planningWidget;
    AffectationWidget *affectationWidget;

    QLineEdit   *nomFichierEdit;
    QComboBox   *formatCSVBox;
    QLabel      *lblPreviewCSV;
    QPushButton *btnGenererCSV;

    BarriereWidget *barriereWidget;

    Arduino a;
    QString data;

    int id;
};

using GestionQuai = PageQuai;

#endif // QUAI_H
