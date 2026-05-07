#include "TunisiaMapWidget.h"
#include <QMap>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QContextMenuEvent>
#include <QMenu>
#include <QRadialGradient>
#include <QLinearGradient>
#include <QConicalGradient>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QUrl>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QFontMetrics>
#include <QtMath>
#include <QApplication>
#include <QRandomGenerator>
#include <QInputDialog>

// ── constructor ──────────────────────────────────────────────────────────────

TunisiaMapWidget::TunisiaMapWidget(const QList<Capture> &capture, QWidget *parent) : QWidget(parent)
{
    setMinimumSize(900, 640);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setMouseTracking(true);

    // Seed boats from capture (one boat per unique ship name)
    QMap<QString, bool> seen;
    auto *rng = QRandomGenerator::global();
    for (const Capture &c : capture) {
        if (seen.contains(c.shipName)) continue;
        seen[c.shipName] = true;
        Boat b;
        b.mmsi    = "CAP-" + c.captureId.left(6);
        b.name    = c.shipName;
        b.type    = c.fishType.isEmpty() ? "artisanal" : c.fishType;
        b.lat     = LAT_MIN + rng->bounded(100)/100.0*(LAT_MAX-LAT_MIN);
        b.lon     = 9.5 + rng->bounded(100)/100.0*(LON_MAX-9.5);
        b.speed   = 2.0 + rng->bounded(60)/10.0;
        b.heading = rng->bounded(360);
        m_boats.append(b);
    }

    m_nam = new QNetworkAccessManager(this);
    connect(m_nam, &QNetworkAccessManager::finished,
            this,  &TunisiaMapWidget::onAisFetched);

    m_alertSound = new QSoundEffect(this);
    m_alertSound->setSource(QUrl("qrc:/sounds/alert.wav")); // optional wav file
    m_alertSound->setVolume(0.8);

    initData();
    buildSidebar();
    simulateBoats();

    m_animTimer = new QTimer(this);
    connect(m_animTimer, &QTimer::timeout, this, &TunisiaMapWidget::onAnimTick);
    m_animTimer->start(40); // 25 fps

    m_fetchTimer = new QTimer(this);
    connect(m_fetchTimer, &QTimer::timeout, this, [this]() {
        fetchWeather();
        simulateBoats(); // replace with fetchAIS() when API key available
    });
    m_fetchTimer->start(30000); // refresh every 30s
    fetchWeather();
}

TunisiaMapWidget::~TunisiaMapWidget() {}

// ── initData ─────────────────────────────────────────────────────────────────

void TunisiaMapWidget::initData()
{
    m_zones = {
               {"Zone Bizerte",   {0.72, 0.08}, 0.07, "libre",
                "Thon, Sardine\nProfondeur: 50-200m\nSaison: Avr-Oct"},
               {"Zone Cap Bon",   {0.80, 0.20}, 0.06, "restreint",
                "Mérou, Daurade\nProfondeur: 20-80m\nFilet interdit"},
               {"Zone Sousse",    {0.82, 0.38}, 0.07, "libre",
                "Poulpe, Crevette\nProfondeur: 30-150m\nToute l'année"},
               {"Zone Sfax",      {0.78, 0.56}, 0.08, "port",
                "Port maritime\nQuais: 12\nCapacité: 200 navires"},
               {"Zone Gabès",     {0.72, 0.74}, 0.07, "interdit",
                "Zone protégée\nProfondeur: 5-40m\nPêche interdite Mar-Juin"},
               {"Zone Djerba",    {0.65, 0.86}, 0.06, "libre",
                "Homard, Langouste\nProfondeur: 15-80m\nSaison: Mai-Sep"},
               {"Zone Kerkennah", {0.74, 0.64}, 0.05, "restreint",
                "Pieuvre, Calamar\nProfondeur: 5-30m\nPêche artisanale uniquement"},
               };

    m_cities = {
                {"Tunis",    {0.42,0.18}, true},
                {"Bizerte",  {0.36,0.08}, false},
                {"Nabeul",   {0.62,0.28}, false},
                {"Sousse",   {0.58,0.40}, false},
                {"Monastir", {0.60,0.48}, false},
                {"Sfax",     {0.50,0.58}, false},
                {"Gabès",    {0.46,0.74}, false},
                {"Djerba",   {0.40,0.82}, false},
                {"Gafsa",    {0.28,0.62}, false},
                };


}

// ── buildSidebar ─────────────────────────────────────────────────────────────

void TunisiaMapWidget::buildSidebar()
{
    m_sidebar = new QWidget(this);
    m_sidebar->setFixedWidth(220);
    m_sidebar->setStyleSheet(
        "QWidget{background:#0f172a;color:#e2e8f0;font-family:'Segoe UI';}"
        "QListWidget{background:#1e293b;border:none;border-radius:6px;color:#e2e8f0;font-size:12px;}"
        "QListWidget::item:selected{background:#1d4ed8;}"
        "QLineEdit{background:#1e293b;border:1px solid #334155;border-radius:6px;"
        "  color:#e2e8f0;padding:5px 8px;font-size:12px;}"
        "QComboBox{background:#1e293b;border:1px solid #334155;border-radius:6px;"
        "  color:#e2e8f0;padding:4px 8px;font-size:12px;}"
        "QPushButton{background:#1d4ed8;color:white;border:none;border-radius:6px;"
        "  padding:6px;font-weight:700;font-size:12px;}"
        "QPushButton:hover{background:#2563eb;}"
        "QLabel{color:#94a3b8;font-size:11px;}"
        );

    auto *vbox = new QVBoxLayout(m_sidebar);
    vbox->setContentsMargins(8,8,8,8);
    vbox->setSpacing(6);

    // Title
    auto *titleLbl = new QLabel("⚓  Surveillance");
    titleLbl->setStyleSheet("font-size:14px;font-weight:900;color:white;padding:4px 0;");
    vbox->addWidget(titleLbl);

    // Night mode toggle
    m_nightBtn = new QPushButton("🌙  Mode nuit");
    connect(m_nightBtn, &QPushButton::clicked, this, [this]() {
        m_nightMode = !m_nightMode;
        m_nightBtn->setText(m_nightMode ? "☀️  Mode jour" : "🌙  Mode nuit");
        update();
    });
    vbox->addWidget(m_nightBtn);

    // Search by boat name
    auto *searchLbl = new QLabel("Rechercher navire");
    vbox->addWidget(searchLbl);
    m_searchEdit = new QLineEdit;
    m_searchEdit->setPlaceholderText("Nom du navire…");
    connect(m_searchEdit, &QLineEdit::textChanged, this, [this](const QString &t) {
        m_searchText = t.toLower();
        if (m_boatList) {
            for (int i = 0; i < m_boatList->count(); i++) {
                auto *item = m_boatList->item(i);
                item->setHidden(!t.isEmpty() &&
                                !item->text().toLower().contains(m_searchText));
            }
        }
        update();
    });
    vbox->addWidget(m_searchEdit);

    // Filter by fish type
    auto *filterLbl = new QLabel("Filtrer par type de poisson");
    vbox->addWidget(filterLbl);
    m_filterCombo = new QComboBox;
    m_filterCombo->addItems({"tous","Thon","Sardine","Crabe","Homard",
                             "Crevette","Saumon","Poulpe","Seiche","Mérou"});
    connect(m_filterCombo, &QComboBox::currentTextChanged, this, [this](const QString &t) {
        m_filterType = t;
        update();
    });
    vbox->addWidget(m_filterCombo);

    // Boat list
    auto *boatLbl = new QLabel("Navires actifs");
    boatLbl->setStyleSheet("color:#94a3b8;font-size:11px;margin-top:4px;");
    vbox->addWidget(boatLbl);
    m_boatList = new QListWidget;
    m_boatList->setMaximumHeight(200);
    connect(m_boatList, &QListWidget::currentRowChanged, this, [this](int row) {
        m_selectedBoat = row;
        update();
    });
    vbox->addWidget(m_boatList);

    // Weather panel
    auto *wLbl = new QLabel("Météo marine");
    wLbl->setStyleSheet("color:#94a3b8;font-size:11px;margin-top:4px;");
    vbox->addWidget(wLbl);
    m_weatherLbl = new QLabel("Chargement…");
    m_weatherLbl->setWordWrap(true);
    m_weatherLbl->setStyleSheet("background:#1e293b;border-radius:6px;padding:6px;"
                                "color:#e2e8f0;font-size:11px;");
    vbox->addWidget(m_weatherLbl);

    vbox->addStretch();

    // Collapse button
    auto *collapseBtn = new QPushButton("◀");
    collapseBtn->setFixedHeight(28);
    collapseBtn->setStyleSheet("background:#334155;color:white;border:none;border-radius:4px;");
    connect(collapseBtn, &QPushButton::clicked, this, [this, collapseBtn]() {
        m_sidebarOpen = !m_sidebarOpen;
        m_sidebar->setVisible(m_sidebarOpen);
        collapseBtn->setText(m_sidebarOpen ? "◀" : "▶");
        update();
    });
    vbox->addWidget(collapseBtn);
}

