#include "mainwindow.h"
#include "connection.h"
#include "AppStyle.h"
#include <QApplication>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QPixmap>
#include <QPainter>
#include <QMessageBox>
#include <QDebug>
#include <QFont>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Style global professionnel
    a.setStyleSheet(appStyleSheet());
    QFont appFont("Segoe UI", 10);
    appFont.setHintingPreference(QFont::PreferFullHinting);
    a.setFont(appFont);

    // Vérifier que le système supporte la barre système
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        qWarning() << "La barre système n'est pas disponible sur ce système.";
    }

    QStringList args = QCoreApplication::arguments();

    if (args.contains("--check-db")) {
        Connection conn;
        bool ok = conn.createconnect();

        // Créer une icône colorée selon le statut
        QPixmap pixmap(64, 64);
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(ok ? Qt::green : Qt::red);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(4, 4, 56, 56);
        painter.end();

        QSystemTrayIcon tray;
        tray.setIcon(QIcon(pixmap));
        tray.setVisible(true);
        tray.showMessage(
            "Database Connection",
            ok ? "✅ Connexion réussie à Oracle freepdb1"
               : "❌ Échec de connexion à la base de données",
            ok ? QSystemTrayIcon::Information
               : QSystemTrayIcon::Critical,
            5000
            );

        qDebug() << (ok ? "Connexion réussie" : "Connexion échouée");

        QTimer::singleShot(5500, &a, &QCoreApplication::quit);
        return a.exec();
    }

    // Vérifier la connexion avant d'ouvrir la fenêtre principale
    Connection conn;
    if (!conn.createconnect()) {
        QMessageBox::critical(
            nullptr,
            "Erreur de connexion",
            "Impossible de se connecter à la base de données Oracle.\n"
            "Vérifiez que le service freepdb1 est démarré."
            );
        return -1; // Quitter l'application si la DB est inaccessible
    }

    qDebug() << "Connexion à Oracle réussie, lancement de l'application...";

    MainWindow w;
    w.show();
    return a.exec();
}