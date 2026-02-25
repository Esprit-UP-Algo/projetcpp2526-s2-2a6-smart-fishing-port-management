#ifndef GESTIONCAPTURES_H
#define GESTIONCAPTURES_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QDateEdit>
#include <QDoubleSpinBox>
#include <QTextEdit>
#include <QSpinBox>
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

struct Capture {
    QString captureId;
    QString shipName;
    QString fishType;
    QString fishIcon;
    int quantity;
    QDate captureDate;
    QString time;
    QString agent;
    QString status;
    double latitude;
    double longitude;
    QString zone;
    QString method;
    QString comments;

    Capture() {
        captureId = QUuid::createUuid().toString();
        quantity = 0;
        latitude = 0.0;
        longitude = 0.0;
        captureDate = QDate::currentDate();
        time = QTime::currentTime().toString("HH:mm");
        status = "Enregistré";
        fishIcon = "🐟";
    }

    QJsonObject toJson() const {
        QJsonObject obj;
        obj["captureId"] = captureId;
        obj["shipName"] = shipName;
        obj["fishType"] = fishType;
        obj["fishIcon"] = fishIcon;
        obj["quantity"] = quantity;
        obj["captureDate"] = captureDate.toString("yyyy-MM-dd");
        obj["time"] = time;
        obj["agent"] = agent;
        obj["status"] = status;
        obj["latitude"] = latitude;
        obj["longitude"] = longitude;
        obj["zone"] = zone;
        obj["method"] = method;
        obj["comments"] = comments;
        return obj;
    }

    void fromJson(const QJsonObject &obj) {
        captureId = obj["captureId"].toString();
        shipName = obj["shipName"].toString();
        fishType = obj["fishType"].toString();
        fishIcon = obj["fishIcon"].toString();
        quantity = obj["quantity"].toInt();
        captureDate = QDate::fromString(obj["captureDate"].toString(), "yyyy-MM-dd");
        time = obj["time"].toString();
        agent = obj["agent"].toString();
        status = obj["status"].toString();
        latitude = obj["latitude"].toDouble();
        longitude = obj["longitude"].toDouble();
        zone = obj["zone"].toString();
        method = obj["method"].toString();
        comments = obj["comments"].toString();
    }
};

class GestionCaptures : public QWidget
{
    Q_OBJECT

public:
    explicit GestionCaptures(QWidget *parent = nullptr);
    ~GestionCaptures();

private slots:
    void ajouterCapture();
    void modifierCapture();
    void supprimerCapture();
    void rechercherCapture(const QString &texte);
    void actualiserTable();
    void exporterCSV();
    void afficherStatistiques();
    void viderFormulaire();

private:
    void configurerInterface();
    void configurerTableauBord();
    void configurerFormulaire();
    void appliquerStyles();
    void chargerCaptures();
    void sauvegarderCaptures();
    QString genererCaptureId();
    void mettreAJourStatistiques();

    QStackedWidget *pages;
    QWidget *pageTableauBord;
    QWidget *pageFormulaire;

    QPushButton *btnAjouter;
    QPushButton *btnModifier;
    QPushButton *btnSupprimer;
    QPushButton *btnConsulter;
    QPushButton *btnExporter;
    QPushButton *btnActualiser;

    QTableWidget *tableCaptures;
    QLineEdit *lineRecherche;

    QLabel *labelNbTotal;
    QLabel *labelNbPoisson;
    QLabel *labelNbCrustace;
    QLabel *labelNbMollusque;

    QLineEdit *editCaptureId;
    QLineEdit *editShipName;
    QComboBox *comboFishType;
    QLineEdit *editFishIcon;
    QSpinBox *spinQuantity;
    QDateEdit *dateCaptureDate;
    QLineEdit *editTime;
    QLineEdit *editAgent;
    QComboBox *comboStatus;
    QDoubleSpinBox *spinLatitude;
    QDoubleSpinBox *spinLongitude;
    QLineEdit *editZone;
    QComboBox *comboMethod;
    QTextEdit *textComments;

    QPushButton *btnSave;
    QPushButton *btnCancel;

    QList<Capture> listeCaptures;
    bool modeModification;
    QString currentCaptureId;

    QString m_jsonPath;
};

#endif