// ── simulateBoats (fallback / demo) ──────────────────────────────────────────

void TunisiaMapWidget::simulateBoats()
{
    static const QStringList names = {
        "Carthage I","Bizerte Star","Sfax Fisher","Djerba Pearl",
        "Cap Bon","Sousse Marin","Kerkennah","Tunis Bay","Nabeul II","Gabès"
    };
    static const QStringList types = {
        "chalutier","palangrier","artisanal","cargo",
        "chalutier","artisanal","palangrier","cargo","chalutier","artisanal"
    };

    auto *rng = QRandomGenerator::global();

    if (m_boats.isEmpty()) {
        for (int i = 0; i < names.size(); i++) {
            Boat b;
            b.mmsi    = QString("636%1").arg(1000+i);
            b.name    = names[i];
            b.type    = types[i];
            b.lat     = LAT_MIN + rng->bounded(100) / 100.0 * (LAT_MAX - LAT_MIN);
            b.lon     = 9.5 + rng->bounded(100) / 100.0 * (LON_MAX - 9.5);
            b.speed   = 2.0 + rng->bounded(120) / 10.0;
            b.heading = rng->bounded(360);
            m_boats.append(b);
        }
    } else {
        // Move boats slightly
        for (Boat &b : m_boats) {
            double rad = qDegreesToRadians(b.heading);
            b.lat += qCos(rad) * b.speed * 0.0001;
            b.lon += qSin(rad) * b.speed * 0.0001;
            // Bounce off bounds — stay in sea (lon >= 9.5)
            if (b.lat < LAT_MIN || b.lat > LAT_MAX) b.heading = 180 - b.heading;
            if (b.lon < 9.5     || b.lon > LON_MAX) b.heading = 360 - b.heading;
            b.lat = qBound(LAT_MIN, b.lat, LAT_MAX);
            b.lon = qBound(9.5,     b.lon, LON_MAX);
            // Trail
            QPointF norm = geoToNorm(b.lat, b.lon);
            b.trail.append(norm);
            if (b.trail.size() > 40) b.trail.removeFirst();
        }
    }

    // Update boat list widget
    if (m_boatList) {
        m_boatList->clear();
        for (const Boat &b : m_boats) {
            QString icon = (b.type=="chalutier") ? "🚢" :
                               (b.type=="palangrier") ? "⛵" :
                               (b.type=="cargo") ? "🛳️" : "🚤";
            m_boatList->addItem(icon + " " + b.name +
                                " (" + QString::number(b.speed,'f',1) + " kn)");
        }
    }

    checkAlerts();
    update();
}

// ── fetchWeather (open-meteo.com — free, no key) ─────────────────────────────

void TunisiaMapWidget::fetchWeather()
{
    // Open-Meteo marine API — Tunis coordinates
    QUrl url("https://marine-api.open-meteo.com/v1/marine"
             "?latitude=36.8&longitude=10.2"
             "&current=wave_height,wave_direction,ocean_current_velocity"
             "&hourly=wave_height");
    QNetworkRequest req(url);
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                     QNetworkRequest::NoLessSafeRedirectPolicy);
    auto *reply = m_nam->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onWeatherFetched(reply);
    });
}

void TunisiaMapWidget::onWeatherFetched(QNetworkReply *reply)
{
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        // Simulate fallback data
        m_weather.windSpeed  = 15 + QRandomGenerator::global()->bounded(20);
        m_weather.windDir    = QRandomGenerator::global()->bounded(360);
        m_weather.waveHeight = 0.5 + QRandomGenerator::global()->bounded(20)/10.0;
        m_weather.waterTemp  = 18 + QRandomGenerator::global()->bounded(8);
        m_weather.condition  = "Modéré";
    } else {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject cur = doc.object()["current"].toObject();
        m_weather.waveHeight = cur["wave_height"].toDouble();
        m_weather.windDir    = cur["wave_direction"].toDouble();
        m_weather.condition  = m_weather.waveHeight < 1.0 ? "Calme" :
                                  m_weather.waveHeight < 2.5 ? "Modéré" : "Agité";
    }

    if (m_weatherLbl) {
        m_weatherLbl->setText(
            QString("🌊 Vagues: %1 m\n"
                    "💨 Vent: %2 km/h %3°\n"
                    "🌡️ Eau: %4°C\n"
                    "📊 État: %5")
                .arg(m_weather.waveHeight, 0,'f',1)
                .arg(m_weather.windSpeed,  0,'f',0)
                .arg(m_weather.windDir,    0,'f',0)
                .arg(m_weather.waterTemp,  0,'f',1)
                .arg(m_weather.condition));
    }
    update();
}

void TunisiaMapWidget::fetchAIS()
{
    // Placeholder — requires aisstream.io API key
    // Replace YOUR_KEY with a real key from https://aisstream.io
    // QUrl url("wss://stream.aisstream.io/v0/stream");
    // For now, simulateBoats() is used as fallback
}

void TunisiaMapWidget::onAisFetched(QNetworkReply *reply)
{
    reply->deleteLater();
}

// ── checkAlerts ──────────────────────────────────────────────────────────────

