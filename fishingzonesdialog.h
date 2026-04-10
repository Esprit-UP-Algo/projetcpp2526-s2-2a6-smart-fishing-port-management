#ifndef FISHINGZONESDIALOG_H
#define FISHINGZONESDIALOG_H

#include <QDialog>
#include <QPointF>
#include <QList>
#include <QString>

class QGraphicsView;
class QGraphicsScene;
class QListWidget;

struct ZoneEntry {
    QString name;
    QPointF pos;
    QString reason;
};

class FishingZonesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FishingZonesDialog(const QList<ZoneEntry> &zones, QWidget *parent = nullptr);
    ~FishingZonesDialog();

private:
    QGraphicsView *view;
    QGraphicsScene *scene;
    QListWidget *list;
    void populate(const QList<ZoneEntry> &zones);
    void toggleFullScreen();
    bool isFull = false;
};

#endif // FISHINGZONESDIALOG_H
