#include "gestioncaptures.h"
#include "TunisiaMapWidget.h"
#include "BarriereWidget.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QSet>
#include <QMap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QTextStream>
#include <QDialog>
#include <QScrollArea>
#include <QFrame>
#include <QPainter>
#include <QPainterPath>
#include <QPdfWriter>
#include <QPageSize>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QGraphicsDropShadowEffect>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QTimer>
#include <QEnterEvent>
#include <algorithm>
#include <cmath>

// ═══════════════════════════════════════════════════════════════════════════════
//  PALETTE & CONSTANTS
// ═══════════════════════════════════════════════════════════════════════════════

static const QColor C_BG       { "#f0f4f8" };
static const QColor C_SURFACE  { "#ffffff" };
static const QColor C_BORDER   { "#e2e8f0" };
static const QColor C_TEXT     { "#0f172a" };
static const QColor C_MUTED    { "#64748b" };
static const QColor C_BLUE     { "#2563eb" };
static const QColor C_GREEN    { "#059669" };
static const QColor C_AMBER    { "#d97706" };
static const QColor C_PURPLE   { "#7c3aed" };
static const QColor C_RED      { "#dc2626" };
static const QColor C_NAVY     { "#0a1628" };

static const QStringList kAvatarColors = {
    "#3b82f6","#10b981","#f59e0b","#ef4444",
    "#8b5cf6","#06b6d4","#f97316","#ec4899"
};

static QString avatarColor(const QString &name) {
    if (name.isEmpty()) return kAvatarColors[0];
    return kAvatarColors[qAbs(name.at(0).unicode()) % kAvatarColors.size()];
}

// ═══════════════════════════════════════════════════════════════════════════════
//  AnimatedButton
// ═══════════════════════════════════════════════════════════════════════════════

AnimatedButton::AnimatedButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent)
{
    setCursor(Qt::PointingHandCursor);
    setMinimumHeight(40);
    m_baseColor  = C_BLUE;
    m_hoverColor = C_BLUE.lighter(115);
    m_glowColor  = C_BLUE;
}

void AnimatedButton::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRectF r = rect().adjusted(1,1,-1,-1);
    if (m_pressed) r = r.adjusted(0,2,0,2);

    // glow halo
    if (m_glow > 0.01) {
        QColor glow = m_glowColor;
        glow.setAlphaF(m_glow * 0.35);
        p.setBrush(Qt::NoBrush);
        for (int i = 4; i >= 1; i--) {
            QPen pen(glow, i * 2.5);
            p.setPen(pen);
            p.drawRoundedRect(r.adjusted(-i,-i,i,i), 11+i, 11+i);
        }
    }

    // shadow
    if (!m_pressed) {
        QColor sh(0,0,0,40);
        p.setBrush(sh); p.setPen(Qt::NoPen);
        p.drawRoundedRect(r.adjusted(0,3,0,3), 10, 10);
    }

    // body gradient
    QLinearGradient grad(r.topLeft(), r.bottomLeft());
    QColor top = m_glow > 0.01 ? m_hoverColor : m_baseColor;
    grad.setColorAt(0, top.lighter(108));
    grad.setColorAt(1, top.darker(108));
    p.setBrush(grad);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(r, 10, 10);

    // shine
    QLinearGradient shine(r.topLeft(), QPointF(r.left(), r.top()+r.height()*0.45));
    shine.setColorAt(0, QColor(255,255,255,55));
    shine.setColorAt(1, QColor(255,255,255,0));
    p.setBrush(shine); p.drawRoundedRect(r, 10, 10);

    // text
    p.setPen(Qt::white);
    p.setFont(font());
    p.drawText(r, Qt::AlignCenter, text());
}

