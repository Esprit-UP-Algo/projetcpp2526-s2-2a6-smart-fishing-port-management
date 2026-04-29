#include "mainwindow.h"
#include "connection.h"
#include <QApplication>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QPixmap>
#include <QPainter>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QStringList args = QCoreApplication::arguments();
    if (args.contains("--check-db")) {
        Connection conn;
        bool ok = conn.createconnect();

        // Create a simple colored icon so the tray message displays
        QPixmap pixmap(64,64);
        pixmap.fill(ok ? Qt::green : Qt::red);
        QSystemTrayIcon tray;
        tray.setIcon(QIcon(pixmap));
        tray.setVisible(true);

        tray.showMessage("Database Connection",
                         ok ? "Connection succeeded" : "Connection failed",
                         ok ? QSystemTrayIcon::Information : QSystemTrayIcon::Critical,
                         5000);

        QTimer::singleShot(5500, &a, &QCoreApplication::quit);
        return a.exec();
    }

    MainWindow w;
    w.show();

    return a.exec();
}