void TunisiaMapWidget::checkAlerts()
{
    for (Boat &b : m_boats) {
        QPointF norm = geoToNorm(b.lat, b.lon);
        int zi = zoneAt(norm);
        bool inRestricted = (zi >= 0 && m_zones[zi].status != "libre");

        if (inRestricted && !b.inRestrictedZone) {
            b.inRestrictedZone = true;
            triggerAlert(b.name + " est entré dans " + m_zones[zi].name +
                         " (" + m_zones[zi].status + ")");
        } else if (!inRestricted) {
            b.inRestrictedZone = false;
        }
    }

    // Count boats per zone
    for (FishingZone &z : m_zones) z.boatCount = 0;
    for (const Boat &b : m_boats) {
        QPointF norm = geoToNorm(b.lat, b.lon);
        int zi = zoneAt(norm);
        if (zi >= 0) m_zones[zi].boatCount++;
    }
}

void TunisiaMapWidget::triggerAlert(const QString &msg)
{
    m_alertMsg  = "⚠️  " + msg;
    m_alertTick = 80;
    if (m_alertSound && m_alertSound->isLoaded())
        m_alertSound->play();
    else
        QApplication::beep();
    update();
}

// ── animation tick ────────────────────────────────────────────────────────────

void TunisiaMapWidget::onAnimTick()
{
    m_tick++;
    if (m_alertTick > 0) m_alertTick--;
    // Move boats every 15 frames (~600ms)
    if (m_tick % 15 == 0) simulateBoats();
    update();
}

// ── coordinate helpers ────────────────────────────────────────────────────────

QPointF TunisiaMapWidget::toPixel(const QPointF &norm, const QRect &r) const
{
    double x = r.x() + (norm.x() * m_zoom + m_pan.x()) * r.width();
    double y = r.y() + (norm.y() * m_zoom + m_pan.y()) * r.height();
    return {x, y};
}

QPointF TunisiaMapWidget::toNorm(const QPointF &px, const QRect &r) const
{
    double nx = (px.x() - r.x()) / r.width();
    double ny = (px.y() - r.y()) / r.height();
    return {(nx - m_pan.x()) / m_zoom, (ny - m_pan.y()) / m_zoom};
}

QPointF TunisiaMapWidget::geoToNorm(double lat, double lon) const
{
    double nx = (lon - LON_MIN) / (LON_MAX - LON_MIN);
    double ny = 1.0 - (lat - LAT_MIN) / (LAT_MAX - LAT_MIN);
    return {nx, ny};
}

int TunisiaMapWidget::zoneAt(const QPointF &norm) const
{
    for (int i = 0; i < m_zones.size(); i++) {
        double dx = norm.x() - m_zones[i].center.x();
        double dy = norm.y() - m_zones[i].center.y();
        if (qSqrt(dx*dx + dy*dy) < m_zones[i].radius) return i;
    }
    return -1;
}

int TunisiaMapWidget::boatAt(const QPointF &norm, const QRect &r) const
{
    for (int i = 0; i < m_boats.size(); i++) {
        QPointF bNorm = geoToNorm(m_boats[i].lat, m_boats[i].lon);
        QPointF bPx   = toPixel(bNorm, r);
        QPointF mPx   = toPixel(norm, r);
        double dx = bPx.x() - mPx.x();
        double dy = bPx.y() - mPx.y();
        if (qSqrt(dx*dx+dy*dy) < 12) return i;
    }
    return -1;
}


// ── paintEvent ───────────────────────────────────────────────────────────────

void TunisiaMapWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);

    int sideW = (m_sidebarOpen && m_sidebar) ? m_sidebar->width() : 0;
    QRect mapR(sideW, 0, width() - sideW, height() - 90);
    QRect infoR(sideW, height() - 90, width() - sideW, 90);

    if (m_sidebar) m_sidebar->setGeometry(0, 0, m_sidebar->width(), height());

    drawSea(p, mapR);
    drawRelief(p, mapR);
    drawWaves(p, mapR);
    drawCurrentArrows(p, mapR);
    drawTrails(p, mapR);
    drawZones(p, mapR);
    drawBoats(p, mapR);
    drawCities(p, mapR);
    drawWeatherOverlay(p, mapR);
    drawCompass(p, mapR);
    drawScale(p, mapR);
    drawLegend(p, mapR);
    drawRadar(p, mapR);
    drawInfoPanel(p, infoR);
    if (m_alertTick > 0) drawAlertBanner(p);
}

// ── drawSea ──────────────────────────────────────────────────────────────────

