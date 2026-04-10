#include "fishingzonesdialog.h"

#include <QHBoxLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QListWidget>
#include <QLabel>
#include <QGraphicsRectItem>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>

FishingZonesDialog::FishingZonesDialog(const QList<ZoneEntry> &zones, QWidget *parent)
    : QDialog(parent), view(nullptr), scene(nullptr), list(nullptr)
{
    setWindowTitle("Recommandation: Zones de pêche");
    setMinimumSize(600, 360);

    QVBoxLayout *outer = new QVBoxLayout(this);
    outer->setContentsMargins(8,8,8,8);
    outer->setSpacing(10);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setSpacing(10);

    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, this);
    view->setRenderHint(QPainter::Antialiasing);
    // make the view larger by default so the map is visible
    view->setMinimumSize(700, 450);
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view->setAlignment(Qt::AlignCenter);
    view->setResizeAnchor(QGraphicsView::AnchorViewCenter);

    list = new QListWidget(this);
    list->setMaximumWidth(340);

    mainLayout->addWidget(view, 4);
    mainLayout->addWidget(list, 1);

    // add main layout to outer container
    outer->addLayout(mainLayout, 1);

    populate(zones);

    // Controls: add fullscreen toggle button under the map
    QHBoxLayout *controlLayout = new QHBoxLayout();
    controlLayout->setContentsMargins(6,6,6,6);
    controlLayout->addStretch();
    QPushButton *fsBtn = new QPushButton("Plein écran", this);
    fsBtn->setToolTip("Basculer en plein écran pour la carte");
    controlLayout->addWidget(fsBtn);
    outer->addLayout(controlLayout);

    connect(fsBtn, &QPushButton::clicked, this, &FishingZonesDialog::toggleFullScreen);

    // Open dialog in fullscreen immediately so the map fills the screen
    this->showFullScreen();
    isFull = true;

    // After entering fullscreen, refit and apply initial zoom
    QRectF boundsAfter = scene->itemsBoundingRect();
    if (!boundsAfter.isEmpty()) {
        view->fitInView(boundsAfter, Qt::KeepAspectRatio);
        view->centerOn(boundsAfter.center());
    } else {
        view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    }
    view->scale(1.5, 1.5);
}

FishingZonesDialog::~FishingZonesDialog()
{
}