void AnimatedButton::enterEvent(QEnterEvent *e) {
    QPushButton::enterEvent(e);
    auto *anim = new QPropertyAnimation(this, "glowOpacity", this);
    anim->setDuration(180); anim->setEndValue(1.0);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void AnimatedButton::leaveEvent(QEvent *e) {
    QPushButton::leaveEvent(e);
    auto *anim = new QPropertyAnimation(this, "glowOpacity", this);
    anim->setDuration(220); anim->setEndValue(0.0);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void AnimatedButton::mousePressEvent(QMouseEvent *e) {
    m_pressed = true; update();
    QPushButton::mousePressEvent(e);
}

void AnimatedButton::mouseReleaseEvent(QMouseEvent *e) {
    m_pressed = false; update();
    QPushButton::mouseReleaseEvent(e);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  PulseLabel
// ═══════════════════════════════════════════════════════════════════════════════

PulseLabel::PulseLabel(const QString &text, QWidget *parent)
    : QLabel(text, parent), m_color(C_BLUE) {}

void PulseLabel::startPulse() {
    auto *anim = new QPropertyAnimation(this, "pulse", this);
    anim->setDuration(600);
    anim->setKeyValueAt(0,   0.0);
    anim->setKeyValueAt(0.4, 1.0);
    anim->setKeyValueAt(1,   0.0);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void PulseLabel::paintEvent(QPaintEvent *e) {
    if (m_pulse > 0.01) {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        QColor c = m_color; c.setAlphaF(m_pulse * 0.18);
        p.setBrush(c); p.setPen(Qt::NoPen);
        p.drawRoundedRect(rect(), 6, 6);
    }
    QLabel::paintEvent(e);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  STATIC HELPERS
// ═══════════════════════════════════════════════════════════════════════════════

static QPixmap makeAvatar(const QString &name, const QString &status = QString()) {
    QPixmap pix(40, 40);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    // outer ring
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(QColor(avatarColor(name)).lighter(140), 1.5));
    p.drawEllipse(1,1,37,37);

    // circle
    p.setBrush(QColor(avatarColor(name)));
    p.setPen(Qt::NoPen);
    p.drawEllipse(4,4,32,32);

    // initials
    p.setPen(Qt::white);
    QString initials;
    QStringList parts = name.split(' ', Qt::SkipEmptyParts);
    if (parts.size() >= 2)
        initials = parts[0].left(1).toUpper() + parts[1].left(1).toUpper();
    else if (!name.isEmpty())
        initials = name.left(2).toUpper();
    p.setFont(QFont("Segoe UI", 10, QFont::Bold));
    p.drawText(QRect(4,4,32,32), Qt::AlignCenter, initials);

    // status dot
    if (!status.isEmpty()) {
        QString dot = "#94a3b8";
        if      (status == "Validé")      dot = "#10b981";
        else if (status == "En transit")  dot = "#3b82f6";
        else if (status == "Livré")       dot = "#8b5cf6";
        else if (status == "En attente")  dot = "#ef4444";
        else if (status == "Enregistré")  dot = "#f59e0b";
        p.setBrush(QColor(dot));
        p.setPen(QPen(Qt::white, 2));
        p.drawEllipse(26,26,12,12);
    }
    p.end();
    return pix;
}

static QWidget *makeQtyBar(int qty, int maxQty) {
    double ratio = maxQty > 0 ? qMin(1.0, (double)qty / maxQty) : 0.0;
    QString color = ratio < 0.33 ? "#10b981" : ratio < 0.66 ? "#f59e0b" : "#ef4444";

    QPixmap pix(120, 32);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    p.setPen(QColor("#1e293b"));
    p.setFont(QFont("Segoe UI", 9, QFont::Bold));
    p.drawText(QRect(0,0,65,14), Qt::AlignLeft|Qt::AlignVCenter, QString::number(qty)+" kg");
    p.setPen(QColor("#94a3b8"));
    p.setFont(QFont("Segoe UI", 8));
    p.drawText(QRect(65,0,55,14), Qt::AlignRight|Qt::AlignVCenter, "/"+QString::number(maxQty));

    // track
    p.setBrush(QColor("#e2e8f0")); p.setPen(Qt::NoPen);
    p.drawRoundedRect(0,18,120,7,3,3);

    // fill with gradient
    int fw = qRound(120 * ratio);
    if (fw > 0) {
        QLinearGradient g(0,0,fw,0);
        g.setColorAt(0, QColor(color).lighter(120));
        g.setColorAt(1, QColor(color));
        p.setBrush(g);
        p.drawRoundedRect(0,18,fw,7,3,3);
    }
    p.end();

    auto *lbl = new QLabel;
    lbl->setPixmap(pix);
    lbl->setFixedSize(120,32);
    auto *w = new QWidget;
    auto *lay = new QHBoxLayout(w);
    lay->setContentsMargins(4,2,4,2);
    lay->addWidget(lbl);
    return w;
}

static QString scrollbarQss() {
    return
        "QScrollBar:vertical{background:#f1f5f9;width:6px;border-radius:3px;margin:0;}"
        "QScrollBar::handle:vertical{background:#cbd5e1;border-radius:3px;min-height:30px;}"
        "QScrollBar::handle:vertical:hover{background:#94a3b8;}"
        "QScrollBar::add-line:vertical,QScrollBar::sub-line:vertical{height:0;}"
        "QScrollBar:horizontal{background:#f1f5f9;height:6px;border-radius:3px;margin:0;}"
        "QScrollBar::handle:horizontal{background:#cbd5e1;border-radius:3px;min-width:30px;}"
        "QScrollBar::handle:horizontal:hover{background:#94a3b8;}"
        "QScrollBar::add-line:horizontal,QScrollBar::sub-line:horizontal{width:0;}";
}

static void fadeIn(QWidget *w, int ms = 200) {
    auto *eff = new QGraphicsOpacityEffect(w);
    w->setGraphicsEffect(eff);
    auto *anim = new QPropertyAnimation(eff, "opacity", w);
    anim->setDuration(ms);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    // Remove effect after animation so it doesn't block child rendering
    QObject::connect(anim, &QPropertyAnimation::finished, w, [w](){ w->setGraphicsEffect(nullptr); });
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}



static QGraphicsDropShadowEffect *makeShadow(int blur=18, int dy=4, int alpha=40) {
    auto *s = new QGraphicsDropShadowEffect;
    s->setBlurRadius(blur);
    s->setOffset(0, dy);
    s->setColor(QColor(0,0,0,alpha));
    return s;
}

static QPushButton *makeBtn(const QString &text, const QString &bg, const QString &hover) {
    auto *b = new QPushButton(text);
    b->setMinimumHeight(40);
    b->setCursor(Qt::PointingHandCursor);
    b->setStyleSheet(QString(
        "QPushButton{"
        "  background:qlineargradient(x1:0,y1:0,x2:0,y2:1,stop:0 %1,stop:1 %2);"
        "  color:white;border:none;border-radius:10px;"
        "  padding:0 18px;font-weight:700;font-size:13px;"
        "  border-bottom:3px solid rgba(0,0,0,.20);}"
        "QPushButton:hover{"
        "  background:qlineargradient(x1:0,y1:0,x2:0,y2:1,stop:0 %2,stop:1 %1);}"
        "QPushButton:pressed{padding-top:3px;border-bottom:1px solid rgba(0,0,0,.20);}")
        .arg(bg, hover));
    return b;
}

static QPushButton *makeIconBtn(const QString &icon, const QString &tip,
                                const QString &bg, const QString &hover) {
    auto *b = new QPushButton(icon);
    b->setFixedSize(36,36);
    b->setToolTip(tip);
    b->setCursor(Qt::PointingHandCursor);
    b->setStyleSheet(QString(
        "QPushButton{background:%1;color:white;border:none;border-radius:9px;"
        "  font-size:15px;border-bottom:2px solid rgba(0,0,0,.18);}"
        "QPushButton:hover{background:%2;}"
        "QPushButton:pressed{padding-top:2px;border-bottom:0px;}").arg(bg,hover));
    return b;
}

static QPushButton *makeFilterChip(const QString &label, const QString &color) {
    auto *b = new QPushButton(label);
    b->setCheckable(true);
    b->setCursor(Qt::PointingHandCursor);
    b->setFixedHeight(30);
    b->setStyleSheet(QString(
        "QPushButton{background:#f1f5f9;color:#475569;border:2px solid #e2e8f0;"
        "  border-radius:15px;padding:0 14px;font-size:12px;font-weight:600;}"
        "QPushButton:hover{background:#e2e8f0;color:#1e293b;}"
        "QPushButton:checked{background:%1;color:white;border-color:%1;"
        "  border-bottom:3px solid rgba(0,0,0,.18);}").arg(color));
    return b;
}




// ═══════════════════════════════════════════════════════════════════════════════
//  CONSTRUCTOR / DESTRUCTOR
// ═══════════════════════════════════════════════════════════════════════════════

GestionCaptures::GestionCaptures(QWidget *parent) : QWidget(parent)
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    m_jsonPath = QDir(dataDir).filePath("capture.json");
    configurerInterface();
    chargerCaptures();
}

GestionCaptures::~GestionCaptures() {}

// ═══════════════════════════════════════════════════════════════════════════════
//  configurerInterface
// ═══════════════════════════════════════════════════════════════════════════════

void GestionCaptures::configurerInterface()
{
    setStyleSheet(
        "GestionCaptures{background:" + C_BG.name() + ";}"
        "GestionCaptures QLabel{background:transparent;}"
        + scrollbarQss());

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0,0,0,0);
    root->setSpacing(0);

    // ── HEADER BANNER ────────────────────────────────────────────────────────
    auto *banner = new QFrame;
    banner->setFixedHeight(72);
    banner->setStyleSheet(
        "QFrame{background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
        "stop:0 #060f1e,stop:0.45 #0f2d6b,stop:1 #1a56db);"
        "border-bottom:1px solid #1e3a8a;}");
    banner->setGraphicsEffect(makeShadow(24, 6, 60));

    auto *bannerLay = new QHBoxLayout(banner);
    bannerLay->setContentsMargins(22,0,18,0);
    bannerLay->setSpacing(10);

    // icon + title
    auto *titleIcon = new QLabel("🐟");
    titleIcon->setStyleSheet("font-size:26px;background:transparent;");
    auto *titleLbl = new QLabel("Gestion des capture");
    titleLbl->setStyleSheet(
        "font-size:20px;font-weight:900;color:white;"
        "letter-spacing:.4px;background:transparent;");
    bannerLay->addWidget(titleIcon);
    bannerLay->addWidget(titleLbl);
    bannerLay->addStretch();

    // live clock
    m_clockLabel = new QLabel;
    m_clockLabel->setStyleSheet(
        "color:rgba(255,255,255,.65);font-size:12px;font-weight:600;"
        "background:rgba(255,255,255,.08);border-radius:8px;padding:4px 10px;");
    auto updateClock = [this]() {
        m_clockLabel->setText(QDateTime::currentDateTime().toString("ddd dd MMM  HH:mm:ss"));
    };
    updateClock();
    m_clockTimer = new QTimer(this);
    m_clockTimer->setInterval(1000);
    connect(m_clockTimer, &QTimer::timeout, this, updateClock);
    m_clockTimer->start();
    bannerLay->addWidget(m_clockLabel);

    // action buttons
    btnAjouter = new AnimatedButton("➕  Ajouter", banner);
    btnAjouter->setMinimumWidth(130);
    btnAjouter->m_baseColor  = C_GREEN;
    btnAjouter->m_hoverColor = C_GREEN.lighter(115);
    btnAjouter->m_glowColor  = C_GREEN;
    bannerLay->addWidget(btnAjouter);

    auto *btnStats    = makeIconBtn("📊","Statistiques",         "rgba(255,255,255,.15)","rgba(255,255,255,.28)");
    auto *btnTrier    = makeIconBtn("🔃","Trier par quantité",   "rgba(255,255,255,.15)","rgba(255,255,255,.28)");
    auto *btnCarte    = makeIconBtn("🗺️","Carte maritime",       "rgba(255,255,255,.15)","rgba(255,255,255,.28)");
    auto *btnBarriere = makeIconBtn("🏗️","Carte du port",       "rgba(255,255,255,.15)","rgba(255,255,255,.28)");
    auto *btnExporter = makeIconBtn("📄","Exporter PDF",         "rgba(255,255,255,.15)","rgba(255,255,255,.28)");
    bannerLay->addWidget(btnStats);
    bannerLay->addWidget(btnTrier);
    bannerLay->addWidget(btnCarte);
    bannerLay->addWidget(btnBarriere);
    bannerLay->addWidget(btnExporter);
    root->addWidget(banner);

    // ── CONTENT AREA ─────────────────────────────────────────────────────────
    auto *content = new QWidget;
    content->setStyleSheet("background:" + C_BG.name() + ";");
    auto *cLay = new QVBoxLayout(content);
    cLay->setContentsMargins(18,14,18,14);
    cLay->setSpacing(10);

    // ── SEARCH ROW ───────────────────────────────────────────────────────────
    auto *searchCard = new QFrame;
    searchCard->setObjectName("searchCard");
    searchCard->setStyleSheet(
        "#searchCard{background:white;border-radius:14px;border:1px solid #e2e8f0;}"
        "#searchCard QLabel{background:transparent;}"
        "#searchCard QLineEdit{background:transparent;border:none;font-size:13px;color:#1e293b;}");
    searchCard->setGraphicsEffect(makeShadow(12,2,18));
    auto *searchLay = new QHBoxLayout(searchCard);
    searchLay->setContentsMargins(14,8,14,8);
    searchLay->setSpacing(10);

    auto *searchIcon = new QLabel("🔍");
    searchIcon->setStyleSheet("font-size:16px;background:transparent;");
    lineRecherche = new QLineEdit;
    lineRecherche->setPlaceholderText("Rechercher par ID, navire, type de poisson…");
    lineRecherche->setMinimumWidth(300);

    labelCompteur = new QLabel("0 résultat(s)");
    labelCompteur->setStyleSheet(
        "color:#64748b;font-size:12px;font-weight:600;"
        "background:#f1f5f9;border-radius:10px;padding:3px 10px;");

    searchLay->addWidget(searchIcon);
    searchLay->addWidget(lineRecherche, 1);
    searchLay->addWidget(labelCompteur);

    cLay->addWidget(searchCard);

    // ── KPI CARDS ────────────────────────────────────────────────────────────
    auto *kpiRow = new QHBoxLayout; kpiRow->setSpacing(12);

    struct KpiDef { QString icon; QString val; QString lbl; QColor accent; QLabel **ref; };
    QList<KpiDef> kpis = {
        {"📦", "0",    "capture totales", C_BLUE,   &kpiTotal},
        {"⚖️", "0 kg", "poids total",      C_GREEN,  &kpiKg},
        {"🚢", "0",    "navires actifs",   C_AMBER,  &kpiNavires},
        {"📈", "0 kg", "moyenne / capture",C_PURPLE, &kpiMoy},
    };

    for (auto &k : kpis) {
        auto *card = new QFrame;
        card->setObjectName("kpiCard");
        card->setStyleSheet(QString(
            "#kpiCard{background:white;border-radius:14px;"
            "border-left:4px solid %1;"
            "border-top:1px solid #e2e8f0;"
            "border-right:1px solid #e2e8f0;"
            "border-bottom:1px solid #e2e8f0;}"
            "#kpiCard QLabel{background:transparent;}").arg(k.accent.name()));
        card->setGraphicsEffect(makeShadow(14,3,22));
        m_kpiCards.append(card);

        auto *h = new QHBoxLayout(card);
        h->setContentsMargins(14,12,14,12); h->setSpacing(12);

        auto *ico = new QLabel(k.icon);
        ico->setStyleSheet(QString(
            "font-size:28px;background:transparent;color:%1;").arg(k.accent.name()));

        auto *vl = new QVBoxLayout; vl->setSpacing(2);
        *k.ref = new QLabel(k.val);
        (*k.ref)->setStyleSheet(
            "font-size:22px;font-weight:900;color:#0a1628;background:transparent;");
        auto *ll = new QLabel(k.lbl);
        ll->setStyleSheet("font-size:11px;color:#64748b;font-weight:600;background:transparent;");
        vl->addWidget(*k.ref); vl->addWidget(ll);
        h->addWidget(ico); h->addLayout(vl);
        kpiRow->addWidget(card);
    }
    cLay->addLayout(kpiRow);

    // ── FILTER CHIPS ─────────────────────────────────────────────────────────
    auto *chipRow = new QHBoxLayout; chipRow->setSpacing(6);
    auto *chipLbl = new QLabel("Filtrer :");
    chipLbl->setStyleSheet("color:#64748b;font-weight:700;font-size:12px;background:transparent;");
    auto *chipTous      = makeFilterChip("Tous",        "#2563eb");
    auto *chipEnreg     = makeFilterChip("Enregistré",  "#92400e");
    auto *chipValide    = makeFilterChip("Validé",      "#10b981");
    auto *chipTransit   = makeFilterChip("En transit",  "#f59e0b");
    auto *chipLivre     = makeFilterChip("Livré",       "#8b5cf6");
    auto *chipAttente   = makeFilterChip("En attente",  "#ef4444");
    chipTous->setChecked(true);
    chipRow->addWidget(chipLbl);
    chipRow->addWidget(chipTous);
    chipRow->addWidget(chipEnreg);
    chipRow->addWidget(chipValide);
    chipRow->addWidget(chipTransit);
    chipRow->addWidget(chipLivre);
    chipRow->addWidget(chipAttente);
    chipRow->addStretch();
    cLay->addLayout(chipRow);

    auto chips   = QList<QPushButton*>{chipTous,chipEnreg,chipValide,chipTransit,chipLivre,chipAttente};
    auto statuts = QList<QString>{"","Enregistré","Validé","En transit","Livré","En attente"};
    for (int i = 0; i < chips.size(); i++) {
        connect(chips[i], &QPushButton::clicked, this, [this,chips,statuts,i]() {
            for (auto *c : chips) c->setChecked(false);
            chips[i]->setChecked(true);
            filtrerParStatut(statuts[i]);
        });
    }

            // ── TABLE STACK ──────────────────────────────────────────────────────────
    stackTable = new QStackedWidget;
    configurerTableauBord();
    stackTable->addWidget(tableCaptures);

    // empty state page
    auto *emptyPage = new QWidget;
    emptyPage->setStyleSheet(
        "background:white;border-radius:14px;border:1px solid #e2e8f0;");
    auto *emptyLay = new QVBoxLayout(emptyPage);
    emptyLay->setAlignment(Qt::AlignCenter);
    auto *emptyIco = new QLabel("🎣");
    emptyIco->setStyleSheet("font-size:64px;background:transparent;");
    emptyIco->setAlignment(Qt::AlignCenter);
    auto *emptyT = new QLabel("Aucune capture enregistrée");
    emptyT->setStyleSheet(
        "font-size:18px;font-weight:800;color:#1e293b;background:transparent;");
    emptyT->setAlignment(Qt::AlignCenter);
    auto *emptyS = new QLabel("Cliquez sur ➕ Ajouter pour enregistrer votre première capture.");
    emptyS->setStyleSheet("font-size:13px;color:#64748b;background:transparent;");
    emptyS->setAlignment(Qt::AlignCenter);
    emptyLay->addWidget(emptyIco); emptyLay->addWidget(emptyT); emptyLay->addWidget(emptyS);
    stackTable->addWidget(emptyPage);
    cLay->addWidget(stackTable, 1);

        // ── FOOTER TOTAUX ────────────────────────────────────────────────────────
    labelTotaux = new QLabel;
    labelTotaux->setStyleSheet(
        "background:white;border:1px solid #e2e8f0;border-radius:10px;"
        "padding:7px 16px;color:#1e293b;font-weight:700;font-size:12px;");
    labelTotaux->setGraphicsEffect(makeShadow(8,1,12));
    cLay->addWidget(labelTotaux);
    root->addWidget(content, 1);

    // ── CONNECTIONS ──────────────────────────────────────────────────────────
    connect(btnAjouter,    &QPushButton::clicked, this, &GestionCaptures::onAjouter);
    connect(btnStats,      &QPushButton::clicked, this, &GestionCaptures::onStatistiques);
    connect(btnTrier,      &QPushButton::clicked, this, &GestionCaptures::onTrierQuantite);
    connect(btnCarte,      &QPushButton::clicked, this, &GestionCaptures::onCarte);
    connect(btnExporter,   &QPushButton::clicked, this, &GestionCaptures::exporterCSV);
    connect(lineRecherche, &QLineEdit::textChanged, this, &GestionCaptures::rechercherCapture);

    connect(btnBarriere, &QPushButton::clicked, this, [this]() {
        QDialog dlg(this);
        dlg.setWindowTitle("Carte du Port");
        dlg.setMinimumSize(900, 700);
        dlg.setStyleSheet("background:#0f172a;");
        auto *vbox = new QVBoxLayout(&dlg);
        vbox->setContentsMargins(0,0,0,0);
        vbox->setSpacing(0);

        // Titre
        auto *title = new QLabel("🏗️  Carte du Port Maritime");
        title->setStyleSheet("background:#1e293b;color:white;font-size:15px;font-weight:700;"
                             "padding:10px 16px;border-bottom:2px solid #1d4ed8;");
        vbox->addWidget(title);

        // Image du port (zoomable via scroll)
        auto *scroll = new QScrollArea;
        scroll->setStyleSheet("background:#0f172a;border:none;");
        scroll->setAlignment(Qt::AlignCenter);
        auto *imgLbl = new QLabel;
        QPixmap pix(":/images/port_aerial.jpg");
        imgLbl->setPixmap(pix.scaled(860, 620, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        imgLbl->setAlignment(Qt::AlignCenter);
        scroll->setWidget(imgLbl);
        scroll->setWidgetResizable(false);
        vbox->addWidget(scroll, 1);

        // Barre inférieure
        auto *bar = new QWidget;
        bar->setStyleSheet("background:#1e293b;");
        auto *barLay = new QHBoxLayout(bar);
        barLay->setContentsMargins(12,6,12,6);
        auto *info = new QLabel("Vue aérienne — Port Maritime  |  Quais & zones d'amarrage");
        info->setStyleSheet("color:#94a3b8;font-size:11px;");
        auto *btnClose = new QPushButton("Fermer");
        btnClose->setStyleSheet("background:#1d4ed8;color:white;border:none;border-radius:6px;"
                                "padding:6px 18px;font-weight:700;");
        connect(btnClose, &QPushButton::clicked, &dlg, &QDialog::accept);
        barLay->addWidget(info, 1);
        barLay->addWidget(btnClose);
        vbox->addWidget(bar);

        dlg.exec();
    });

}


// ═══════════════════════════════════════════════════════════════════════════════
//  configurerTableauBord
// ═══════════════════════════════════════════════════════════════════════════════

void GestionCaptures::configurerTableauBord()
{
    tableCaptures = new QTableWidget;
    tableCaptures->setColumnCount(7);
    tableCaptures->setHorizontalHeaderLabels(
        {"", "ID Capture", "Navire", "Date", "Type", "Quantité", "Actions"});

    auto *hdr = tableCaptures->horizontalHeader();
    hdr->setSectionResizeMode(QHeaderView::Stretch);
    hdr->setSectionResizeMode(0, QHeaderView::Fixed); tableCaptures->setColumnWidth(0, 46);
    hdr->setSectionResizeMode(3, QHeaderView::Fixed); tableCaptures->setColumnWidth(3, 92);
    hdr->setSectionResizeMode(5, QHeaderView::Fixed); tableCaptures->setColumnWidth(5, 130);
    hdr->setSectionResizeMode(6, QHeaderView::Fixed); tableCaptures->setColumnWidth(6, 108);

    tableCaptures->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableCaptures->setSelectionMode(QAbstractItemView::SingleSelection);
    tableCaptures->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableCaptures->verticalHeader()->setVisible(false);
    tableCaptures->setAlternatingRowColors(true);
    tableCaptures->setShowGrid(false);
    tableCaptures->setStyleSheet(
        "QTableWidget{"
        "  background:white;border:1px solid #e2e8f0;border-radius:14px;"
        "  alternate-background-color:#f8fafc;outline:none;}"
        "QTableWidget::item{padding:2px 8px;border-bottom:1px solid #f1f5f9;}"
        "QTableWidget::item:selected{background:#dbeafe;color:#1e293b;}"
        "QTableWidget::item:hover{background:#eff6ff;}"
        "QHeaderView::section{"
        "  background:qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "  stop:0 #1e3a8a,stop:1 #1d4ed8);"
        "  color:white;padding:8px;border:none;"
        "  font-weight:700;font-size:12px;letter-spacing:.3px;}"
        "QHeaderView::section:first{border-top-left-radius:12px;}"
        "QHeaderView::section:last{border-top-right-radius:12px;}");
    tableCaptures->setGraphicsEffect(makeShadow(16,3,20));
}

// ═══════════════════════════════════════════════════════════════════════════════
//  actualiserTable
// ═══════════════════════════════════════════════════════════════════════════════

void GestionCaptures::actualiserTable()
{
    if (!tableCaptures) return;
    tableCaptures->setRowCount(0);

    m_maxQuantity = 1;
    for (const Capture &c : listeCaptures)
        if (c.quantity > m_maxQuantity) m_maxQuantity = c.quantity;

    for (const Capture &c : listeCaptures) {
        if (!m_filtreStatut.isEmpty() && c.status != m_filtreStatut) continue;

        int row = tableCaptures->rowCount();
        tableCaptures->insertRow(row);
        tableCaptures->setRowHeight(row, 40);

        auto item = [](const QString &t) {
            auto *i = new QTableWidgetItem(t);
            i->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
            return i;
        };

        // col 0: avatar
        auto *avatarLbl = new QLabel;
        avatarLbl->setPixmap(
            makeAvatar(c.shipName, c.status)
            .scaled(30,30,Qt::KeepAspectRatio,Qt::SmoothTransformation));
        avatarLbl->setToolTip(c.status);
        avatarLbl->setAlignment(Qt::AlignCenter);
        auto *avatarW = new QWidget;
        auto *al = new QHBoxLayout(avatarW);
        al->setContentsMargins(4,4,4,4); al->addWidget(avatarLbl);
        tableCaptures->setCellWidget(row, 0, avatarW);

        // col 1: ID (truncated, monospace)
        auto *idItem = item("CAP-" + c.captureId.rightJustified(3, '0'));
        idItem->setToolTip(c.captureId);
        idItem->setData(Qt::UserRole, c.captureId);
        idItem->setForeground(QColor("#94a3b8"));
        idItem->setFont(QFont("Consolas", 10));
        tableCaptures->setItem(row, 1, idItem);

        // col 2: Navire (bold)
        auto *shipItem = item(c.shipName);
        shipItem->setFont(QFont("Segoe UI", 12, QFont::Bold));
        tableCaptures->setItem(row, 2, shipItem);

        // col 3: Date
        tableCaptures->setItem(row, 3, item(c.captureDate.toString("dd/MM/yyyy")));

        // col 4: Type
        tableCaptures->setItem(row, 4, item(c.fishIcon + "  " + c.fishType));

        // col 5: Qty bar
        tableCaptures->setCellWidget(row, 5, makeQtyBar(c.quantity, m_maxQuantity));

        // col 6: Actions
        auto *actW = new QWidget;
        auto *actL = new QHBoxLayout(actW);
        actL->setContentsMargins(4,4,4,4); actL->setSpacing(4);
        auto *btnC = makeIconBtn("👁️","Consulter","#64748b","#475569");
        auto *btnE = makeIconBtn("✏️","Modifier",  "#2563eb","#1d4ed8");
        auto *btnD = makeIconBtn("🗑️","Supprimer", "#dc2626","#b91c1c");
        btnC->setFixedSize(30,30); btnE->setFixedSize(30,30); btnD->setFixedSize(30,30);
        actL->addWidget(btnC); actL->addWidget(btnE); actL->addWidget(btnD);
        actL->addStretch();
        tableCaptures->setCellWidget(row, 6, actW);

        QString cid = c.captureId;
        connect(btnC, &QPushButton::clicked, this, [this,cid]() {
            for (int i = 0; i < listeCaptures.size(); i++)
                if (listeCaptures[i].captureId == cid) { ouvrirDialog(&listeCaptures[i], true); return; }
        });
        connect(btnE, &QPushButton::clicked, this, [this,cid]() {
            for (int i = 0; i < listeCaptures.size(); i++)
                if (listeCaptures[i].captureId == cid) { ouvrirDialog(&listeCaptures[i], false); return; }
        });
        connect(btnD, &QPushButton::clicked, this, [this,cid]() {
            int idx = -1;
            for (int k = 0; k < listeCaptures.size(); k++)
                if (listeCaptures[k].captureId == cid) { idx = k; break; }
            if (idx < 0) return;
            QMessageBox mb(this);
            mb.setWindowTitle("Confirmer la suppression");
            mb.setIcon(QMessageBox::Warning);
            mb.setText(QString(
                "<b>Supprimer cette capture ?</b><br><br>"
                "Navire : <b>%1</b> — %2 — %3 kg<br>"
                "<span style='color:#94a3b8;font-size:11px;'>%4</span>")
                .arg(listeCaptures[idx].shipName,
                     listeCaptures[idx].fishType,
                     QString::number(listeCaptures[idx].quantity),
                     cid));
            mb.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
            mb.setDefaultButton(QMessageBox::Cancel);
            mb.button(QMessageBox::Yes)->setText("Supprimer");
            if (mb.exec() != QMessageBox::Yes) return;
            QSqlQuery q;
            q.prepare("DELETE FROM capture WHERE ID_CAPTURE=:id");
            q.bindValue(":id", cid);
            if (!q.exec()) QMessageBox::warning(this,"Erreur SQL",q.lastError().text());
            listeCaptures.removeAt(idx);
            actualiserTable();
        });
    }

    mettreAJourStatistiques();
    mettreAJourCompteur();
    mettreAJourBadge();
    mettreAJourTotaux();
    mettreAJourKPI();

    if (stackTable)
        stackTable->setCurrentIndex(tableCaptures->rowCount() == 0 ? 1 : 0);

    // animate rows in with staggered fade
    QTimer::singleShot(30, this, &GestionCaptures::animateRowsIn);
}

void GestionCaptures::animateRowsIn()
{
    // Simple: just ensure all cell widgets are fully visible
    int n = tableCaptures->rowCount();
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < tableCaptures->columnCount(); col++) {
            QWidget *w = tableCaptures->cellWidget(row, col);
            if (w) w->setGraphicsEffect(nullptr);
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  KPI & COUNTERS
// ═══════════════════════════════════════════════════════════════════════════════

void GestionCaptures::animateKpiUpdate(QLabel *lbl, const QString &newVal)
{
    if (!lbl) return;
    lbl->setGraphicsEffect(nullptr); // clear any previous effect
    lbl->setText(newVal);
}

void GestionCaptures::mettreAJourKPI()
{
    if (!kpiTotal) return;
    int total = listeCaptures.size(), totalKg = 0;
    QSet<QString> navires;
    for (const Capture &c : listeCaptures) {
        totalKg += c.quantity;
        if (!c.shipName.isEmpty()) navires.insert(c.shipName);
    }
    int moy = total > 0 ? totalKg / total : 0;
    animateKpiUpdate(kpiTotal,   QString::number(total));
    animateKpiUpdate(kpiKg,      QString::number(totalKg) + " kg");
    animateKpiUpdate(kpiNavires, QString::number(navires.size()));
    animateKpiUpdate(kpiMoy,     QString::number(moy) + " kg");
}

void GestionCaptures::mettreAJourCompteur()
{
    if (!labelCompteur) return;
    int visible = 0;
    for (int i = 0; i < tableCaptures->rowCount(); i++)
        if (!tableCaptures->isRowHidden(i)) visible++;
    labelCompteur->setText(QString("%1 résultat(s)").arg(visible));
}

void GestionCaptures::mettreAJourBadge()
{
    if (!btnAjouter) return;
    int n = listeCaptures.size();
    btnAjouter->setText(n > 0 ? QString("➕  Ajouter  (%1)").arg(n) : "➕  Ajouter");
}

void GestionCaptures::mettreAJourTotaux()
{
    if (!labelTotaux) return;
    int totalKg = 0, visible = 0;
    for (const Capture &c : listeCaptures) {
        if (!m_filtreStatut.isEmpty() && c.status != m_filtreStatut) continue;
        totalKg += c.quantity; visible++;
    }
    labelTotaux->setText(
        QString("  %1 capture(s) affichée(s)   •   Total : %2 kg   •   %3")
        .arg(visible).arg(totalKg)
        .arg(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm")));
}

void GestionCaptures::mettreAJourStatistiques()
{
    if (labelNbTotal) labelNbTotal->setText(QString::number(listeCaptures.size()));
}


// ═══════════════════════════════════════════════════════════════════════════════
//  SLOTS
// ═══════════════════════════════════════════════════════════════════════════════

void GestionCaptures::onAjouter() { ouvrirDialog(nullptr, false); }

void GestionCaptures::onTrierQuantite()
{
    std::sort(listeCaptures.begin(), listeCaptures.end(),
        [this](const Capture &a, const Capture &b) {
            return m_triAscendant ? a.quantity < b.quantity : a.quantity > b.quantity;
        });
    m_triAscendant = !m_triAscendant;
    actualiserTable();
}

void GestionCaptures::filtrerParStatut(const QString &statut)
{
    m_filtreStatut = statut;
    actualiserTable();
    rechercherCapture(lineRecherche ? lineRecherche->text() : QString());
}

void GestionCaptures::rechercherCapture(const QString &texte)
{
    for (int i = 0; i < tableCaptures->rowCount(); i++) {
        bool match = texte.isEmpty();
        for (int col : {1,2,3,4}) {
            if (tableCaptures->item(i,col) &&
                tableCaptures->item(i,col)->text().contains(texte, Qt::CaseInsensitive))
                match = true;
        }
        tableCaptures->setRowHidden(i, !match);
    }
    mettreAJourCompteur();
}

void GestionCaptures::onCarte()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Carte maritime — Zones de pêche Tunisie");
    dlg.setMinimumSize(820,680);
    dlg.setStyleSheet("QDialog{background:#0f172a;}");
    fadeIn(&dlg, 180);
    auto *vbox = new QVBoxLayout(&dlg);
    vbox->setContentsMargins(0,0,0,8); vbox->setSpacing(0);
    vbox->addWidget(new TunisiaMapWidget(listeCaptures, &dlg), 1);
    auto *btnClose = makeBtn("Fermer","#64748b","#475569");
    btnClose->setMaximumWidth(120);
    auto *bh = new QHBoxLayout;
    bh->addStretch(); bh->addWidget(btnClose); bh->setContentsMargins(0,0,12,0);
    vbox->addLayout(bh);
    connect(btnClose, &QPushButton::clicked, &dlg, &QDialog::reject);
    dlg.exec();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  SQL
// ═══════════════════════════════════════════════════════════════════════════════

void GestionCaptures::sauvegarderCapture(const Capture &c, bool modification, const QString &oldId)
{
    if (!c.shipName.trimmed().isEmpty()) {
        QSqlQuery qChk;
        qChk.prepare("SELECT COUNT(*) FROM navire WHERE UPPER(nom)=UPPER(:n)");
        qChk.bindValue(":n", c.shipName.trimmed());
        if (qChk.exec() && qChk.next() && qChk.value(0).toInt() == 0) {
            QSqlQuery qN;
            qN.prepare("INSERT INTO navire (nom,immatriculation,type,capacite,statut) VALUES (:n,'-','artisanal',0,'a quai')");
            qN.bindValue(":n", c.shipName.trimmed());
            if (!qN.exec()) QMessageBox::warning(this,"Erreur navire",qN.lastError().text());
        }
    }
    QSqlQuery q;
    if (modification) {
        q.prepare("UPDATE capture SET ID_NAVIRE=(SELECT id_navire FROM navire WHERE UPPER(nom)=UPPER(:ship)),TYPE_POISSON=:fish,QUANTITE=:qty,DATE_CAPTURE=:dt,AGENT=:agent,STATUT=:statut WHERE ID_CAPTURE=:id");
        q.bindValue(":id", oldId);
        q.bindValue(":ship", c.shipName.trimmed());
    } else {
        q.prepare("INSERT INTO capture (ID_CAPTURE,ID_NAVIRE,DATE_CAPTURE,TYPE_POISSON,QUANTITE,AGENT,STATUT) VALUES (SEQ_CAPTURE.NEXTVAL,(SELECT id_navire FROM navire WHERE UPPER(nom)=UPPER(:ship)),:dt,:fish,:qty,:agent,:statut)");
        q.bindValue(":ship", c.shipName.trimmed());
    }
    q.bindValue(":fish",   c.fishType);
    q.bindValue(":qty",    c.quantity);
    q.bindValue(":dt",     c.captureDate);
    q.bindValue(":agent",  c.agent);
    q.bindValue(":statut", c.status);
    if (!q.exec()) { QMessageBox::warning(this,"Erreur SQL",q.lastError().text()); return; }
    if (!modification) loadFromDb();
}

void GestionCaptures::loadFromDb()
{
    listeCaptures.clear();
    QSqlQuery q;
    if (!q.exec("SELECT c.ID_CAPTURE, n.NOM, c.TYPE_POISSON, c.QUANTITE, c.DATE_CAPTURE, c.AGENT, c.STATUT "
                "FROM capture c LEFT JOIN navire n ON c.ID_NAVIRE=n.ID_NAVIRE ORDER BY c.ID_CAPTURE DESC")) {
        qDebug() << "capture load error:" << q.lastError().text(); return;
    }
    while (q.next()) {
        Capture c;
        c.captureId   = q.value(0).toString();
        c.shipName    = q.value(1).toString();
        c.fishType    = q.value(2).toString();
        c.quantity    = q.value(3).toInt();
        c.captureDate = q.value(4).toDate();
        c.agent       = q.value(5).toString();
        c.status      = q.value(6).toString();
        if (c.status.isEmpty()) c.status = "Enregistré";
        listeCaptures.append(c);
    }
    actualiserTable();
}

void GestionCaptures::chargerCaptures()     { loadFromDb(); }
void GestionCaptures::sauvegarderCaptures() {}

void GestionCaptures::exporterCSV()
{
    if (listeCaptures.isEmpty()) {
        QMessageBox::information(this,"Export PDF","Aucune capture a exporter.");
        return;
    }
    QString fn = QFileDialog::getSaveFileName(this,"Exporter PDF",
        QDir::homePath()+"/captures_"+QDate::currentDate().toString("yyyyMMdd")+".pdf",
        "PDF (*.pdf)");
    if (fn.isEmpty()) return;

    QPdfWriter pdf(fn);
    pdf.setPageSize(QPageSize(QPageSize::A4));
    pdf.setResolution(96);

    QPainter p;
    if (!p.begin(&pdf)) return;

    // Couleurs identiques a l'interface
    QColor cNavy   ("#0a1628");
    QColor cBlue   ("#2563eb");
    QColor cGreen  ("#059669");
    QColor cAmber  ("#d97706");
    QColor cPurple ("#7c3aed");
    QColor cRed    ("#dc2626");
    QColor cBg     ("#f0f4f8");
    QColor cBorder ("#e2e8f0");
    QColor cMuted  ("#64748b");

    int W  = pdf.width();
    int mg = 36;
    int y  = 0;

    // ── HEADER (degrade identique au banner) ─────────────────────────────────
    QLinearGradient grad(0,0,W,0);
    grad.setColorAt(0.00, QColor("#060f1e"));
    grad.setColorAt(0.45, QColor("#0f2d6b"));
    grad.setColorAt(1.00, QColor("#1a56db"));
    p.setBrush(grad); p.setPen(Qt::NoPen);
    p.drawRect(0,0,W,82);

    p.setPen(Qt::white);
    p.setFont(QFont("Arial",21,QFont::Bold));
    p.drawText(mg+36,0,W-mg-36,52,Qt::AlignVCenter|Qt::AlignLeft,"Gestion des capture");
    p.setFont(QFont("Arial",9));
    p.drawText(mg+36,48,W-mg-36,30,Qt::AlignVCenter|Qt::AlignLeft,
               "Genere le : "+QDateTime::currentDateTime().toString("dd/MM/yyyy  hh:mm")
               +"   |   "+QString::number(listeCaptures.size())+" capture");
    // icone poisson
    p.setFont(QFont("Arial",26));
    p.drawText(mg,0,34,82,Qt::AlignVCenter|Qt::AlignLeft,QString::fromUtf8("\xF0\x9F\x90\x9F"));
    y = 98;

    // ── KPI CARDS (memes couleurs que les cartes KPI de l'interface) ──────────
    int totalKg = 0;
    QSet<QString> navires;
    for (const Capture &c : listeCaptures) { totalKg += c.quantity; navires.insert(c.shipName); }
    int moy = listeCaptures.size()>0 ? totalKg/listeCaptures.size() : 0;

    struct Kpi { QString lbl; QString val; QColor col; };
    QList<Kpi> kpis = {
        {"capture totales",  QString::number(listeCaptures.size()), cBlue},
        {"poids total",       QString::number(totalKg)+" kg",        cGreen},
        {"navires actifs",    QString::number(navires.size()),        cAmber},
        {"moyenne / capture", QString::number(moy)+" kg",            cPurple},
    };
    int kw = (W-2*mg)/4;
    for (int i=0;i<4;++i) {
        int bx=mg+i*kw;
        // fond blanc avec bordure gauche coloree (comme les cartes KPI)
        p.setBrush(Qt::white); p.setPen(Qt::NoPen);
        p.drawRoundedRect(bx+3,y,kw-6,54,8,8);
        p.setBrush(kpis[i].col); p.setPen(Qt::NoPen);
        p.drawRoundedRect(bx+3,y,5,54,3,3);
        // valeur
        p.setPen(cNavy);
        p.setFont(QFont("Arial",17,QFont::Bold));
        p.drawText(bx+14,y+2,kw-18,30,Qt::AlignVCenter|Qt::AlignLeft,kpis[i].val);
        // label
        p.setPen(cMuted);
        p.setFont(QFont("Arial",8));
        p.drawText(bx+14,y+30,kw-18,22,Qt::AlignVCenter|Qt::AlignLeft,kpis[i].lbl);
    }
    y += 66;

    // ── TABLE HEADER ─────────────────────────────────────────────────────────
    QStringList hdrs = {"Navire","Type","Qte(kg)","Date","Agent","Zone","Statut"};
    int cws[] = {118,88,72,78,88,88,102};
    // fond degrade bleu marine comme le header de l'interface
    QLinearGradient thGrad(mg,y,W-mg,y);
    thGrad.setColorAt(0, QColor("#0f2d6b"));
    thGrad.setColorAt(1, QColor("#1a56db"));
    p.setBrush(thGrad); p.setPen(Qt::NoPen);
    p.drawRoundedRect(mg,y,W-2*mg,28,6,6);
    p.setPen(Qt::white); p.setFont(QFont("Arial",9,QFont::Bold));
    int cx=mg;
    for (int i=0;i<7;++i) {
        p.drawText(cx+5,y,cws[i]-5,28,Qt::AlignVCenter|Qt::AlignLeft,hdrs[i]);
        cx+=cws[i];
    }
    y+=28;

    // ── ROWS ─────────────────────────────────────────────────────────────────
    auto statusColor = [&](const QString &s) -> QColor {
        if (s=="Valide"||s==QString::fromUtf8("Valid\xC3\xA9"))  return cGreen;
        if (s=="En transit")                                       return cBlue;
        if (s=="Livre"||s==QString::fromUtf8("Livr\xC3\xA9"))    return cPurple;
        if (s=="En attente")                                       return cRed;
        return cAmber;
    };

    for (int i=0;i<listeCaptures.size();++i) {
        if (y > pdf.height()-50) { pdf.newPage(); y=36; }
        const Capture &c = listeCaptures[i];
        // fond alternance identique a l'interface (#f0f4f8 / blanc)
        p.setBrush(i%2==0 ? cBg : Qt::white); p.setPen(Qt::NoPen);
        p.drawRect(mg,y,W-2*mg,26);

        p.setPen(cNavy); p.setFont(QFont("Arial",8));
        QStringList vals={c.shipName,c.fishType,QString::number(c.quantity),
                          c.captureDate.toString("dd/MM/yy"),c.agent,c.zone,""};
        cx=mg;
        for (int j=0;j<6;++j) {
            p.drawText(cx+5,y,cws[j]-5,26,Qt::AlignVCenter|Qt::AlignLeft,vals[j]);
            cx+=cws[j];
        }
        // badge statut
        QColor sc=statusColor(c.status);
        p.setBrush(QColor(sc.red(),sc.green(),sc.blue(),40));
        p.setPen(QPen(sc,1));
        p.drawRoundedRect(cx+4,y+5,cws[6]-10,16,6,6);
        p.setPen(sc); p.setFont(QFont("Arial",7,QFont::Bold));
        p.drawText(cx+4,y+5,cws[6]-10,16,Qt::AlignCenter,c.status);

        p.setPen(QPen(cBorder,1));
        p.drawLine(mg,y+26,W-mg,y+26);
        y+=26;
    }

    // ── FOOTER (meme degrade que le header) ──────────────────────────────────
    int fh=pdf.height();
    QLinearGradient fGrad(0,fh-32,W,fh-32);
    fGrad.setColorAt(0, QColor("#060f1e"));
    fGrad.setColorAt(1, QColor("#1a56db"));
    p.setBrush(fGrad); p.setPen(Qt::NoPen);
    p.drawRect(0,fh-32,W,32);
    p.setPen(Qt::white); p.setFont(QFont("Arial",8));
    p.drawText(mg,fh-32,W-2*mg,32,Qt::AlignVCenter|Qt::AlignCenter,
               "Port Management System  |  Rapport confidentiel");

    p.end();
    QMessageBox::information(this,"Succes",
        QString("%1 capture exportees en PDF.").arg(listeCaptures.size()));
}
QString GestionCaptures::genererCaptureId()
{
    return "CAP-" + QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss-")
           + QUuid::createUuid().toString().mid(1,8).toUpper();
}

GestionCaptures::CapturesStats GestionCaptures::getStats() const
{
    CapturesStats s{0,0,0,0};
    s.total = listeCaptures.size();
    for (const Capture &c : listeCaptures) {
        QString t = c.fishType.toLower();
        if (t == "poisson")       s.poisson++;
        else if (t == "crustacé") s.crustace++;
        else if (t == "mollusque")s.mollusque++;
    }
    return s;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  ouvrirDialog
// ═══════════════════════════════════════════════════════════════════════════════
void GestionCaptures::ouvrirDialog(Capture *capture, bool readOnly)
{
    bool modification = (capture != nullptr && !readOnly);

    QDialog dlg(this);
    dlg.setMinimumSize(720, 640);
    dlg.setStyleSheet(
        "QDialog{background:#f8fafc;font-family:'Segoe UI';font-size:13px;}"
        "QLabel{background:transparent;color:#1e293b;}"
        "QLineEdit,QComboBox,QSpinBox,QDateEdit{"
        "  background:white;border:1px solid #d1d5db;border-radius:8px;"
        "  padding:10px 12px;font-size:13px;color:#111827;}"
        "QLineEdit:focus,QComboBox:focus,QSpinBox:focus,QDateEdit:focus{"
        "  border:2px solid #2563eb;outline:none;}"
        "QTextEdit{background:white;border:1px solid #d1d5db;border-radius:8px;"
        "  padding:10px;font-size:13px;color:#111827;}"
        "QTextEdit:focus{border:2px solid #2563eb;}"
        "QComboBox::drop-down{border:none;width:20px;}"
        "QComboBox QAbstractItemView{background:white;border:1px solid #e5e7eb;"
        "  selection-background-color:#eff6ff;color:#111827;}");

    auto *root = new QVBoxLayout(&dlg);
    root->setContentsMargins(0,0,0,0); root->setSpacing(0);

    QString hdrBg = "qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #060f1e,stop:0.45 #0f2d6b,stop:1 #1a56db)";
    auto *hdr = new QFrame;
    hdr->setFixedHeight(70);
    hdr->setStyleSheet(QString("background:%1;").arg(hdrBg));
    auto *hL = new QHBoxLayout(hdr); hL->setContentsMargins(24,0,24,0);
    auto *hTitle = new QLabel(readOnly ? "Détails de la capture" :
                              modification ? "Modifier la capture" : "Nouvelle capture");
    hTitle->setStyleSheet("color:white;font-size:16px;font-weight:700;");
    hL->addWidget(hTitle); hL->addStretch();
    if (capture) {
        auto *badge = new QLabel("ID: " + capture->captureId);
        badge->setStyleSheet("color:rgba(255,255,255,0.65);font-size:10px;"
                             "background:rgba(255,255,255,0.12);border-radius:6px;padding:3px 9px;");
        hL->addWidget(badge);
    }
    root->addWidget(hdr);

    // BODY
    auto *scroll = new QScrollArea; scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea{border:none;background:#f8fafc;}");
    auto *body = new QWidget; body->setStyleSheet("background:#f8fafc;");
    auto *bLay = new QVBoxLayout(body);
    bLay->setContentsMargins(24,20,24,20); bLay->setSpacing(16);

    // card helper
    auto card = [](const QString &title) {
        auto *f = new QFrame;
        f->setStyleSheet("background:white;border:1px solid #e5e7eb;border-radius:12px;");
        auto *g = new QGridLayout(f);
        g->setContentsMargins(20,16,20,20); g->setHorizontalSpacing(14); g->setVerticalSpacing(12);
        auto *t = new QLabel(title);
        t->setStyleSheet("color:#6b7280;font-size:11px;font-weight:700;letter-spacing:0.5px;");
        g->addWidget(t,0,0,1,3);
        return qMakePair(f,g);
    };

    // field helper
    auto fld = [&](QGridLayout *g, int r, int c, const QString &l, QWidget *w, int s=1){
        auto *lbl = new QLabel(l);
        lbl->setStyleSheet("color:#374151;font-size:11px;font-weight:600;");
        g->addWidget(lbl, r*2+1, c, 1, s);
        g->addWidget(w,   r*2+2, c, 1, s);
        if (readOnly) w->setEnabled(false);
    };

    // FIELDS
    auto *cShip   = new QComboBox; cShip->setEditable(true);
    cShip->setInsertPolicy(QComboBox::NoInsert);
    cShip->lineEdit()->setPlaceholderText("Nom du navire");
    auto *eAgent  = new QLineEdit; eAgent->setPlaceholderText("Agent responsable");
    auto *cStatus = new QComboBox;
    auto *cFish   = new QComboBox;
    auto *sSpin   = new QSpinBox; sSpin->setRange(1,999999); sSpin->setSuffix(" kg");
    auto *dDate   = new QDateEdit; dDate->setCalendarPopup(true);
    dDate->setDisplayFormat("dd/MM/yyyy"); dDate->setDate(QDate::currentDate());
    auto *cMethod = new QComboBox;
    auto *eZone   = new QLineEdit; eZone->setPlaceholderText("Zone de pêche");
    auto *tNotes  = new QTextEdit; tNotes->setFixedHeight(80);
    tNotes->setPlaceholderText("Observations, conditions météo...");

    { QSqlQuery q("SELECT nom FROM navire ORDER BY nom");
      while(q.next()) cShip->addItem(q.value(0).toString()); }
    cFish->addItems({"Thon","Sardine","Bar","Daurade","Mérou","Crabe","Homard","Crevette","Poulpe","Seiche","Autre"});
    cMethod->addItems({"Filet","Ligne","Casier","Chalut","Palangre","Harpon","Autre"});
    cStatus->addItems({"Enregistré","Validé","En transit","Livré","En attente"});

    if (capture) {
        cShip->setCurrentText(capture->shipName);
        eAgent->setText(capture->agent);
        cStatus->setCurrentText(capture->status);
        cFish->setCurrentText(capture->fishType);
        sSpin->setValue(capture->quantity);
        if (capture->captureDate.isValid()) dDate->setDate(capture->captureDate);
        cMethod->setCurrentText(capture->method);
        eZone->setText(capture->zone);
        tNotes->setText(capture->comments);
    } else {
        cShip->setCurrentIndex(-1); cShip->clearEditText();
    }

    auto [card1,g1] = card("NAVIRE & AGENT");
    fld(g1,0,0,"Navire *",  cShip,  3);
    fld(g1,1,0,"Agent *",   eAgent, 2);
    fld(g1,1,2,"Statut",    cStatus);
    bLay->addWidget(card1);

    auto [card2,g2] = card("DÉTAILS DE LA CAPTURE");
    fld(g2,0,0,"Type de poisson *", cFish);
    fld(g2,0,1,"Quantité *",        sSpin);
    fld(g2,0,2,"Date *",            dDate);
    fld(g2,1,0,"Méthode",           cMethod, 3);
    bLay->addWidget(card2);

    auto [card3,g3] = card("LOCALISATION & NOTES");
    fld(g3,0,0,"Zone", eZone, 3);
    auto *nLbl = new QLabel("Notes");
    nLbl->setStyleSheet("color:#374151;font-size:11px;font-weight:600;");
    g3->addWidget(nLbl,  3,0,1,3);
    g3->addWidget(tNotes,4,0,1,3);
    bLay->addWidget(card3);

    scroll->setWidget(body); root->addWidget(scroll,1);

    // FOOTER
    auto *foot = new QFrame;
    foot->setStyleSheet("background:white;border-top:1px solid #e5e7eb;");
    auto *fRow = new QHBoxLayout(foot); fRow->setContentsMargins(24,14,24,14);
    auto *errLbl = new QLabel; errLbl->setVisible(false);
    errLbl->setStyleSheet("color:#dc2626;font-size:12px;font-weight:600;");
    fRow->addWidget(errLbl,1);

    if (readOnly) {
        auto *btnClose = new QPushButton("Fermer");
        btnClose->setStyleSheet(QString("background:%1;color:white;border:none;"
            "border-radius:8px;padding:10px 28px;font-weight:700;").arg(hdrBg));
        connect(btnClose,&QPushButton::clicked,&dlg,&QDialog::accept);
        fRow->addWidget(btnClose);
    } else {
        auto *btnCancel = new QPushButton("Annuler");
        auto *btnSave   = new QPushButton(modification ? "Enregistrer" : "Créer");
        btnCancel->setStyleSheet("background:white;color:#374151;border:1px solid #d1d5db;"
                                 "border-radius:8px;padding:10px 22px;font-weight:600;");
        btnSave->setStyleSheet(QString("background:%1;color:white;border:none;"
            "border-radius:8px;padding:10px 28px;font-weight:700;").arg(hdrBg));
        connect(btnCancel,&QPushButton::clicked,&dlg,&QDialog::reject);
        connect(btnSave,  &QPushButton::clicked,&dlg,[&](){
            bool ok = !cShip->currentText().trimmed().isEmpty() && !eAgent->text().trimmed().isEmpty();
            errLbl->setVisible(!ok);
            if (!ok) errLbl->setText("Navire et Agent sont obligatoires.");
            else dlg.accept();
        });
        fRow->addWidget(btnCancel); fRow->addWidget(btnSave);
    }
    root->addWidget(foot);

    if (dlg.exec() != QDialog::Accepted || readOnly) return;

    Capture c; if (capture) c = *capture; else c.captureId = genererCaptureId();
    c.shipName    = cShip->currentText();
    c.agent       = eAgent->text().trimmed();
    c.fishType    = cFish->currentText();
    c.quantity    = sSpin->value();
    c.captureDate = dDate->date();
    c.zone        = eZone->text().trimmed();
    c.method      = cMethod->currentText();
    c.status      = cStatus->currentText();
    c.comments    = tNotes->toPlainText().trimmed();

    sauvegarderCapture(c, modification, modification ? capture->captureId : QString());
    if (modification && capture) { *capture = c; actualiserTable(); }
    actualiserTable();
    QMessageBox::information(this,"Succès",
        modification ? "Capture modifiée." : "Capture ajoutée.");
}

// ═══════════════════════════════════════════════════════════════════════════════
//  onStatistiques
// ═══════════════════════════════════════════════════════════════════════════════

void GestionCaptures::onStatistiques()
{
    QMap<QString,int> byType;
    int totalQty = 0;
    for (const Capture &c : listeCaptures) { byType[c.fishType] += c.quantity; totalQty += c.quantity; }

    QDialog dlg(this);
    dlg.setWindowTitle("Statistiques des capture");
    dlg.setMinimumSize(640,540);
    dlg.setStyleSheet(
        "QDialog{background:#f0f4f8;font-family:'Segoe UI',Arial;font-size:13px;}"
        "QLabel{background:transparent;color:#1e293b;}"
        "QFrame{background:transparent;}");
    fadeIn(&dlg, 180);

    auto *vbox = new QVBoxLayout(&dlg);
    vbox->setContentsMargins(22,18,22,18); vbox->setSpacing(14);

    // header
    auto *hdr = new QFrame;
    hdr->setStyleSheet(
        "QFrame{background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
        "stop:0 #060f1e,stop:1 #1a56db);border-radius:12px;}");
    hdr->setFixedHeight(54);
    hdr->setGraphicsEffect(makeShadow(14,3,40));
    auto *hdrLay = new QHBoxLayout(hdr);
    hdrLay->setContentsMargins(16,0,16,0);
    auto *hdrT = new QLabel("📊  Répartition des capture par type");
    hdrT->setStyleSheet("font-size:16px;font-weight:900;color:white;background:transparent;");
    hdrLay->addWidget(hdrT); hdrLay->addStretch();
    vbox->addWidget(hdr);

    // summary chips
    auto *sumRow = new QHBoxLayout; sumRow->setSpacing(10);
    auto makeChip = [](const QString &val, const QString &lbl, const QColor &color) {
        auto *f = new QFrame;
        f->setObjectName("statChip");
        f->setStyleSheet(QString(
            "#statChip{background:%1;border-radius:12px;}"
            "#statChip QLabel{background:transparent;color:white;}").arg(color.name()));
        f->setGraphicsEffect(makeShadow(10,2,30));
        auto *h = new QVBoxLayout(f); h->setContentsMargins(16,10,16,10); h->setSpacing(2);
        auto *v = new QLabel(val);
        v->setStyleSheet("font-size:22px;font-weight:900;color:white;background:transparent;");
        v->setAlignment(Qt::AlignCenter);
        auto *l = new QLabel(lbl);
        l->setStyleSheet("font-size:11px;color:rgba(255,255,255,.8);font-weight:600;background:transparent;");
        l->setAlignment(Qt::AlignCenter);
        h->addWidget(v); h->addWidget(l);
        return f;
    };
    sumRow->addWidget(makeChip(QString::number(listeCaptures.size()), "capture",    C_BLUE));
    sumRow->addWidget(makeChip(QString::number(totalQty)+" kg",       "Poids total", C_GREEN));
    sumRow->addWidget(makeChip(QString::number(byType.size()),         "Types",       C_AMBER));
    vbox->addLayout(sumRow);

    const QList<QString> palette = {
        "#3b82f6","#10b981","#f59e0b","#ef4444",
        "#8b5cf6","#06b6d4","#f97316","#84cc16"
    };

    QList<QPair<QString,int>> slices;
    for (auto it = byType.begin(); it != byType.end(); ++it)
        slices.append({it.key(), it.value()});

    // donut chart
    const int SZ = 260;
    QPixmap donutPix(SZ, SZ);
    donutPix.fill(Qt::transparent);
    {
        QPainter p(&donutPix);
        p.setRenderHint(QPainter::Antialiasing);
        QRectF outer(4,4,SZ-8,SZ-8);
        QRectF inner(SZ*0.28, SZ*0.28, SZ*0.44, SZ*0.44);
        int startAngle = 90*16;
        for (int i = 0; i < slices.size(); i++) {
            double pct = totalQty > 0 ? (double)slices[i].second / totalQty : 0.0;
            int span = qRound(pct * 360 * 16);
            p.save();
            double mid = (startAngle + span/2.0) / 16.0 * M_PI / 180.0;
            p.translate(qCos(mid)*3, -qSin(mid)*3);
            QColor c(palette[i % palette.size()]);
            QRadialGradient rg(outer.center(), outer.width()/2);
            rg.setColorAt(0, c.lighter(115));
            rg.setColorAt(1, c);
            p.setBrush(rg);
            p.setPen(QPen(QColor("#f0f4f8"), 3));
            p.drawPie(outer, startAngle, span);
            p.restore();
            startAngle += span;
        }
        // inner hole
        p.setBrush(QColor("#f0f4f8")); p.setPen(Qt::NoPen);
        p.drawEllipse(inner);
        // center text
        p.setPen(QColor("#0a1628"));
        p.setFont(QFont("Segoe UI", 14, QFont::Black));
        p.drawText(inner.adjusted(0,-10,0,-10), Qt::AlignCenter, QString::number(listeCaptures.size()));
        p.setFont(QFont("Segoe UI", 9)); p.setPen(QColor("#64748b"));
        p.drawText(inner.adjusted(0,14,0,14), Qt::AlignCenter, "capture");
        p.end();
    }

    auto *donutLbl = new QLabel;
    donutLbl->setPixmap(donutPix);
    donutLbl->setFixedSize(SZ,SZ);
    donutLbl->setAlignment(Qt::AlignCenter);

    // legend
    auto *legendFrame = new QFrame;
    legendFrame->setObjectName("legendFrame");
    legendFrame->setStyleSheet(
        "#legendFrame{background:white;border-radius:12px;border:1px solid #e2e8f0;}"
        "#legendFrame QLabel{background:transparent;color:#1e293b;}");
    legendFrame->setGraphicsEffect(makeShadow(10,2,14));
    auto *legendLay = new QVBoxLayout(legendFrame);
    legendLay->setContentsMargins(14,12,14,12); legendLay->setSpacing(8);
    for (int i = 0; i < slices.size(); i++) {
        double pct = totalQty > 0 ? (double)slices[i].second / totalQty * 100.0 : 0.0;
        auto *row = new QHBoxLayout; row->setSpacing(8);
        auto *dot = new QLabel("●");
        dot->setStyleSheet(QString("color:%1;font-size:14px;background:transparent;")
                           .arg(palette[i%palette.size()]));
        dot->setFixedWidth(18);
        auto *nameLbl = new QLabel(slices[i].first);
        nameLbl->setStyleSheet("font-weight:700;font-size:12px;");
        auto *pctLbl = new QLabel(QString::number(pct,'f',1)+"%");
        pctLbl->setStyleSheet(QString("font-weight:800;font-size:12px;color:%1;")
                              .arg(palette[i%palette.size()]));
        pctLbl->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
        auto *kgLbl = new QLabel(QString::number(slices[i].second)+" kg");
        kgLbl->setStyleSheet("font-size:11px;color:#64748b;");
        kgLbl->setFixedWidth(64); kgLbl->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
        row->addWidget(dot); row->addWidget(nameLbl,1);
        row->addWidget(pctLbl); row->addWidget(kgLbl);
        legendLay->addLayout(row);
    }

    auto *chartRow = new QHBoxLayout; chartRow->setSpacing(16);
    chartRow->addWidget(donutLbl);
    chartRow->addWidget(legendFrame, 1);
    vbox->addLayout(chartRow, 1);

    // buttons
    auto *bh = new QHBoxLayout; bh->addStretch();
    auto *btnPdf   = makeBtn("📄  Exporter PDF","#2563eb","#1d4ed8");
    auto *btnClose = makeBtn("Fermer",          "#64748b","#475569");
    btnPdf->setMinimumWidth(150); btnClose->setMinimumWidth(110);
    bh->addWidget(btnPdf); bh->addWidget(btnClose);
    vbox->addLayout(bh);

    connect(btnClose, &QPushButton::clicked, &dlg, &QDialog::reject);
    connect(btnPdf, &QPushButton::clicked, &dlg, [&]() {
        QString fn = QFileDialog::getSaveFileName(&dlg,"Enregistrer PDF",
            QDir::homePath()+"/statistiques_captures.pdf","PDF (*.pdf)");
        if (fn.isEmpty()) return;
        QPdfWriter pdf(fn);
        pdf.setPageSize(QPageSize(QPageSize::A4));
        pdf.setResolution(150);
        QPainter p(&pdf);
        p.setFont(QFont("Segoe UI",18,QFont::Bold)); p.setPen(QColor("#0a1628"));
        p.drawText(QRect(60,60,1100,60),Qt::AlignLeft,"Statistiques des capture");
        p.setFont(QFont("Segoe UI",10)); p.setPen(QColor("#64748b"));
        p.drawText(QRect(60,130,1100,40),Qt::AlignLeft,
            "Généré le "+QDate::currentDate().toString("dd/MM/yyyy")
            +"  —  "+QString::number(listeCaptures.size())+" capture"
            +"  —  "+QString::number(totalQty)+" kg total");
        p.drawPixmap(QRect(350,180,500,500), donutPix);
        int y=720; int ci=0;
        p.setFont(QFont("Segoe UI",11,QFont::Bold));
        for (const auto &sl : slices) {
            double pct = totalQty > 0 ? (double)sl.second/totalQty*100.0 : 0.0;
            p.setPen(QColor(palette[ci++%palette.size()]));
            p.drawText(QRect(60,y,40,36),Qt::AlignCenter,"●");
            p.setPen(QColor("#1e293b"));
            p.drawText(QRect(110,y,600,36),Qt::AlignLeft|Qt::AlignVCenter,
                sl.first+"  —  "+QString::number(pct,'f',1)+"%  —  "+QString::number(sl.second)+" kg");
            y+=44;
        }
        p.end();
        QMessageBox::information(&dlg,"Succès","PDF exporté avec succès !");
    });

    dlg.exec();
}

void GestionCaptures::afficherStatistiquesModal() { onStatistiques(); }