void TunisiaMapWidget::drawSea(QPainter &p, const QRect &r)
{
    // ── Fond bathymétrique multi-couches ──────────────────────────────────
    // Couche 1 : fond profond (centre mer)
    QRadialGradient bathyGrad(r.center(), r.width() * 0.7);
    if (m_nightMode) {
        bathyGrad.setColorAt(0.0, QColor(2, 18, 38));
        bathyGrad.setColorAt(0.4, QColor(4, 28, 58));
        bathyGrad.setColorAt(0.7, QColor(8, 40, 80));
        bathyGrad.setColorAt(1.0, QColor(12, 55, 100));
    } else {
        bathyGrad.setColorAt(0.0, QColor(2, 62, 138));
        bathyGrad.setColorAt(0.3, QColor(5, 90, 170));
        bathyGrad.setColorAt(0.6, QColor(14, 130, 200));
        bathyGrad.setColorAt(1.0, QColor(56, 189, 248));
    }
    p.fillRect(r, bathyGrad);

    // Couche 2 : zone côtière peu profonde (dégradé depuis la côte tunisienne)
    QLinearGradient shallowG(toPixel({0.55,0.0},r), toPixel({1.0,0.0},r));
    QColor shallowC = m_nightMode ? QColor(20,80,140,90) : QColor(125,211,252,100);
    shallowG.setColorAt(0.0, shallowC);
    shallowG.setColorAt(1.0, Qt::transparent);
    p.fillRect(r, shallowG);

    // Couche 3 : isobathe 200m (ligne de profondeur)
    p.setPen(QPen(QColor(255,255,255, m_nightMode?18:28), 1.0, Qt::DotLine));
    const QVector<QPointF> iso200 = {
        {0.60,0.05},{0.65,0.12},{0.68,0.22},{0.70,0.34},
        {0.72,0.46},{0.70,0.58},{0.68,0.68},{0.65,0.78},{0.62,0.88}
    };
    QPainterPath isoPath;
    isoPath.moveTo(toPixel(iso200[0], r));
    for (int i=1;i<iso200.size();i++) isoPath.lineTo(toPixel(iso200[i],r));
    p.drawPath(isoPath);

    // ── Contour Tunisie précis ────────────────────────────────────────────
    const QVector<QPointF> land = {
        // Nord : Cap Blanc → Bizerte → Lac de Bizerte
        {0.30,0.00},{0.36,0.00},{0.40,0.03},{0.44,0.05},
        // Cap Bon
        {0.52,0.08},{0.58,0.12},{0.63,0.18},{0.64,0.24},
        {0.62,0.30},{0.60,0.36},
        // Côte est : Sousse → Sfax
        {0.59,0.42},{0.57,0.48},{0.55,0.54},{0.53,0.60},
        // Golfe de Gabès
        {0.50,0.66},{0.48,0.72},{0.46,0.76},
        // Djerba / Zarzis
        {0.44,0.82},{0.40,0.88},{0.36,0.93},{0.32,0.96},
        // Sud-ouest
        {0.24,0.98},{0.16,0.96},{0.10,0.90},{0.08,0.82},
        // Chott el Jérid / intérieur
        {0.10,0.72},{0.12,0.62},{0.14,0.52},{0.16,0.42},
        {0.18,0.32},{0.20,0.22},{0.22,0.14},{0.26,0.06},{0.30,0.00}
    };
    QPainterPath landPath;
    landPath.moveTo(toPixel(land[0], r));
    for (int i = 1; i < land.size(); i++) landPath.lineTo(toPixel(land[i], r));
    landPath.closeSubpath();

    // Relief terrestre avec dégradé réaliste
    QLinearGradient landG(r.topLeft(), r.bottomRight());
    if (m_nightMode) {
        landG.setColorAt(0.0, QColor(30, 50, 28));
        landG.setColorAt(0.35,QColor(55, 42, 18));
        landG.setColorAt(0.65,QColor(80, 60, 20));
        landG.setColorAt(1.0, QColor(100, 75, 30));
    } else {
        landG.setColorAt(0.0, QColor(134,197,120)); // nord vert
        landG.setColorAt(0.3, QColor(210,180,120)); // centre ocre
        landG.setColorAt(0.6, QColor(220,165,90));  // sud semi-aride
        landG.setColorAt(1.0, QColor(200,140,70));  // désert
    }
    p.fillPath(landPath, landG);

    // Ombrage côtier (effet relief)
    QPainterPath shadowPath = landPath;
    p.setPen(QPen(QColor(0,0,0,40), 6));
    p.setBrush(Qt::NoBrush);
    p.drawPath(shadowPath);

    // Contour côtier net
    p.setPen(QPen(m_nightMode ? QColor(80,110,140) : QColor(100,120,90), 1.8));
    p.setBrush(Qt::NoBrush);
    p.drawPath(landPath);

    // ── Grille de coordonnées géographiques ──────────────────────────────
    p.setPen(QPen(QColor(255,255,255, m_nightMode?22:18), 0.7, Qt::DotLine));
    p.setFont(QFont("Consolas", 7));
    p.setPen(QColor(255,255,255, m_nightMode?60:50));
    // Méridiens (lon)
    for (double lon = 8.0; lon <= 11.5; lon += 0.5) {
        double nx = (lon - LON_MIN)/(LON_MAX - LON_MIN);
        QPointF top = toPixel({nx, 0.0}, r);
        QPointF bot = toPixel({nx, 1.0}, r);
        p.setPen(QPen(QColor(255,255,255, m_nightMode?20:15), 0.6, Qt::DotLine));
        p.drawLine(top, bot);
        p.setPen(QColor(255,255,255, m_nightMode?80:60));
        p.setFont(QFont("Consolas", 7));
        p.drawText(QPointF(top.x()+2, r.top()+10), QString::number(lon,'f',1)+"°E");
    }
    // Parallèles (lat)
    for (double lat = 31.0; lat <= 37.5; lat += 1.0) {
        double ny = 1.0 - (lat - LAT_MIN)/(LAT_MAX - LAT_MIN);
        QPointF lft = toPixel({0.0, ny}, r);
        QPointF rgt = toPixel({1.0, ny}, r);
        p.setPen(QPen(QColor(255,255,255, m_nightMode?20:15), 0.6, Qt::DotLine));
        p.drawLine(lft, rgt);
        p.setPen(QColor(255,255,255, m_nightMode?80:60));
        p.setFont(QFont("Consolas", 7));
        p.drawText(QPointF(r.left()+2, lft.y()-2), QString::number(lat,'f',0)+"°N");
    }
}

// ── drawRelief ───────────────────────────────────────────────────────────────

void TunisiaMapWidget::drawRelief(QPainter &p, const QRect &r)
{
    // ── Port Maritime (centre mer, position zone Sfax) ────────────────────
    const QPointF portNorm(0.78, 0.56);
    QPointF C = toPixel(portNorm, r);
    double  W = 0.060 * r.width()  * m_zoom;
    double  H = 0.042 * r.height() * m_zoom;

    // Eau du bassin
    QColor water  = m_nightMode ? QColor(8,40,90,220)   : QColor(14,116,144,200);
    QColor border = m_nightMode ? QColor(56,189,248,200) : QColor(3,105,161,220);
    QColor quai   = m_nightMode ? QColor(71,85,105)      : QColor(148,163,184);

    // Bassin principal
    p.setBrush(water);
    p.setPen(QPen(border, 2.0));
    p.drawRoundedRect(QRectF(C.x()-W, C.y()-H, W*2, H*2), 5, 5);

    // Quais internes (3 lignes horizontales = postes d'amarrage)
    p.setPen(QPen(quai, 2.5));
    for (int i = 1; i <= 3; i++) {
        double qy = C.y() - H + i * (H * 2.0 / 4.0);
        p.drawLine(QPointF(C.x()-W+5, qy), QPointF(C.x()+W-5, qy));
    }
    // Quais verticaux (séparateurs)
    p.setPen(QPen(quai.darker(120), 1.5));
    p.drawLine(QPointF(C.x(), C.y()-H+4), QPointF(C.x(), C.y()+H-4));

    // Jetées extérieures (brise-lames)
    p.setPen(QPen(quai.darker(150), 4, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(QPointF(C.x()-W,   C.y()-H), QPointF(C.x()-W*1.5, C.y()+H*0.2));
    p.drawLine(QPointF(C.x()+W,   C.y()-H), QPointF(C.x()+W*1.5, C.y()+H*0.2));

    // Phare (petit cercle lumineux)
    double lx = C.x() + W*1.5, ly = C.y() + H*0.2;
    int pulse = 120 + 100*(int)qAbs(qSin(m_tick * 0.08));
    p.setBrush(QColor(255,220,50, pulse));
    p.setPen(Qt::NoPen);
    p.drawEllipse(QPointF(lx, ly), 5, 5);
    p.setBrush(QColor(255,220,50,40));
    p.drawEllipse(QPointF(lx, ly), 10, 10);

    // Icône ancre
    p.setPen(m_nightMode ? QColor(147,197,253) : QColor(15,58,95));
    p.setFont(QFont("Segoe UI", qMax(9,(int)(W*0.38))));
    p.drawText(QRectF(C.x()-W, C.y()-H, W*2, H*2), Qt::AlignCenter, "\u2693");

    // Label
    p.setPen(m_nightMode ? QColor(226,232,240) : QColor(15,23,42));
    p.setFont(QFont("Segoe UI", 8, QFont::Bold));
    QRectF lblR(C.x()-W*2, C.y()+H+4, W*4, 15);
    p.fillRect(lblR, QColor(0,0,0,80));
    p.drawText(lblR, Qt::AlignCenter, "Port Maritime");
}


// ── drawWaves ────────────────────────────────────────────────────────────────

void TunisiaMapWidget::drawWaves(QPainter &p, const QRect &r)
{
    p.setPen(QPen(QColor(255,255,255, m_nightMode?30:55), 1.2));
    double phase = m_tick * 0.08;
    for (int row = 0; row < 12; row++) {
        double ny = 0.08 + row * 0.08;
        QPainterPath wave;
        bool started = false;
        for (double nx = 0.55; nx <= 1.0; nx += 0.012) {
            double y = ny + 0.008 * qSin(nx * 20 + phase + row * 0.7);
            QPointF px = toPixel({nx, y}, r);
            if (!r.contains(px.toPoint())) { started = false; continue; }
            if (!started) { wave.moveTo(px); started = true; }
            else wave.lineTo(px);
        }
        p.drawPath(wave);
    }
}

// ── drawCurrentArrows ────────────────────────────────────────────────────────

void TunisiaMapWidget::drawCurrentArrows(QPainter &p, const QRect &r)
{
    double dir = qDegreesToRadians(m_weather.windDir + 90);
    double dx = qCos(dir) * 0.06, dy = qSin(dir) * 0.06;
    QColor arrowColor(100, 200, 255, m_nightMode ? 100 : 70);
    p.setPen(QPen(arrowColor, 1.5));

    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 4; col++) {
            double nx = 0.60 + col * 0.10;
            double ny = 0.10 + row * 0.18;
            double anim = 0.02 * qSin(m_tick * 0.06 + row + col);
            QPointF from = toPixel({nx + anim, ny + anim}, r);
            QPointF to   = toPixel({nx + dx + anim, ny + dy + anim}, r);
            if (!r.contains(from.toPoint())) continue;
            p.drawLine(from, to);
            // arrowhead
            double angle = qAtan2(to.y()-from.y(), to.x()-from.x());
            double hs = 6;
            p.drawLine(to, QPointF(to.x()-hs*qCos(angle-0.4), to.y()-hs*qSin(angle-0.4)));
            p.drawLine(to, QPointF(to.x()-hs*qCos(angle+0.4), to.y()-hs*qSin(angle+0.4)));
        }
    }
}

