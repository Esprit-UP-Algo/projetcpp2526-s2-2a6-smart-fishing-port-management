#pragma once
#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QComboBox>
#include "../arduino.h"

class BarriereWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BarriereWidget(const QList<QString> &navires, QWidget *parent = nullptr);

private slots:
    void onOuvrir();
    void onFermer();
    void onAutoToggle();
    void onTick();
    void onNavireSelected(int row);

private:
    void sendCmd(const QByteArray &cmd);
    void log(const QString &msg);
    void updateBarriereVisual();

    Arduino      m_arduino;
    bool         m_connected   = false;
    bool         m_autoMode    = false;
    bool         m_barriereOpen= false;
    int          m_tick        = 0;
    int          m_detectedIdx = -1;

    QList<QString> m_navires;

    QLabel      *m_statusLbl   = nullptr;
    QLabel      *m_barriereViz = nullptr;  // animated barrier visual
    QListWidget *m_navireList  = nullptr;
    QListWidget *m_logList     = nullptr;
    QPushButton *m_btnOuvrir   = nullptr;
    QPushButton *m_btnFermer   = nullptr;
    QPushButton *m_btnAuto     = nullptr;
    QTimer      *m_timer       = nullptr;
};
