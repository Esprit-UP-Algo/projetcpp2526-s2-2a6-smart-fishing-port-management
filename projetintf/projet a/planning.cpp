#include "planning.h"
#include "quai.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QFileDialog>
#include <QTextStream>
#include <QDesktopServices>
#include <QDebug>
#include <QDate>
#include <QLocale>
#include <QFontMetrics>
#include <QtGlobal>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QStringConverter>
#endif

namespace {

QDate convertirDateOracle(const QString &texteBrut)
{
    QString texte;
    for (int i = 0; i < texteBrut.size(); ++i) {
        const QChar c = texteBrut.at(i);
        if (c.isDigit() || c == '-' || c == '/') {
            texte += c;
        }
    }

    texte = texte.trimmed();
    if (texte.isEmpty()) {
        return QDate();
    }

    if (texte.size() >= 10 && texte[4] == '-' && texte[7] == '-') {
        const int year = texte.mid(0, 4).toInt();
        const int month = texte.mid(5, 2).toInt();
        const int day = texte.mid(8, 2).toInt();
        return QDate(year, month, day);
    }

    if (texte.size() >= 8 && (texte[2] == '/' || texte[2] == '-') && (texte[5] == '/' || texte[5] == '-')) {
        const int day = texte.mid(0, 2).toInt();
        const int month = texte.mid(3, 2).toInt();
        int year = texte.mid(6).toInt();
        if (texte.mid(6).size() == 2) {
            year += 2000;
        }
        return QDate(year, month, day);
    }

    return QDate();
}

}

QList<QuaiDB> QuaiDB::recupererQuaisEnTravaux()
{
    QList<QuaiDB> quaisEnTravaux;

    QSqlQuery query;
    query.prepare(
        "SELECT id_quai, nom, places_total, navires_presents, adresse, etat, tirant_eau_max, equipements, "
        "EXTRACT(YEAR FROM date_debut) AS date_debut_year, "
        "EXTRACT(MONTH FROM date_debut) AS date_debut_month, "
        "EXTRACT(DAY FROM date_debut) AS date_debut_day, "
        "EXTRACT(YEAR FROM date_fin) AS date_fin_year, "
        "EXTRACT(MONTH FROM date_fin) AS date_fin_month, "
        "EXTRACT(DAY FROM date_fin) AS date_fin_day "
        "FROM quai "
        "WHERE UPPER(TRIM(NVL(etat, ''))) IN ('EN TRAVAUX', 'OUI') "
        "   OR (date_debut IS NOT NULL AND date_fin IS NOT NULL) "
        "ORDER BY id_quai ASC");

    if (!query.exec()) {
        qDebug() << "Erreur chargement quais en travaux:" << query.lastError().text();
        return quaisEnTravaux;
    }

    const QSqlRecord record = query.record();
    const int idxDebutYear = record.indexOf("DATE_DEBUT_YEAR");
    const int idxDebutMonth = record.indexOf("DATE_DEBUT_MONTH");
    const int idxDebutDay = record.indexOf("DATE_DEBUT_DAY");
    const int idxFinYear = record.indexOf("DATE_FIN_YEAR");
    const int idxFinMonth = record.indexOf("DATE_FIN_MONTH");
    const int idxFinDay = record.indexOf("DATE_FIN_DAY");

    while (query.next()) {
        QuaiDB quai;
        quai.id_quai = query.value(0).toInt();
        quai.nom = query.value(1).toString();
        quai.places_total = query.value(2).toInt();
        quai.navires_presents = query.value(3).toInt();
        quai.adresse = query.value(4).toString();
        quai.etat = query.value(5).toString();
        quai.tirant_eau_max = query.value(6).toDouble();
        quai.equipements = query.value(7).toString();
        const int debutYear = idxDebutYear >= 0 ? query.value(idxDebutYear).toInt() : 0;
        const int debutMonth = idxDebutMonth >= 0 ? query.value(idxDebutMonth).toInt() : 0;
        const int debutDay = idxDebutDay >= 0 ? query.value(idxDebutDay).toInt() : 0;
        const int finYear = idxFinYear >= 0 ? query.value(idxFinYear).toInt() : 0;
        const int finMonth = idxFinMonth >= 0 ? query.value(idxFinMonth).toInt() : 0;
        const int finDay = idxFinDay >= 0 ? query.value(idxFinDay).toInt() : 0;

        quai.date_debut = QDate(debutYear, debutMonth, debutDay);
        quai.date_fin = QDate(finYear, finMonth, finDay);
        quai.date_debut_txt = QString("%1-%2-%3")
                                  .arg(debutYear, 4, 10, QChar('0'))
                                  .arg(debutMonth, 2, 10, QChar('0'))
                                  .arg(debutDay, 2, 10, QChar('0'));
        quai.date_fin_txt = QString("%1-%2-%3")
                                .arg(finYear, 4, 10, QChar('0'))
                                .arg(finMonth, 2, 10, QChar('0'))
                                .arg(finDay, 2, 10, QChar('0'));

        qDebug() << "Planning - brut quai" << quai.id_quai
                 << "etat:" << quai.etat
                 << "debut Y/M/J:" << debutYear << debutMonth << debutDay
                 << "fin Y/M/J:" << finYear << finMonth << finDay;
        quaisEnTravaux.append(quai);
    }

    return quaisEnTravaux;
}

