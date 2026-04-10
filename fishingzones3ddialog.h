#ifndef FISHINGZONES3DDIALOG_H
#define FISHINGZONES3DDIALOG_H

#include <QDialog>
#include <QString>
#include <QList>

struct ZoneEntry3D {
    QString name;
    double lat;
    double lon;
    QString reason;
};

class QWebEngineView;

class FishingZones3DDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FishingZones3DDialog(const QList<ZoneEntry3D> &zones, QWidget *parent = nullptr);
    ~FishingZones3DDialog();

private:
    QWebEngineView *view;
    void loadMapWithZones(const QList<ZoneEntry3D> &zones);
};

#endif // FISHINGZONES3DDIALOG_H
