#ifndef GESTIONNAVIRES_H
#define GESTIONNAVIRES_H

#include <QWidget>
#include <QList>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QStackedWidget>
#include <QDialog>
#include <QSpinBox>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>

class Navire {
public:
    Navire();
    Navire(QString nom, QString immat, int cap, QString stat);
    QString getNom() const;
    QString getImmatriculation() const;
    int getCapacite() const;
    QString getStatut() const;
    void setNom(QString n);
    void setImmatriculation(QString i);
    void setCapacite(int c);
    void setStatut(QString s);

private:
    QString nom;
    QString immatriculation;
    int capacite;
    QString statut;
};

class DialogNavire : public QDialog {
    Q_OBJECT
public:
    explicit DialogNavire(QWidget *parent = nullptr, Navire *navire = nullptr, bool adminMode = false);
    Navire getNavire() const;
private slots:
    void valider();
    void supprimer();
private:
    void configurerInterface();
    void appliquerStyles();
    QLineEdit *lineNom;
    QLineEdit *lineImmatriculation;
    QSpinBox *spinCapacite;
    QComboBox *comboStatut;
    QPushButton *btnEnregistrer;
    QPushButton *btnAnnuler;
    QPushButton *btnSupprimer;
    bool modeModification;
    bool modeAdmin;
};

class GestionNavires : public QWidget
{
    Q_OBJECT

public:
    explicit GestionNavires(QWidget *parent = nullptr);
    void loadFromDb();
    void mettreAJourStatistiques();
    struct NaviresStats { int total; int quai; int mer; int interdit; };
    NaviresStats getStats() const;

private slots:
    void ajouterNavire();
    void modifierNavire();
    void supprimerNavire();
    void rechercherNavire(const QString &texte);
    void afficherTableauBord();
    void afficherGestion();

private:
    void configurerInterface();
    void configurerTableauBord();
    void configurerGestion();
    void appliquerStyles();
    void chargerNavires();

    QStackedWidget *pages;
    QWidget *pageTableauBord;
    QWidget *pageGestion;

    QPushButton *btnAjouter;
    QPushButton *btnModifier;
    QPushButton *btnSupprimer;
    QPushButton *btnConsulter;

    QTableWidget *tableNavires;
    QLineEdit *lineRecherche;
    QLabel *labelNbTotal;
    QLabel *labelNbQuai;
    QLabel *labelNbMer;
    QLabel *labelNbInterdit;

    QLineEdit *champNom;
    QLineEdit *champImmat;
    QComboBox *comboCapacite;
    QComboBox *comboStatut;
    QTableWidget *tableGestion;

    QList<Navire> listeNavires;
};

#endif