// ── drawTrails ───────────────────────────────────────────────────────────────

void TunisiaMapWidget::drawTrails(QPainter &p, const QRect &r)
{
    for (int i = 0; i < m_boats.size(); i++) {
        const Boat &b = m_boats[i];
        if (b.trail.size() < 2) continue;
        if (m_filterType != "tous" && b.type != m_filterType) continue;

        QColor tc = (b.type=="chalutier") ? QColor("#3b82f6") :
                        (b.type=="palangrier") ? QColor("#10b981") :
                        (b.type=="cargo")      ? QColor("#f59e0b") : QColor("#8b5cf6");

        for (int j = 1; j < b.trail.size(); j++) {
            int alpha = 30 + (j * 180 / b.trail.size());
            tc.setAlpha(alpha);
            p.setPen(QPen(tc, 1.5));
            p.drawLine(toPixel(b.trail[j-1], r), toPixel(b.trail[j], r));
        }
    }
}

// ── drawZones ────────────────────────────────────────────────────────────────

void TunisiaMapWidget::drawZones(QPainter &p, const QRect &r)
{
    // Formes polygonales réalistes pour chaque zone (au lieu d'ellipses)
    // Chaque zone = liste de points normalisés formant un polygone irrégulier
    static const QVector<QVector<QPointF>> shapes = {
        // Bizerte
        {{0.66,0.04},{0.78,0.04},{0.82,0.08},{0.80,0.14},{0.72,0.15},{0.65,0.12}},
        // Cap Bon
        {{0.74,0.16},{0.86,0.16},{0.88,0.22},{0.84,0.28},{0.76,0.27},{0.72,0.22}},
        // Sousse
        {{0.75,0.32},{0.88,0.32},{0.90,0.40},{0.86,0.46},{0.76,0.45},{0.73,0.38}},
        // Sfax (port — skip)
        {},
        // Gabès
        {{0.65,0.68},{0.80,0.68},{0.82,0.76},{0.78,0.82},{0.66,0.81},{0.62,0.74}},
        // Djerba
        {{0.58,0.82},{0.72,0.82},{0.74,0.90},{0.68,0.94},{0.58,0.92},{0.55,0.87}},
        // Kerkennah
        {{0.68,0.58},{0.80,0.58},{0.82,0.64},{0.78,0.70},{0.68,0.69},{0.65,0.63}},
    };

    for (int i = 0; i < m_zones.size(); i++) {
        const FishingZone &z = m_zones[i];
        if (z.status == "port") continue;
        if (i >= shapes.size() || shapes[i].isEmpty()) continue;

        QColor fill, border;
        if      (z.status=="libre")     { fill=QColor(16,185,129,60);  border=QColor(16,185,129,220); }
        else if (z.status=="restreint") { fill=QColor(245,158,11,60);  border=QColor(245,158,11,220); }
        else                            { fill=QColor(239,68,68,60);   border=QColor(239,68,68,220);  }

        // Construire le polygone
        QPainterPath zonePath;
        zonePath.moveTo(toPixel(shapes[i][0], r));
        for (int j = 1; j < shapes[i].size(); j++)
            zonePath.lineTo(toPixel(shapes[i][j], r));
        zonePath.closeSubpath();

        // Pulsation animée (halo extérieur)
        double pulse = 1.0 + 0.06 * qSin(m_tick * 0.10 + i * 1.2);
        QTransform t;
        QPointF cx = toPixel(z.center, r);
        t.translate(cx.x(), cx.y());
        t.scale(pulse, pulse);
        t.translate(-cx.x(), -cx.y());
        p.setPen(QPen(border.lighter(140), 1.2, Qt::DashLine));
        p.setBrush(Qt::NoBrush);
        p.drawPath(t.map(zonePath));

        // Zone remplie
        p.setBrush(i == m_selectedZone ? fill.lighter(160) : fill);
        p.setPen(QPen(border, i == m_selectedZone ? 2.5 : 1.8));
        p.drawPath(zonePath);

        // Badge nombre de navires
        if (z.boatCount > 0) {
            QPointF badgePx = toPixel({z.center.x()+z.radius*0.7, z.center.y()-z.radius*0.7}, r);
            p.setBrush(QColor(29,78,216,230));
            p.setPen(Qt::NoPen);
            p.drawEllipse(badgePx, 10, 10);
            p.setPen(Qt::white);
            p.setFont(QFont("Segoe UI", 7, QFont::Bold));
            p.drawText(QRectF(badgePx.x()-10, badgePx.y()-8, 20, 16),
                       Qt::AlignCenter, QString::number(z.boatCount));
        }

        // Nom de la zone
        QPointF namePx = toPixel(z.center, r);
        p.setPen(m_nightMode ? border.lighter(200) : border.darker(160));
        p.setFont(QFont("Segoe UI", 8, QFont::Bold));
        p.drawText(QRectF(namePx.x()-50, namePx.y()-9, 100, 18),
                   Qt::AlignCenter, z.name.split(" ").last());
        // Statut en dessous
        p.setFont(QFont("Segoe UI", 7));
        p.setPen(border.lighter(m_nightMode?180:120));
        p.drawText(QRectF(namePx.x()-50, namePx.y()+4, 100, 14),
                   Qt::AlignCenter, z.status.toUpper());
    }
}


// ── drawBoats ────────────────────────────────────────────────────────────────