PlanningWidget::PlanningWidget(QWidget *parent) : QWidget(parent)
{
    QDate aujourdhui = QDate::currentDate();
    moisCourant = aujourdhui.month();
    anneeCourante = aujourdhui.year();

    btnPrev = new QPushButton("<");
    btnNext = new QPushButton(">");
    btnExport = new QPushButton("Exporter CSV");
    labelMois = new QLabel;
    labelMois->setAlignment(Qt::AlignCenter);
    labelMois->setStyleSheet("font-size: 16px; font-weight: bold;");

    QHBoxLayout *navLayout = new QHBoxLayout;
    navLayout->addWidget(btnPrev);
    navLayout->addStretch();
    navLayout->addWidget(labelMois);
    navLayout->addStretch();
    navLayout->addWidget(btnNext);
    navLayout->addSpacing(20);
    navLayout->addWidget(btnExport);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(navLayout);
    mainLayout->addStretch();

    setMinimumSize(700, 500);

    connect(btnPrev, &QPushButton::clicked, this, &PlanningWidget::moisPrecedent);
    connect(btnNext, &QPushButton::clicked, this, &PlanningWidget::moisSuivant);
    connect(btnExport, &QPushButton::clicked, this, &PlanningWidget::exporterCSV);

    refresh();
}

void PlanningWidget::refresh()
{
    chargerEvenements();
    labelMois->setText(QLocale().monthName(moisCourant, QLocale::LongFormat) + " " + QString::number(anneeCourante));
    update();
}

void PlanningWidget::chargerEvenements()
{
    evenements.clear();

    const QDate debutMois(anneeCourante, moisCourant, 1);
    const QDate finMois = debutMois.addMonths(1).addDays(-1);

    QSqlQuery query;
    query.prepare(
        "SELECT id_quai, nom, "
        "EXTRACT(YEAR FROM date_debut) AS date_debut_year, "
        "EXTRACT(MONTH FROM date_debut) AS date_debut_month, "
        "EXTRACT(DAY FROM date_debut) AS date_debut_day, "
        "EXTRACT(YEAR FROM date_fin) AS date_fin_year, "
        "EXTRACT(MONTH FROM date_fin) AS date_fin_month, "
        "EXTRACT(DAY FROM date_fin) AS date_fin_day "
        "FROM quai "
        "WHERE (UPPER(TRIM(NVL(etat, ''))) IN ('EN TRAVAUX', 'OUI') "
        "   OR (date_debut IS NOT NULL AND date_fin IS NOT NULL)) "
        "AND date_debut IS NOT NULL AND date_fin IS NOT NULL "
        "ORDER BY id_quai ASC");

    if (!query.exec()) {
        qDebug() << "Planning - erreur chargement quais:" << query.lastError().text();
        return;
    }

    int nbQuais = 0;
    while (query.next()) {
        const int idQuai = query.value(0).toInt();
        const QString nomQuai = query.value(1).toString();
        QDate dateDebut(query.value(2).toInt(), query.value(3).toInt(), query.value(4).toInt());
        QDate dateFin(query.value(5).toInt(), query.value(6).toInt(), query.value(7).toInt());

        qDebug() << "Planning - brut quai" << idQuai << nomQuai << dateDebut << dateFin;
        ++nbQuais;

        if (!dateDebut.isValid() || !dateFin.isValid()) {
            qDebug() << "Planning - dates invalides pour quai" << idQuai << nomQuai << dateDebut << dateFin;
            continue;
        }

        if (dateDebut > finMois || dateFin < debutMois) {
            continue;
        }

        if (dateDebut < debutMois) dateDebut = debutMois;
        if (dateFin > finMois) dateFin = finMois;

        const QString libelle = QString("Q%1 - %2").arg(idQuai).arg(nomQuai);
        const QString couleur = couleurPourQuai(idQuai).name();

        for (QDate d = dateDebut; d <= dateFin; d = d.addDays(1)) {
            EvenementCalendrier ev;
            ev.date = d;
            ev.texte = libelle;
            ev.type = "QuaiTravaux";
            ev.couleur = couleur;
            evenements.append(ev);
        }
    }

    qDebug() << "Planning - quais recuperes:" << nbQuais;
    qDebug() << "Planning - evenements charges:" << evenements.size();
}

