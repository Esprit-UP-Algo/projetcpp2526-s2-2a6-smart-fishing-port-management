#include "fishingzones3ddialog.h"
#include <QWebEngineView>
#include <QVBoxLayout>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDir>

FishingZones3DDialog::FishingZones3DDialog(const QList<ZoneEntry3D> &zones, QWidget *parent)
    : QDialog(parent), view(nullptr)
{
    setWindowTitle("Recommandation: Zones de pêche (3D)");
    setMinimumSize(900, 600);

    QVBoxLayout *main = new QVBoxLayout(this);
    view = new QWebEngineView(this);
    main->addWidget(view, 1);

    loadMapWithZones(zones);
}

FishingZones3DDialog::~FishingZones3DDialog()
{
}

void FishingZones3DDialog::loadMapWithZones(const QList<ZoneEntry3D> &zones)
{
    // Load HTML template from resource and inject zones as JSON
    QFile f(":/web/capture_map.html");
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // fallback: simple HTML
        QString fallback = "<html><body><h3>Impossible de charger la carte (fichier manquant).</h3></body></html>";
        view->setHtml(fallback);
        return;
    }
    QString html = QString::fromUtf8(f.readAll());
    f.close();

    QJsonArray arr;
    for (const ZoneEntry3D &z : zones) {
        QJsonObject o;
        o["name"] = z.name;
        o["lat"] = z.lat;
        o["lon"] = z.lon;
        o["reason"] = z.reason;
        arr.append(o);
    }
    QJsonDocument doc(arr);
    QString json = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));

    html.replace("__ZONES_DATA__", json);

    view->setHtml(html, QUrl("qrc:/web/"));
}