void TunisiaMapWidget::drawBoats(QPainter &p, const QRect &r)
{
    for (int i = 0; i < m_boats.size(); i++) {
        const Boat &b = m_boats[i];
        if (m_filterType != "tous" && !b.type.contains(m_filterType, Qt::CaseInsensitive)) continue;
        if (!m_searchText.isEmpty() && !b.name.toLower().contains(m_searchText)) continue;

        QPointF norm = geoToNorm(b.lat, b.lon);
        QPointF px   = toPixel(norm, r);
        if (!r.contains(px.toPoint())) continue;

        QColor bc = (b.type=="chalutier")  ? QColor(59,130,246)  :
                    (b.type=="palangrier") ? QColor(16,185,129)  :
                    (b.type=="cargo")      ? QColor(245,158,11)  : QColor(139,92,246);

        // Halo zone restreinte (clignotant rouge)
        if (b.inRestrictedZone) {
            int alpha = 60 + 80*(int)qAbs(qSin(m_tick*0.25));
            p.setBrush(QColor(239,68,68,alpha));
            p.setPen(Qt::NoPen);
            p.drawEllipse(px, 20, 20);
        }

        // Halo vitesse (cercle proportionnel à la vitesse)
        double speedR = qBound(8.0, b.speed * 2.2, 28.0);
        QRadialGradient speedHalo(px, speedR);
        speedHalo.setColorAt(0.0, QColor(bc.red(),bc.green(),bc.blue(),60));
        speedHalo.setColorAt(1.0, Qt::transparent);
        p.setBrush(speedHalo);
        p.setPen(Qt::NoPen);
        p.drawEllipse(px, speedR, speedR);

        // Icône navire détaillée selon le type
        p.save();
        p.translate(px);
        p.rotate(b.heading);

        QPainterPath hull;
        if (b.type == "cargo") {
            // Cargo : forme rectangulaire large
            hull.moveTo(0, -13);
            hull.lineTo(-7, -6);
            hull.lineTo(-7,  8);
            hull.lineTo(-3, 12);
            hull.lineTo( 3, 12);
            hull.lineTo( 7,  8);
            hull.lineTo( 7, -6);
            hull.closeSubpath();
        } else if (b.type == "chalutier") {
            // Chalutier : proue arrondie
            hull.moveTo(0, -14);
            hull.cubicTo(-4,-8, -6, 0, -5, 10);
            hull.lineTo(5, 10);
            hull.cubicTo(6, 0, 4,-8, 0,-14);
        } else {
            // Artisanal / palangrier : triangle effilé
            hull.moveTo(0, -12);
            hull.lineTo(-5, 8);
            hull.lineTo(0,  5);
            hull.lineTo(5,  8);
            hull.closeSubpath();
        }

        // Ombre portée
        p.translate(1.5, 1.5);
        p.fillPath(hull, QColor(0,0,0,60));
        p.translate(-1.5, -1.5);

        // Corps du navire
        QColor fillC = (i == m_selectedBoat) ? Qt::white : bc;
        p.fillPath(hull, fillC);
        p.setPen(QPen(bc.darker(160), 1.2));
        p.drawPath(hull);

        // Superstructure (petit rectangle sur le pont)
        p.fillRect(QRectF(-2.5, -4, 5, 5), bc.lighter(160));

        p.restore();

        // Étiquette nom + vitesse
        bool selected = (i == m_selectedBoat);
        if (selected) {
            // Fond glassmorphism pour navire sélectionné
            QRectF lbl(px.x()+14, px.y()-14, 100, 30);
            p.fillRect(lbl, QColor(15,23,42,200));
            p.setPen(QPen(bc, 1));
            p.drawRect(lbl);
            p.setPen(Qt::white);
            p.setFont(QFont("Segoe UI", 9, QFont::Bold));
            p.drawText(lbl.adjusted(4,2,0,0), Qt::AlignTop|Qt::AlignLeft, b.name);
            p.setFont(QFont("Segoe UI", 7));
            p.setPen(QColor(148,163,184));
            p.drawText(lbl.adjusted(4,14,0,0), Qt::AlignTop|Qt::AlignLeft,
                       QString("%1 kn  %2°").arg(b.speed,0,'f',1).arg((int)b.heading));
        } else {
            p.setPen(m_nightMode ? QColor(226,232,240) : QColor(15,23,42));
            p.setFont(QFont("Segoe UI", 8));
            p.drawText(QRectF(px.x()+12, px.y()-8, 90, 14), Qt::AlignLeft, b.name);
            p.setPen(m_nightMode ? QColor(148,163,184) : QColor(71,85,105));
            p.setFont(QFont("Segoe UI", 7));
            p.drawText(QRectF(px.x()+12, px.y()+4, 90, 12), Qt::AlignLeft,
                       QString("%1 kn").arg(b.speed,0,'f',1));
        }
    }
}

// ── drawCities ───────────────────────────────────────────────────────────────

void TunisiaMapWidget::drawCities(QPainter &p, const QRect &r)
{
    for (const City &c : m_cities) {
        QPointF px = toPixel(c.pos, r);
        if (!r.contains(px.toPoint())) continue;

        if (c.capital) {
            p.setBrush(QColor("#fbbf24"));
            p.setPen(QPen(QColor("#92400e"),1.5));
            double s = 8;
            QPainterPath star;
            for (int i = 0; i < 5; i++) {
                double a1 = -M_PI/2 + i*2*M_PI/5;
                double a2 = a1 + M_PI/5;
                QPointF o(px.x()+s*qCos(a1), px.y()+s*qSin(a1));
                QPointF in(px.x()+s*0.4*qCos(a2), px.y()+s*0.4*qSin(a2));
                if (i==0) star.moveTo(o); else star.lineTo(o);
                star.lineTo(in);
            }
            star.closeSubpath();
            p.drawPath(star);
        } else {
            p.setBrush(m_nightMode ? QColor("#334155") : QColor("#f8fafc"));
            p.setPen(QPen(m_nightMode ? QColor("#94a3b8") : QColor("#1e293b"), 1.5));
            p.drawEllipse(px, 5, 5);
        }
        p.setPen(m_nightMode ? QColor("#e2e8f0") : QColor("#0f172a"));
        p.setFont(QFont("Segoe UI", c.capital?9:8, c.capital?QFont::Bold:QFont::Normal));
        p.drawText(QRectF(px.x()+8, px.y()-8, 80, 16), Qt::AlignLeft, c.name);
    }
}

// ── drawWeatherOverlay ───────────────────────────────────────────────────────

void TunisiaMapWidget::drawWeatherOverlay(QPainter &p, const QRect &r)
{
    // Wind direction arrow in top-right of map
    QPointF center(r.right()-80, r.top()+30);
    double rad = qDegreesToRadians(m_weather.windDir);
    double len = 24;
    QPointF tip(center.x()+len*qSin(rad), center.y()-len*qCos(rad));

    p.setPen(QPen(QColor("#38bdf8"), 2));
    p.drawLine(center, tip);
    double a = qAtan2(tip.y()-center.y(), tip.x()-center.x());
    p.drawLine(tip, QPointF(tip.x()-8*qCos(a-0.4), tip.y()-8*qSin(a-0.4)));
    p.drawLine(tip, QPointF(tip.x()-8*qCos(a+0.4), tip.y()-8*qSin(a+0.4)));

    p.setPen(QColor("#38bdf8"));
    p.setFont(QFont("Segoe UI", 8));
    p.drawText(QRectF(center.x()-20, center.y()+28, 60, 14),
               Qt::AlignCenter,
               QString::number(m_weather.windSpeed,'f',0)+" km/h");
}


// ── drawCompass ──────────────────────────────────────────────────────────────

