#ifndef GESTIONCAPTURES_H
#define GESTIONCAPTURES_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QDateEdit>
#include <QTextEdit>
#include <QSpinBox>
#include <QStackedWidget>
#include <QList>
#include <QMessageBox>
#include <QString>
#include <QDateTime>
#include <QUuid>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDir>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QGraphicsOpacityEffect>
#include <QGraphicsDropShadowEffect>
#include <QTimer>
#include <QScrollArea>
#include <QFrame>

struct Capture {
    QString captureId;
    QString shipName;
    QString fishType;
    QString fishIcon;
    int     quantity    = 0;
    QDate   captureDate = QDate::currentDate();
    QString time;
    QString agent;
    QString status      = "Enregistré";
    double  latitude    = 0.0;
    double  longitude   = 0.0;
    QString zone;
    QString method;
    QString comments;

    Capture() {
        captureId = QUuid::createUuid().toString();
        time      = QTime::currentTime().toString("HH:mm");
        fishIcon  = "🐟";
    }

    QJsonObject toJson() const {
        QJsonObject o;
        o["captureId"]   = captureId;
        o["shipName"]    = shipName;
        o["fishType"]    = fishType;
        o["fishIcon"]    = fishIcon;
        o["quantity"]    = quantity;
        o["captureDate"] = captureDate.toString("yyyy-MM-dd");
        o["time"]        = time;
        o["agent"]       = agent;
        o["status"]      = status;
        o["latitude"]    = latitude;
        o["longitude"]   = longitude;
        o["zone"]        = zone;
        o["method"]      = method;
        o["comments"]    = comments;
        return o;
    }

    void fromJson(const QJsonObject &o) {
        captureId   = o["captureId"].toString();
        shipName    = o["shipName"].toString();
        fishType    = o["fishType"].toString();
        fishIcon    = o["fishIcon"].toString();
        quantity    = o["quantity"].toInt();
        captureDate = QDate::fromString(o["captureDate"].toString(), "yyyy-MM-dd");
        time        = o["time"].toString();
        agent       = o["agent"].toString();
        status      = o["status"].toString();
        latitude    = o["latitude"].toDouble();
        longitude   = o["longitude"].toDouble();
        zone        = o["zone"].toString();
        method      = o["method"].toString();
        comments    = o["comments"].toString();
    }
};

class AnimatedButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(qreal glowOpacity READ glowOpacity WRITE setGlowOpacity)
public:
    explicit AnimatedButton(const QString &text, QWidget *parent = nullptr);
    qreal glowOpacity() const { return m_glow; }
    void  setGlowOpacity(qreal v) { m_glow = v; update(); }
protected:
    void paintEvent(QPaintEvent *e) override;
    void enterEvent(QEnterEvent *e) override;
    void leaveEvent(QEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
private:
    qreal m_glow   = 0.0;
    qreal m_scale  = 1.0;
    bool  m_pressed = false;
    QColor m_baseColor;
    QColor m_hoverColor;
    QColor m_glowColor;
    friend class GestionCaptures;
};

class PulseLabel : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(qreal pulse READ pulse WRITE setPulse)
public:
    explicit PulseLabel(const QString &text, QWidget *parent = nullptr);
    qreal pulse() const { return m_pulse; }
    void  setPulse(qreal v) { m_pulse = v; update(); }
    void  startPulse();
protected:
    void paintEvent(QPaintEvent *e) override;
private:
    qreal m_pulse = 0.0;
    QColor m_color;
};

class GestionCaptures : public QWidget
{
    Q_OBJECT
public:
    explicit GestionCaptures(QWidget *parent = nullptr);
    ~GestionCaptures();
    void loadFromDb();

    struct CapturesStats { int total; int poisson; int crustace; int mollusque; };
    CapturesStats getStats() const;

private slots:
    void onAjouter();
    void onStatistiques();
    void onTrierQuantite();
    void onCarte();
    void rechercherCapture(const QString &texte);
    void exporterCSV();
    void filtrerParStatut(const QString &statut);
    void animateRowsIn();

private:
    void configurerInterface();
    void configurerTableauBord();
    void ouvrirDialog(Capture *capture = nullptr, bool readOnly = false);
    void sauvegarderCapture(const Capture &c, bool modification, const QString &oldId);
    void afficherStatistiquesModal();
    void actualiserTable();
    void chargerCaptures();
    void sauvegarderCaptures();
    void mettreAJourStatistiques();
    void mettreAJourCompteur();
    void mettreAJourBadge();
    void mettreAJourTotaux();
    void mettreAJourKPI();
    void animateKpiUpdate(QLabel *lbl, const QString &newVal);
    QString genererCaptureId();

    // Layout
    QTableWidget   *tableCaptures  = nullptr;
    QLineEdit      *lineRecherche  = nullptr;
    QLabel         *labelCompteur  = nullptr;
    QLabel         *labelTotaux    = nullptr;
    AnimatedButton *btnAjouter     = nullptr;
    QStackedWidget *stackTable     = nullptr;
    QString         m_filtreStatut;
    int             m_maxQuantity  = 1;

    // KPI
    QLabel *kpiTotal   = nullptr;
    QLabel *kpiKg      = nullptr;
    QLabel *kpiNavires = nullptr;
    QLabel *kpiMoy     = nullptr;

    // Stats labels (legacy)
    QLabel *labelNbTotal     = nullptr;
    QLabel *labelNbPoisson   = nullptr;
    QLabel *labelNbCrustace  = nullptr;
    QLabel *labelNbMollusque = nullptr;

    // KPI card frames (for shadow animation)
    QList<QFrame*> m_kpiCards;

    QList<Capture> listeCaptures;
    QString m_jsonPath;
    bool    m_triAscendant = true;

    // Ticker for live clock in header
    QTimer *m_clockTimer = nullptr;
    QLabel *m_clockLabel = nullptr;
};

#endif
