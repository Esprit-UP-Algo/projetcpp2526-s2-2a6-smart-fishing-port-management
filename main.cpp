#include "mainwindow.h"
#include "connection.h"
#include <QApplication>
#include <QFont>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QPixmap>
#include <QPainter>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Set a default application font to prevent QFont warnings
    QFont defaultFont = a.font();
    if (defaultFont.pointSize() <= 0) {
        defaultFont.setPointSize(11);
    }
    defaultFont.setFamily("Segoe UI");
    a.setFont(defaultFont);

    // Load global stylesheet
    QFile styleFile("style.qss");
    if (styleFile.exists() && styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        a.setStyleSheet(styleSheet);
        styleFile.close();
    }

    QStringList args = QCoreApplication::arguments();
    if (args.contains("--check-db")) {
        Connection conn;
        bool ok = conn.createConnect();

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