void TunisiaMapWidget::drawCompass(QPainter &p, const QRect &r)
{
    QPointF c(r.right()-48, r.top()+48);
    double s = 22;
    p.setBrush(QColor(255,255,255, m_nightMode?60:200));
    p.setPen(QPen(QColor("#94a3b8"),1));
    p.drawEllipse(c, s+4, s+4);

    QPainterPath north, south;
    north.moveTo(c.x(), c.y()-s); north.lineTo(c.x()-6,c.y()); north.lineTo(c.x()+6,c.y()); north.closeSubpath();
    south.moveTo(c.x(), c.y()+s); south.lineTo(c.x()-6,c.y()); south.lineTo(c.x()+6,c.y()); south.closeSubpath();
    p.fillPath(north, QColor("#ef4444"));
    p.fillPath(south, QColor(m_nightMode?"#334155":"#e2e8f0"));
    p.setPen(QPen(QColor("#64748b"),1)); p.drawPath(south);

    p.setPen(m_nightMode ? QColor("#e2e8f0") : QColor("#0f172a"));
    p.setFont(QFont("Segoe UI",8,QFont::Bold));
    p.drawText(QRectF(c.x()-6,c.y()-s-14,12,12), Qt::AlignCenter, "N");
    p.drawText(QRectF(c.x()-6,c.y()+s+2, 12,12), Qt::AlignCenter, "S");
}

// ── drawScale ────────────────────────────────────────────────────────────────

void TunisiaMapWidget::drawScale(QPainter &p, const QRect &r)
{
    int bw = r.width()*0.10, x=r.left()+16, y=r.bottom()-24;
    p.fillRect(x,    y, bw/2, 6, m_nightMode?QColor("#94a3b8"):QColor("#1e293b"));
    p.fillRect(x+bw/2,y,bw/2,6, m_nightMode?QColor("#475569"):QColor("#f8fafc"));
    p.setPen(m_nightMode?QColor("#94a3b8"):QColor("#0f172a"));
    p.setFont(QFont("Segoe UI",8));
    p.drawText(x, y+18, "0");
    p.drawText(x+bw-8, y+18, "100 km");
}

// ── drawLegend ───────────────────────────────────────────────────────────────

void TunisiaMapWidget::drawLegend(QPainter &p, const QRect &r)
{
    int x=r.left()+14, y=r.top()+14, w=160, h=130;

    // Fond glassmorphism
    p.setBrush(QColor(15,23,42, m_nightMode?210:190));
    p.setPen(QPen(QColor(255,255,255,30), 1));
    QPainterPath glass;
    glass.addRoundedRect(x, y, w, h, 10, 10);
    p.drawPath(glass);

    // Ligne de titre
    p.fillRect(x, y, w, 22, QColor(29,78,216,180));
    p.setPen(Qt::white);
    p.setFont(QFont("Segoe UI", 8, QFont::Bold));
    p.drawText(QRect(x+8, y+4, w-16, 16), Qt::AlignVCenter|Qt::AlignLeft, "ZONES DE PÊCHE");

    struct { const char *lbl; QColor col; } zones[] = {
        {"Libre",    QColor(16,185,129)},
        {"Restreint",QColor(245,158,11)},
        {"Interdit", QColor(239,68,68)}
    };
    for (int i = 0; i < 3; i++) {
        int iy = y + 30 + i*22;
        // Pastille colorée
        p.setBrush(zones[i].col);
        p.setPen(Qt::NoPen);
        p.drawEllipse(x+10, iy, 11, 11);
        p.setPen(QColor(226,232,240));
        p.setFont(QFont("Segoe UI", 8));
        p.drawText(x+28, iy+10, zones[i].lbl);
    }

    // Séparateur
    p.setPen(QPen(QColor(255,255,255,30), 1));
    p.drawLine(x+8, y+98, x+w-8, y+98);

    // Navires actifs
    p.setPen(QColor(148,163,184));
    p.setFont(QFont("Segoe UI", 7));
    p.drawText(x+8, y+112, QString("Navires actifs : %1").arg(m_boats.size()));
    p.drawText(x+8, y+124, QString("Zoom : x%1").arg(m_zoom,0,'f',1));
}

// ── drawRadar ────────────────────────────────────────────────────────────────

void TunisiaMapWidget::drawRadar(QPainter &p, const QRect &r)
{
    QPointF c(r.right()-52, r.bottom()-52);
    double  s = 38;

    // Background
    p.setBrush(QColor(0,20,10,200)); p.setPen(QPen(QColor("#10b981"),1));
    p.drawEllipse(c, s, s);

    // Grid rings
    p.setPen(QPen(QColor(16,185,129,80),0.5));
    for (double rs : {s*0.33, s*0.66}) p.drawEllipse(c, rs, rs);
    p.drawLine(QPointF(c.x()-s,c.y()), QPointF(c.x()+s,c.y()));
    p.drawLine(QPointF(c.x(),c.y()-s), QPointF(c.x(),c.y()+s));

    // Sweep line
    double sweepAngle = qDegreesToRadians(m_tick * 3.0);
    p.setPen(QPen(QColor("#10b981"),1.5));
    p.drawLine(c, QPointF(c.x()+s*qCos(sweepAngle), c.y()+s*qSin(sweepAngle)));

    // Sweep fade arc
    QConicalGradient cg(c, -m_tick*3);
    cg.setColorAt(0.0, QColor(16,185,129,120));
    cg.setColorAt(0.25,QColor(16,185,129,0));
    cg.setColorAt(1.0, QColor(16,185,129,0));
    p.setBrush(cg); p.setPen(Qt::NoPen);
    p.drawEllipse(c, s, s);

    // Boat blips
    for (const Boat &b : m_boats) {
        QPointF norm = geoToNorm(b.lat, b.lon);
        double bx = c.x() + (norm.x()-0.5)*2*s;
        double by = c.y() + (norm.y()-0.5)*2*s;
        QColor bc = b.inRestrictedZone ? QColor("#ef4444") : QColor("#10b981");
        p.setBrush(bc); p.setPen(Qt::NoPen);
        p.drawEllipse(QPointF(bx,by), 2.5, 2.5);
    }
}

// ── drawInfoPanel ────────────────────────────────────────────────────────────