void FishingZonesDialog::populate(const QList<ZoneEntry> &zones)
{
    scene->clear();
    list->clear();

    // Make scene size large so it can scale to the dialog size
    QRectF sceneRect(0, 0, 800, 500);
    scene->setSceneRect(sceneRect);

    // Procedural bathymetric-like background using QImage and QPainter
    QImage img(sceneRect.size().toSize(), QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);
    QPainter ip(&img);
    ip.setRenderHint(QPainter::Antialiasing);

    // base gradient (shallow -> deep)
    QLinearGradient grad(0, 0, 0, img.height());
    grad.setColorAt(0.0, QColor(200, 230, 245));
    grad.setColorAt(0.4, QColor(160, 210, 235));
    grad.setColorAt(0.7, QColor(90, 170, 200));
    grad.setColorAt(1.0, QColor(30, 100, 150));
    ip.fillRect(img.rect(), grad);

    // add soft light noise / ridges to simulate bathymetry (simple sine waves)
    ip.setPen(QPen(QColor(255,255,255,30), 1));
    for (int y = 0; y < img.height(); y += 8) {
        QPainterPath p;
        p.moveTo(0, y);
        for (int x = 0; x <= img.width(); x += 10) {
            double nx = x / double(img.width());
            double ny = y / double(img.height());
            double offset = 12.0 * sin(nx * 6.0 + ny * 3.0) * cos(nx * 3.0 - ny * 2.0);
            p.lineTo(x, y + offset);
        }
        ip.drawPath(p);
    }

    // subtle dark contours
    ip.setPen(QPen(QColor(0,0,0,30), 1));
    for (int y = 10; y < img.height(); y += 40) {
        ip.drawPolyline(new QPointF[0], 0); // nop to keep consistent loops
        QPainterPath p;
        p.moveTo(0, y);
        for (int x = 0; x <= img.width(); x += 20) {
            double nx = x / double(img.width());
            double offset = 6.0 * sin(nx * 10.0 + y * 0.02);
            p.lineTo(x, y + offset);
        }
        ip.setOpacity(0.25);
        ip.drawPath(p);
        ip.setOpacity(1.0);
    }

    ip.end();

        QGraphicsPixmapItem *pix = scene->addPixmap(QPixmap::fromImage(img));
        pix->setPos(sceneRect.topLeft());
        pix->setZValue(-10);

        // Add demo island polygons (land) on top of the bathymetric background
        QPolygonF isl1;
        isl1 << QPointF(sceneRect.width()*0.18, sceneRect.height()*0.62)
            << QPointF(sceneRect.width()*0.23, sceneRect.height()*0.55)
            << QPointF(sceneRect.width()*0.28, sceneRect.height()*0.58)
            << QPointF(sceneRect.width()*0.27, sceneRect.height()*0.66)
            << QPointF(sceneRect.width()*0.20, sceneRect.height()*0.68);
        QGraphicsPolygonItem *poly1 = scene->addPolygon(isl1, QPen(QColor(80,60,30)), QBrush(QColor(238,214,175)));
        poly1->setZValue(-5);

        QPolygonF isl2;
        isl2 << QPointF(sceneRect.width()*0.66, sceneRect.height()*0.20)
            << QPointF(sceneRect.width()*0.72, sceneRect.height()*0.18)
            << QPointF(sceneRect.width()*0.78, sceneRect.height()*0.22)
            << QPointF(sceneRect.width()*0.75, sceneRect.height()*0.28)
            << QPointF(sceneRect.width()*0.68, sceneRect.height()*0.26);
        QGraphicsPolygonItem *poly2 = scene->addPolygon(isl2, QPen(QColor(80,60,30)), QBrush(QColor(238,214,175)));
        poly2->setZValue(-5);

        QPolygonF isl3;
        isl3 << QPointF(sceneRect.width()*0.45, sceneRect.height()*0.72)
            << QPointF(sceneRect.width()*0.49, sceneRect.height()*0.68)
            << QPointF(sceneRect.width()*0.55, sceneRect.height()*0.70)
            << QPointF(sceneRect.width()*0.52, sceneRect.height()*0.76)
            << QPointF(sceneRect.width()*0.46, sceneRect.height()*0.76);
        QGraphicsPolygonItem *poly3 = scene->addPolygon(isl3, QPen(QColor(80,60,30)), QBrush(QColor(238,214,175)));
        poly3->setZValue(-5);

    // Draw each zone as a circle with label
    for (int i = 0; i < zones.size(); ++i) {
        const QString &name = zones[i].name;
        const QString &reason = zones[i].reason;
        QPointF p = zones[i].pos;
        // If positions were provided as fractional/relative coordinates (x,y within 0..400 range),
        // map them into the scene rect. If they look like lon/lat they should already be mapped externally.
        QPointF pt = p;
        if (p.x() <= 400 && p.y() <= 260) {
            // scale from demo 400x260 to scene size
            pt.setX((p.x() / 400.0) * sceneRect.width() + sceneRect.left());
            pt.setY((p.y() / 260.0) * sceneRect.height() + sceneRect.top());
        }

        QGraphicsEllipseItem *e = scene->addEllipse(pt.x(), pt.y(), 18, 18, QPen(Qt::darkBlue), QBrush(Qt::blue));
        e->setToolTip(name + "\n" + reason);
        QGraphicsTextItem *t = scene->addText(name);
        t->setDefaultTextColor(Qt::black);
        t->setPos(pt.x() + 20, pt.y() - 4);

        list->addItem(name + " - " + reason);
    }

    // Fit the scene to the view so the map fills the available space
    QRectF bounds = scene->itemsBoundingRect();
    if (!bounds.isEmpty()) {
        view->fitInView(bounds, Qt::KeepAspectRatio);
        view->centerOn(bounds.center());
    } else {
        view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    }

    // Force a larger initial zoom: scale up slightly to make map elements more visible
    view->scale(1.35, 1.35);
}

void FishingZonesDialog::toggleFullScreen()
{
    if (!isFull) {
        this->showFullScreen();
        isFull = true;
    } else {
        this->showNormal();
        isFull = false;
    }
    // after toggling, refit to available area
    QRectF bounds = scene->itemsBoundingRect();
    if (!bounds.isEmpty()) view->fitInView(bounds, Qt::KeepAspectRatio);
}
