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
#include <QProgressBar>
#include <QLabel>
#include <QVector>
#include <QPair>
#include <QList>
#include <QDate>
#include <QString>
#include <QGroupBox>

// =====================================================================
// CLASSE QUAI  (modèle de données)
// =====================================================================
class Quai
{
public:
    Quai() : placesTotales(0), naviresPresents(0), travaux(false) {}
    Quai(const QString &id, const QString &nom, int places, int navires,
         const QString &adresse, bool travaux)
        : id(id), nom(nom), placesTotales(places), naviresPresents(navires),
        adresse(adresse), travaux(travaux) {}

    QString getId()             const { return id; }
    QString getNom()            const { return nom; }
    int     getPlacesTotales()  const { return placesTotales; }
    int     getNaviresPresents()const { return naviresPresents; }
    QString getAdresse()        const { return adresse; }
    bool    getTravaux()        const { return travaux; }
    QString getEtat()           const {
        if (travaux)                         return "En travaux";
        if (naviresPresents >= placesTotales) return "Complet";
        return "Disponible";
    }

    void setNom(const QString &v)     { nom = v; }
    void setPlacesTotales(int v)      { placesTotales = v; }
    void setNaviresPresents(int v)    { naviresPresents = v; }
    void setAdresse(const QString &v) { adresse = v; }
    void setTravaux(bool v)           { travaux = v; }

private:
    QString id;
    QString nom;
    int     placesTotales;
    int     naviresPresents;
    QString adresse;
    bool    travaux;
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
// CLASSE HISTOGRAMME
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
// STRUCTURES
// =====================================================================
struct MaintenanceEntry {
    QString idQuai;
    QString nomQuai;
    QDate   datePrevue;
    QString typeIntervention;
    QString priorite;
    QString responsable;
    QString notes;
};

struct RapportOccupation {
    QString idQuai;
    QString nomQuai;
    int     placesTotales;
    int     naviresPresents;
    int     placesLibres;
    double  tauxOccupation;
    QString etat;
    QString alerte;
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
    void showMaintenance();
    void showRapportOccupation();
    void showExportCSV();

    void ajouterQuai();
    void supprimerQuai();
    void remplirFormulaireDepuisTable(int row, int col);
    void modifierQuai();

    void rechercherQuai(const QString &texte);
    void trierQuais(int index);

    void ajouterMaintenance();
    void supprimerMaintenance();
    void filtrerMaintenances(int index);
    void exporterMaintenances();

    void genererRapport();
    void filtrerRapportParEtat(int index);
    void exporterRapport();

    void exporterCSV();

    void annulerAjout();
    void annulerModif();

    void onPlacesChanged(const QString &);
    void onNaviresChanged(const QString &);
    void onTravauxChanged(int);

private:
    void    syncTables();
    void    updateStats();
    QString genererContenuCSV();
    QString calculerEtat(int places, int navires, bool travaux);
    void    refreshTableMaintenance();
    void    refreshTableRapport();

    // Données
    QList<Quai>               listeQuais;
    QVector<MaintenanceEntry> maintenances;
    QVector<RapportOccupation> rapportData;

    // Navigation
    QStackedWidget *pages;
    QWidget *pageAjouter;
    QWidget *pageModifier;
    QWidget *pageSupprimer;
    QWidget *pageConsulter;
    QWidget *pageStatistiques;
    QWidget *pageMaintenance;
    QWidget *pageRapportOccupation;
    QWidget *pageExportCSV;

    // Tables
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
    QPushButton *btnAnnulerAjout;

    // Formulaire Modifier
    QLineEdit   *idEditM;
    QLineEdit   *nomEditM;
    QLineEdit   *placesEditM;
    QLineEdit   *naviresEditM;
    QLineEdit   *adresseEditM;
    QCheckBox   *travauxCheckM;
    QPushButton *btnAnnulerModif;

    // Maintenance
    QTableWidget *tableMaintenance;
    QComboBox    *comboQuaiMaint;
    QDateEdit    *dateMaintenanceEdit;
    QComboBox    *comboTypeMaint;
    QComboBox    *comboPriorite;
    QLineEdit    *lineResponsable;
    QLineEdit    *lineNotesMaint;
    QComboBox    *comboFiltreStatut;
    QLabel       *lblResumeMaint;

    // Rapport d'occupation
    QTableWidget   *tableRapport;
    MonHistogramme *histoRapport;
    QComboBox      *comboFiltreRapport;
    QLabel         *lblSummaryRapport;
    QLabel         *lblAlertes;

    // Export CSV
    QLineEdit   *nomFichierEdit;
    QComboBox   *formatCSVBox;
    QLabel      *lblPreviewCSV;
    QPushButton *btnGenererCSV;
};

using GestionQuai = PageQuai;

#endif // QUAI_H