QColor PlanningWidget::couleurPourQuai(int idQuai) const
{
    static const QVector<QColor> palette = {
        QColor("#fca5a5"),
        QColor("#fdba74"),
        QColor("#fde68a"),
        QColor("#86efac"),
        QColor("#93c5fd"),
        QColor("#c4b5fd"),
        QColor("#f9a8d4"),
        QColor("#67e8f9")
    };

    return palette[qAbs(idQuai) % palette.size()];
}

void PlanningWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    dessinerCalendrier(painter);
    dessinerLegende(painter);
}

void PlanningWidget::resizeEvent(QResizeEvent *)
{
    update();
}

void PlanningWidget::dessinerCalendrier(QPainter &painter)
{
    const int largeur = width();
    const int hauteur = height();
    const int margeG = 30;
    const int margeH = 80;
    const int zoneLargeur = largeur - 2 * margeG;
    const int zoneHauteur = hauteur - margeH;

    const QDate premierJour(anneeCourante, moisCourant, 1);
    const int joursDansMois = premierJour.daysInMonth();
    const int premierJourSemaine = premierJour.dayOfWeek();
    const int decalage = (premierJourSemaine + 5) % 7;

    const int nbLignes = (decalage + joursDansMois + 6) / 7;
    const int hauteurCase = zoneHauteur / nbLignes;
    const int largeurCase = zoneLargeur / 7;

    painter.fillRect(QRect(margeG, margeH, zoneLargeur, zoneHauteur), Qt::white);
    painter.setPen(QPen(Qt::gray, 1));

    for (int i = 0; i <= 7; ++i) {
        const int x = margeG + i * largeurCase;
        painter.drawLine(x, margeH, x, margeH + zoneHauteur);
    }
    for (int i = 0; i <= nbLignes; ++i) {
        const int y = margeH + i * hauteurCase;
        painter.drawLine(margeG, y, margeG + zoneLargeur, y);
    }

    const QStringList jours = {"Lun", "Mar", "Mer", "Jeu", "Ven", "Sam", "Dim"};
    QFont fontJours = painter.font();
    fontJours.setBold(true);
    painter.setFont(fontJours);
    for (int i = 0; i < 7; ++i) {
        const QRect rect(margeG + i * largeurCase, margeH - 20, largeurCase, 20);
        painter.drawText(rect, Qt::AlignCenter, jours[i]);
    }

    painter.setFont(QFont("Arial", 8));
    for (int jour = 1; jour <= joursDansMois; ++jour) {
        const int idx = decalage + jour - 1;
        const int ligne = idx / 7;
        const int col = idx % 7;
        const int x = margeG + col * largeurCase;
        const int y = margeH + ligne * hauteurCase;
        const QRect caseRect(x + 2, y + 2, largeurCase - 4, hauteurCase - 4);

        const QDate dateCourante(anneeCourante, moisCourant, jour);
        QVector<EvenementCalendrier> evenementsDuJour;
        for (const EvenementCalendrier &ev : evenements) {
            if (ev.date == dateCourante) {
                evenementsDuJour.append(ev);
            }
        }

        painter.fillRect(caseRect, Qt::white);
        painter.setPen(Qt::black);
        painter.drawText(caseRect.adjusted(2, 2, -2, -2), Qt::AlignTop | Qt::AlignLeft, QString::number(jour));

        const QRect zoneEvenements = caseRect.adjusted(4, 18, -4, -4);
        const int hauteurBadge = 16;
        const int espacement = 3;
        const int capaciteBrute = (zoneEvenements.height() + espacement) / (hauteurBadge + espacement);
        const int capacite = qMax(1, capaciteBrute);

        int lignesAffichees = evenementsDuJour.size();
        bool afficherResume = false;
        if (evenementsDuJour.size() > capacite) {
            afficherResume = true;
            lignesAffichees = (capacite > 1) ? (capacite - 1) : 0;
        }

        QFontMetrics metrics(painter.font());
        for (int i = 0; i < lignesAffichees; ++i) {
            const EvenementCalendrier &ev = evenementsDuJour[i];
            const QRect badgeRect(zoneEvenements.left(),
                                  zoneEvenements.top() + i * (hauteurBadge + espacement),
                                  zoneEvenements.width(),
                                  hauteurBadge);

            const QColor badgeColor(ev.couleur);
            painter.setPen(Qt::NoPen);
            painter.setBrush(badgeColor);
            painter.drawRoundedRect(badgeRect, 4, 4);

            const QColor texteColor = (badgeColor.lightness() < 150) ? Qt::white : Qt::black;
            painter.setPen(texteColor);
            const QString texte = metrics.elidedText(ev.texte, Qt::ElideRight, badgeRect.width() - 8);
            painter.drawText(badgeRect.adjusted(4, 0, -4, 0), Qt::AlignVCenter | Qt::AlignLeft, texte);
        }

        if (afficherResume) {
            const int restantes = evenementsDuJour.size() - lignesAffichees;
            const QRect badgeRect(zoneEvenements.left(),
                                  zoneEvenements.top() + lignesAffichees * (hauteurBadge + espacement),
                                  zoneEvenements.width(),
                                  hauteurBadge);
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor("#e5e7eb"));
            painter.drawRoundedRect(badgeRect, 4, 4);
            painter.setPen(QColor("#374151"));
            painter.drawText(badgeRect.adjusted(4, 0, -4, 0),
                             Qt::AlignVCenter | Qt::AlignLeft,
                             QString("+%1 autres").arg(restantes));
        }
    }
}