void TunisiaMapWidget::drawInfoPanel(QPainter &p, const QRect &r)
{
    // Fond glassmorphism
    QLinearGradient bg(r.topLeft(), r.bottomLeft());
    bg.setColorAt(0.0, QColor(15,23,42,245));
    bg.setColorAt(1.0, QColor(8,14,28,255));
    p.fillRect(r, bg);

    // Ligne de séparation supérieure avec dégradé coloré
    QLinearGradient topLine(r.topLeft(), r.topRight());
    topLine.setColorAt(0.0, QColor(29,78,216,0));
    topLine.setColorAt(0.3, QColor(29,78,216,255));
    topLine.setColorAt(0.7, QColor(56,189,248,255));
    topLine.setColorAt(1.0, QColor(56,189,248,0));
    p.fillRect(r.left(), r.top(), r.width(), 2, topLine);

    if (m_selectedBoat >= 0 && m_selectedBoat < m_boats.size()) {
        const Boat &b = m_boats[m_selectedBoat];
        QColor bc = (b.type=="chalutier")  ? QColor(59,130,246)  :
                    (b.type=="palangrier") ? QColor(16,185,129)  :
                    (b.type=="cargo")      ? QColor(245,158,11)  : QColor(139,92,246);

        // Indicateur de type (barre colorée gauche)
        p.fillRect(r.left(), r.top()+2, 4, r.height()-2, bc);

        p.setFont(QFont("Segoe UI", 13, QFont::Bold));
        p.setPen(Qt::white);
        p.drawText(r.adjusted(16,6,0,0), Qt::AlignTop|Qt::AlignLeft, b.name);

        // Badge type
        QRectF badge(r.left()+16 + QFontMetrics(QFont("Segoe UI",13,QFont::Bold)).horizontalAdvance(b.name)+8,
                     r.top()+8, 70, 16);
        p.fillRect(badge, bc.darker(150));
        p.setPen(bc.lighter(150));
        p.setFont(QFont("Segoe UI", 7, QFont::Bold));
        p.drawText(badge, Qt::AlignCenter, b.type.toUpper());

        // Infos détaillées
        p.setFont(QFont("Segoe UI", 9));
        p.setPen(QColor(148,163,184));
        QString info = QString("MMSI: %1   |   Vitesse: %2 kn   |   Cap: %3°   |   Lat: %4°N   |   Lon: %5°E")
            .arg(b.mmsi).arg(b.speed,0,'f',1).arg((int)b.heading)
            .arg(b.lat,0,'f',4).arg(b.lon,0,'f',4);
        p.drawText(r.adjusted(16,32,0,0), Qt::AlignTop|Qt::AlignLeft, info);

        if (b.inRestrictedZone) {
            p.setPen(QColor(239,68,68));
            p.setFont(QFont("Segoe UI", 9, QFont::Bold));
            p.drawText(r.adjusted(16,54,0,0), Qt::AlignTop|Qt::AlignLeft, "⚠  ZONE RESTREINTE — Intervention requise");
        }
        return;
    }

    if (m_selectedZone >= 0 && m_selectedZone < m_zones.size()) {
        const FishingZone &z = m_zones[m_selectedZone];
        QColor sc = (z.status=="libre")    ? QColor(16,185,129) :
                    (z.status=="restreint")? QColor(245,158,11) : QColor(239,68,68);

        p.fillRect(r.left(), r.top()+2, 4, r.height()-2, sc);
        p.setFont(QFont("Segoe UI", 13, QFont::Bold));
        p.setPen(Qt::white);
        p.drawText(r.adjusted(16,6,0,0), Qt::AlignTop|Qt::AlignLeft, z.name);

        QRectF badge(r.left()+200, r.top()+8, 80, 16);
        p.fillRect(badge, sc.darker(160));
        p.setPen(sc.lighter(150));
        p.setFont(QFont("Segoe UI", 7, QFont::Bold));
        p.drawText(badge, Qt::AlignCenter, z.status.toUpper());

        p.setFont(QFont("Segoe UI", 9));
        p.setPen(QColor(148,163,184));
        p.drawText(r.adjusted(16,32,0,0), Qt::AlignTop|Qt::AlignLeft,
                   QString("Navires: %1   |   %2").arg(z.boatCount).arg(z.details.split("\n").join("   |   ")));
        return;
    }

    // Aucune sélection — message d'aide
    p.setPen(QColor(71,85,105));
    p.setFont(QFont("Segoe UI", 10));
    p.drawText(r, Qt::AlignCenter, "Cliquez sur une zone ou un navire pour afficher les détails");
}

// ── drawAlertBanner ──────────────────────────────────────────────────────────

void TunisiaMapWidget::drawAlertBanner(QPainter &p)
{
    int alpha = qMin(255, m_alertTick * 4);
    bool blink = (m_tick % 10) < 5;
    QColor bg(239, 68, 68, blink ? alpha : alpha/2);
    QRect banner(0, 0, width(), 36);
    p.fillRect(banner, bg);
    p.setPen(Qt::white);
    p.setFont(QFont("Segoe UI", 11, QFont::Bold));
    p.drawText(banner, Qt::AlignCenter, m_alertMsg);
}


// ── mousePressEvent ──────────────────────────────────────────────────────────

void TunisiaMapWidget::mousePressEvent(QMouseEvent *e)
{
    int sideW = (m_sidebarOpen && m_sidebar) ? m_sidebar->width() : 0;
    QRect mapR(sideW, 0, width()-sideW, height()-90);
    if (!mapR.contains(e->pos())) return;

    QPointF norm = toNorm(e->pos(), mapR);

    // Check boat first
    int bi = boatAt(norm, mapR);
    if (bi >= 0) {
        m_selectedBoat = bi;
        m_selectedZone = -1;
        update(); return;
    }

    // Then zone
    m_selectedBoat = -1;
    m_selectedZone = zoneAt(norm);
    update();
}

// ── mouseDoubleClickEvent — zoom on zone ─────────────────────────────────────

void TunisiaMapWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    int sideW = (m_sidebarOpen && m_sidebar) ? m_sidebar->width() : 0;
    QRect mapR(sideW, 0, width()-sideW, height()-90);
    if (!mapR.contains(e->pos())) return;

    QPointF norm = toNorm(e->pos(), mapR);
    int zi = zoneAt(norm);
    if (zi >= 0) {
        // Zoom in and center on zone
        m_zoom = qMin(m_zoom * 1.8, 5.0);
        m_pan.setX(0.5 - m_zones[zi].center.x() * m_zoom);
        m_pan.setY(0.5 - m_zones[zi].center.y() * m_zoom);
    } else {
        // Double-click on sea: reset zoom
        m_zoom = 1.0;
        m_pan  = {0, 0};
    }
    update();
}

// ── wheelEvent — zoom ────────────────────────────────────────────────────────

void TunisiaMapWidget::wheelEvent(QWheelEvent *e)
{
    double factor = e->angleDelta().y() > 0 ? 1.15 : 0.87;
    m_zoom = qBound(0.5, m_zoom * factor, 6.0);
    update();
}

// ── contextMenuEvent — change zone status ────────────────────────────────────

void TunisiaMapWidget::contextMenuEvent(QContextMenuEvent *e)
{
    int sideW = (m_sidebarOpen && m_sidebar) ? m_sidebar->width() : 0;
    QRect mapR(sideW, 0, width()-sideW, height()-90);
    if (!mapR.contains(e->pos())) return;

    QPointF norm = toNorm(e->pos(), mapR);
    int zi = zoneAt(norm);
    if (zi < 0) return;

    QMenu menu(this);
    menu.setStyleSheet(
        "QMenu{background:#1e293b;color:#e2e8f0;border:1px solid #334155;border-radius:8px;}"
        "QMenu::item{padding:8px 20px;font-size:13px;}"
        "QMenu::item:selected{background:#1d4ed8;border-radius:4px;}");

    menu.addSection("Zone : " + m_zones[zi].name);
    auto *aLibre     = menu.addAction("🟢  Marquer Libre");
    auto *aRestreint = menu.addAction("🟡  Marquer Restreint");
    auto *aInterdit  = menu.addAction("🔴  Marquer Interdit");

    QAction *chosen = menu.exec(e->globalPos());
    if      (chosen == aLibre)     m_zones[zi].status = "libre";
    else if (chosen == aRestreint) m_zones[zi].status = "restreint";
    else if (chosen == aInterdit)  m_zones[zi].status = "interdit";
    update();
}
