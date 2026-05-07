#include "affectation.h"
#include "quai.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <algorithm>
#include <cmath>

AffectationWidget::AffectationWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QGroupBox *navBox = new QGroupBox("Caracteristiques du navire");
    QFormLayout *navLayout = new QFormLayout(navBox);
    navireNomEdit = new QLineEdit;
    navireTirantSpin = new QDoubleSpinBox;
    navireTirantSpin->setRange(0.5, 25.0);
    navireTirantSpin->setValue(3.5);
    navireTirantSpin->setSuffix(" m");
    navirePrioriteBox = new QComboBox;
    navirePrioriteBox->addItems({"Normal", "VIP", "Urgence"});
    dateArriveeEdit = new QDateEdit(QDate::currentDate());
    dateArriveeEdit->setCalendarPopup(true);
    dureeSpin = new QSpinBox;
    dureeSpin->setRange(1, 180);
    dureeSpin->setValue(3);
    dureeSpin->setSuffix(" jours");
    navLayout->addRow("Nom :", navireNomEdit);
    navLayout->addRow("Tirant d'eau :", navireTirantSpin);
    navLayout->addRow("Priorite :", navirePrioriteBox);
    navLayout->addRow("Date arrivee :", dateArriveeEdit);
    navLayout->addRow("Duree sejour :", dureeSpin);

    QGroupBox *equipBox = new QGroupBox("Equipements requis");
    QHBoxLayout *eqLayout = new QHBoxLayout(equipBox);
    chkGrue = new QCheckBox("Grue");
    chkCarburant = new QCheckBox("Carburant");
    chkElec = new QCheckBox("Electricite");
    chkEau = new QCheckBox("Eau");
    eqLayout->addWidget(chkGrue);
    eqLayout->addWidget(chkCarburant);
    eqLayout->addWidget(chkElec);
    eqLayout->addWidget(chkEau);
    navLayout->addRow(equipBox);
    mainLayout->addWidget(navBox);

    btnLancer = new QPushButton("Analyser les quais");
    btnLancer->setStyleSheet("background:#0891b2; color:white; font-weight:bold; padding:10px;");
    mainLayout->addWidget(btnLancer);

    tableResultats = new QTableWidget(0, 5);
    tableResultats->setHorizontalHeaderLabels({"Quai", "Places libres", "Tirant d'eau", "Equipements", "Etat"});
    tableResultats->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mainLayout->addWidget(tableResultats);

    lblDetail = new QLabel;
    lblDetail->setWordWrap(true);
    lblDetail->setStyleSheet("background:#eff6ff; padding:10px; border-radius:8px;");
    mainLayout->addWidget(lblDetail);

    btnConfirmer = new QPushButton("Confirmer l'affectation");
    btnConfirmer->setEnabled(false);
    mainLayout->addWidget(btnConfirmer);

    connect(btnLancer, &QPushButton::clicked, this, &AffectationWidget::lancerAnalyse);
    connect(btnConfirmer, &QPushButton::clicked, this, &AffectationWidget::confirmerAffectation);

    refreshQuais();
}

void AffectationWidget::refreshQuais()
{
    quais.clear();
    QSqlQuery query("SELECT id_quai, nom, places_total, navires_presents, etat, tirant_eau_max, equipements FROM quai");
    while (query.next()) {
        InfoQuai q;
        q.id = query.value(0).toString();
        q.nom = query.value(1).toString();
        q.places = query.value(2).toInt();
        q.navires = query.value(3).toInt();
        q.etat = query.value(4).toString();
        q.tirantEau = query.value(5).toDouble();
        q.equipements = query.value(6).toString();
        QSqlQuery cnt("SELECT COUNT(*) FROM affectation WHERE quai_id = :id AND statut = 'Confirmee'");
        cnt.bindValue(":id", q.id);
        cnt.exec();
        cnt.next();
        q.nbAffectations = cnt.value(0).toInt();
        quais.append(q);
    }
}

QStringList AffectationWidget::equipementsSelectionnes() const
{
    QStringList equipements;
    if (chkGrue->isChecked()) equipements << "Grue";
    if (chkCarburant->isChecked()) equipements << "Carburant";
    if (chkElec->isChecked()) equipements << "Electricite";
    if (chkEau->isChecked()) equipements << "Eau";
    return equipements;
}

