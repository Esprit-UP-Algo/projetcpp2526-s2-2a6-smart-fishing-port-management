#pragma once
#include "gestioncaptures.h"
#include <QWidget>
#include <QTimer>
#include <QPointF>
#include <QVector>
#include <QMap>
#include <QString>
#include <QColor>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSoundEffect>
#include <QLineEdit>
#include <QListWidget>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>

// ── data structs ─────────────────────────────────────────────────────────────

struct Boat {
    QString mmsi;
    QString name;
    QString type;       // "chalutier" | "palangrier" | "artisanal" | "cargo"
    double  lat = 0, lon = 0;
    double  speed = 0;  // knots
    double  heading = 0;// degrees
    QVector<QPointF> trail; // normalized history
    bool    inRestrictedZone = false;
};

struct WeatherData {
    double windSpeed    = 0;   // km/h
    double windDir      = 0;   // degrees
    double waveHeight   = 0;   // meters
    double waterTemp    = 0;   // °C
    QString condition   = "—";
};

struct FishingZone {
    QString name;
    QPointF center;    // normalized 0-1
    double  radius;    // normalized
    QString status;    // "libre" | "restreint" | "interdit"
    QString details;
    int     boatCount  = 0;
};

struct City {
    QString name;
    QPointF pos;
    bool    capital = false;
};



class TunisiaMapWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TunisiaMapWidget(const QList<Capture> &captures, QWidget *parent = nullptr);
    ~TunisiaMapWidget() override;

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    void contextMenuEvent(QContextMenuEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;

private slots:
    void onAnimTick();
    void onAisFetched(QNetworkReply *reply);
    void onWeatherFetched(QNetworkReply *reply);

private:
    // init
    void initData();
    void buildSidebar();

    // fetch
    void fetchAIS();
    void fetchWeather();
    void simulateBoats();   // fallback when no API key

    // alert
    void checkAlerts();
    void triggerAlert(const QString &msg);

    // draw helpers
    void drawSea(QPainter &p, const QRect &r);
    void drawRelief(QPainter &p, const QRect &r);
    void drawWaves(QPainter &p, const QRect &r);
    void drawCurrentArrows(QPainter &p, const QRect &r);
    void drawZones(QPainter &p, const QRect &r);
    void drawBoats(QPainter &p, const QRect &r);
    void drawTrails(QPainter &p, const QRect &r);
    void drawCities(QPainter &p, const QRect &r);
    void drawWeatherOverlay(QPainter &p, const QRect &r);
    void drawCompass(QPainter &p, const QRect &r);
    void drawScale(QPainter &p, const QRect &r);
    void drawLegend(QPainter &p, const QRect &r);
    void drawRadar(QPainter &p, const QRect &r);
    void drawInfoPanel(QPainter &p, const QRect &r);
    void drawAlertBanner(QPainter &p);

    // coord helpers
    QPointF toPixel(const QPointF &norm, const QRect &r) const;
    QPointF toNorm(const QPointF &px, const QRect &r) const;
    QPointF geoToNorm(double lat, double lon) const;
    int     zoneAt(const QPointF &norm) const;
    int     boatAt(const QPointF &norm, const QRect &r) const;

    // map bounds (Tunisia area)
    static constexpr double LAT_MAX = 37.5, LAT_MIN = 30.2;
    static constexpr double LON_MIN =  7.5, LON_MAX = 11.6;

    // state
    int     m_tick       = 0;
    double  m_zoom       = 1.0;
    QPointF m_pan        = {0, 0};
    bool    m_nightMode  = false;
    bool    m_sidebarOpen= true;
    int     m_selectedZone = -1;
    int     m_selectedBoat = -1;
    QString m_alertMsg;
    int     m_alertTick  = 0;
    QString m_filterType = "tous";
    QString m_searchText;

    QVector<FishingZone> m_zones;
    QVector<City>        m_cities;
    QVector<Boat>        m_boats;
    WeatherData          m_weather;

    // timers
    QTimer *m_animTimer  = nullptr;
    QTimer *m_fetchTimer = nullptr;

    // network
    QNetworkAccessManager *m_nam = nullptr;

    // sound
    QSoundEffect *m_alertSound = nullptr;

    // sidebar widgets
    QWidget    *m_sidebar    = nullptr;
    QListWidget*m_boatList   = nullptr;
    QLineEdit  *m_searchEdit = nullptr;
    QComboBox  *m_filterCombo= nullptr;
    QLabel     *m_weatherLbl = nullptr;
    QPushButton*m_nightBtn   = nullptr;
};
