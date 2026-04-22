#ifndef AFFECTATION_H
#define AFFECTATION_H

#include <QWidget>
#include <QVector>
#include <QStringList>
#include <QDate>
#include <QTableWidget>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QDateEdit>

struct InfoQuai {
    QString id;
    QString nom;
    QString etat;
    int places;
    int navires;
    double tirantEau;
    QString equipements;
    int nbAffectations;
};

struct ResultatScoring {
    InfoQuai quai;
    double scoreTotal;
    double scoreCapacite;
    double scoreTirant;
    double scoreEquip;
    double scoreHistorique;
};

class AffectationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AffectationWidget(QWidget *parent = nullptr);
    void refreshQuais();

private slots:
    void lancerAnalyse();
    void confirmerAffectation();

private:
    QStringList equipementsSelectionnes() const;
    void afficherResultats();

    QVector<InfoQuai> quais;
    QVector<ResultatScoring> dernierResultat;

    QLineEdit *navireNomEdit;
    QDoubleSpinBox *navireTirantSpin;
    QComboBox *navirePrioriteBox;
    QDateEdit *dateArriveeEdit;
    QSpinBox *dureeSpin;
    QCheckBox *chkGrue;
    QCheckBox *chkCarburant;
    QCheckBox *chkElec;
    QCheckBox *chkEau;

    QPushButton *btnLancer;
    QPushButton *btnConfirmer;
    QTableWidget *tableResultats;
    QLabel *lblDetail;
};

#endif // AFFECTATION_H