void AffectationWidget::lancerAnalyse()
{
    if (navireNomEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Entrez le nom du navire.");
        return;
    }

    const double tirant = navireTirantSpin->value();
    const QStringList equipReq = equipementsSelectionnes();

    QuaiDB quaiDb;
    QList<QuaiDB> quaisCompatibles;

    if (equipReq.isEmpty()) {
        quaisCompatibles = quaiDb.trouverQuaisCompatibles(QString(), tirant);
    } else {
        quaisCompatibles = quaiDb.trouverQuaisCompatibles(equipReq.first(), tirant);

        for (int i = 1; i < equipReq.size(); ++i) {
            const QList<QuaiDB> suivants = quaiDb.trouverQuaisCompatibles(equipReq[i], tirant);
            QList<QuaiDB> intersection;

            for (const QuaiDB &courant : std::as_const(quaisCompatibles)) {
                for (const QuaiDB &suivant : std::as_const(suivants)) {
                    if (courant.id_quai == suivant.id_quai) {
                        intersection.append(courant);
                        break;
                    }
                }
            }

            quaisCompatibles = intersection;
            if (quaisCompatibles.isEmpty()) {
                break;
            }
        }
    }

    if (quaisCompatibles.isEmpty()) {
        dernierResultat.clear();
        tableResultats->setRowCount(0);
        lblDetail->setText("Aucun quai disponible ne correspond aux criteres saisis.");
        btnConfirmer->setEnabled(false);
        return;
    }

    std::sort(quaisCompatibles.begin(), quaisCompatibles.end(),
              [](const QuaiDB &a, const QuaiDB &b) {
                  if (std::abs(a.tirant_eau_max - b.tirant_eau_max) > 0.0001) {
                      return a.tirant_eau_max < b.tirant_eau_max;
                  }
                  return a.id_quai < b.id_quai;
              });

    const double tirantMinimum = quaisCompatibles.first().tirant_eau_max;

    dernierResultat.clear();
    for (const QuaiDB &quai : std::as_const(quaisCompatibles)) {
        if (std::abs(quai.tirant_eau_max - tirantMinimum) > 0.0001) {
            break;
        }

        ResultatScoring resultat;
        resultat.quai.id = QString::number(quai.id_quai);
        resultat.quai.nom = quai.nom;
        resultat.quai.etat = quai.etat;
        resultat.quai.places = quai.places_total;
        resultat.quai.navires = quai.navires_presents;
        resultat.quai.tirantEau = quai.tirant_eau_max;
        resultat.quai.equipements = quai.equipements;
        resultat.quai.nbAffectations = 0;
        resultat.scoreTotal = 100;
        resultat.scoreCapacite = quai.places_total - quai.navires_presents;
        resultat.scoreTirant = quai.tirant_eau_max;
        resultat.scoreEquip = 100;
        resultat.scoreHistorique = 0;
        dernierResultat.append(resultat);
    }

    afficherResultats();
    btnConfirmer->setEnabled(!dernierResultat.isEmpty());
}

void AffectationWidget::afficherResultats()
{
    tableResultats->setRowCount(0);

    for (const ResultatScoring &resultat : std::as_const(dernierResultat)) {
        const int row = tableResultats->rowCount();
        tableResultats->insertRow(row);
        tableResultats->setItem(row, 0, new QTableWidgetItem(resultat.quai.nom));
        tableResultats->setItem(row, 1, new QTableWidgetItem(QString::number(resultat.quai.places - resultat.quai.navires)));
        tableResultats->setItem(row, 2, new QTableWidgetItem(QString::number(resultat.quai.tirantEau, 'f', 2) + " m"));
        tableResultats->setItem(row, 3, new QTableWidgetItem(resultat.quai.equipements));
        tableResultats->setItem(row, 4, new QTableWidgetItem(resultat.quai.etat));
    }

    if (dernierResultat.isEmpty()) {
        lblDetail->clear();
        return;
    }

    const ResultatScoring &best = dernierResultat.first();
    lblDetail->setText(
        QString("<b>Quais retenus :</b> %1<br>"
                "Tirant d'eau minimal retenu : %2 m<br>"
                "Le premier quai de la liste sera utilise pour la confirmation.")
            .arg(dernierResultat.size())
            .arg(best.quai.tirantEau, 0, 'f', 2));
}

void AffectationWidget::confirmerAffectation()
{
    if (dernierResultat.isEmpty()) return;
    const ResultatScoring &best = dernierResultat[0];

    QSqlQuery updateQuai;
    updateQuai.prepare("UPDATE quai SET navires_presents = navires_presents + 1 WHERE id_quai = :id");
    updateQuai.bindValue(":id", best.quai.id);
    if (!updateQuai.exec()) {
        QMessageBox::warning(this, "Erreur", "Impossible de mettre a jour le quai.");
        return;
    }

    QSqlQuery insertAffect;
    insertAffect.prepare("INSERT INTO affectation (navire_nom, tirant_navire, priorite, quai_id, date_arrivee, duree_estimee, score_final, statut) "
                         "VALUES (:navire, :tirant, :priorite, :quai, :date, :duree, :score, 'Confirmee')");
    insertAffect.bindValue(":navire", navireNomEdit->text().trimmed());
    insertAffect.bindValue(":tirant", navireTirantSpin->value());
    insertAffect.bindValue(":priorite", navirePrioriteBox->currentIndex() + 1);
    insertAffect.bindValue(":quai", best.quai.id);
    insertAffect.bindValue(":date", dateArriveeEdit->date());
    insertAffect.bindValue(":duree", dureeSpin->value());
    insertAffect.bindValue(":score", best.scoreTotal);
    if (insertAffect.exec()) {
        QMessageBox::information(this, "Succes", QString("Navire affecte au quai %1").arg(best.quai.nom));
        refreshQuais();
        navireNomEdit->clear();
        navireTirantSpin->setValue(3.5);
        navirePrioriteBox->setCurrentIndex(0);
        chkGrue->setChecked(false);
        chkCarburant->setChecked(false);
        chkElec->setChecked(false);
        chkEau->setChecked(false);
        dernierResultat.clear();
        tableResultats->setRowCount(0);
        lblDetail->clear();
        btnConfirmer->setEnabled(false);
    } else {
        QMessageBox::warning(this, "Erreur", "Echec de l'enregistrement : " + insertAffect.lastError().text());
    }
}