void PlanningWidget::dessinerLegende(QPainter &painter)
{
    const int y = height() - 25;
    painter.setPen(Qt::black);
    painter.fillRect(10, y - 10, 15, 15, QColor("#93c5fd"));
    painter.drawText(30, y, "Quai en travaux");
    painter.fillRect(170, y - 10, 15, 15, QColor("#e5e7eb"));
    painter.drawText(190, y, "Autres quais");
}

void PlanningWidget::moisPrecedent()
{
    if (--moisCourant < 1) {
        moisCourant = 12;
        anneeCourante--;
    }
    refresh();
}

void PlanningWidget::moisSuivant()
{
    if (++moisCourant > 12) {
        moisCourant = 1;
        anneeCourante++;
    }
    refresh();
}

void PlanningWidget::exporterCSV()
{
    if (evenements.isEmpty()) {
        QMessageBox::warning(this, "Export", "Aucun evenement ce mois-ci.");
        return;
    }

    const QString fileName = QFileDialog::getSaveFileName(
        this,
        "Exporter planning",
        QDir::homePath() + "/planning_" + QString::number(anneeCourante) + "_" + QString::number(moisCourant) + ".csv",
        "CSV (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Erreur", "Impossible d'ecrire le fichier.");
        return;
    }

    QTextStream out(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    out.setEncoding(QStringConverter::Utf8);
#else
    out.setCodec("UTF-8");
#endif

    out << "Date;Type;Description\n";
    for (const EvenementCalendrier &ev : evenements) {
        out << ev.date.toString("dd/MM/yyyy") << ";" << ev.type << ";\"" << ev.texte << "\"\n";
    }

    file.close();
    QMessageBox::information(this, "Export", "Export CSV termine.\n" + fileName);
    QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
}
