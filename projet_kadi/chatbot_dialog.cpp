#include "chatbot_dialog.h"
#include "ui_chatbot_dialog.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>

ChatbotDialog::ChatbotDialog(QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChatbotDialog),
    m_db(db)
{
    ui->setupUi(this);
    
    appendMessage("Assistant", "Bonjour ! Je suis votre assistant local. Je peux vous aider à trouver des navires ou répondre à vos requêtes sur la flotte.");
}

ChatbotDialog::~ChatbotDialog()
{
    delete ui;
}

void ChatbotDialog::appendMessage(const QString &sender, const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm");
    QString color = (sender == "Vous") ? "#1E3A8A" : "#10B981"; // Vert pour local
    
    ui->chatArea->append(QString("<b style='color:%1;'>%2 [%3]:</b> %4")
                         .arg(color, sender, timestamp, message));
}

void ChatbotDialog::on_pushButton_send_clicked()
{
    QString userText = ui->lineEdit_input->text().trimmed();
    if (userText.isEmpty()) return;

    appendMessage("Vous", userText);
    ui->lineEdit_input->clear();

    processLocalQuery(userText.toLower());
}

void ChatbotDialog::processLocalQuery(const QString &query)
{
    QString response;
    
    if (query.contains("bonjour") || query.contains("salut")) {
        response = "Bonjour ! Comment puis-je vous aider aujourd'hui ?";
    } 
    else if (query.contains("recommande") || query.contains("conseille") || query.contains("meilleur")) {
        QSqlQuery q("SELECT nom, capacite, type FROM navire ORDER BY capacite DESC", m_db);
        if (q.next()) {
            response = QString("D'après ma base de données, je vous recommande le **%1** (%2). C'est un %3 avec une excellente capacité de %4.")
                       .arg(q.value(0).toString())
                       .arg(q.value(1).toString())
                       .arg(q.value(2).toString())
                       .arg(q.value(1).toInt());
        } else {
            response = "Je n'ai aucun navire à vous recommander pour le moment.";
        }
    }
    else if (query.contains("plus petit") || query.contains("moins de capacité")) {
        QSqlQuery q("SELECT nom, capacite FROM navire ORDER BY capacite ASC", m_db);
        if (q.next()) {
            response = QString("Le navire le plus compact de la flotte est le **%1** avec une capacité de %2.")
                       .arg(q.value(0).toString()).arg(q.value(1).toInt());
        }
    }
    else if (query.contains("moyenne")) {
        QSqlQuery q("SELECT AVG(capacite) FROM navire", m_db);
        if (q.next()) response = QString("La capacité moyenne des navires dans la flotte est de **%1**.").arg(q.value(0).toDouble(), 0, 'f', 2);
    }
    else if (query.contains("en mer")) {
        QSqlQuery q("SELECT nom FROM navire WHERE statut = 'en mer'", m_db);
        QStringList atSea;
        while (q.next()) atSea << q.value(0).toString();
        if (atSea.isEmpty()) response = "Aucun navire n'est actuellement en mer.";
        else response = "Navires en mer : " + atSea.join(", ");
    }
    else if (query.contains("cherche") || query.contains("trouve")) {
        QString target = query.split(" ").last();
        QSqlQuery q(m_db);
        q.prepare("SELECT nom, type, statut FROM navire WHERE LOWER(nom) LIKE :name");
        q.bindValue(":name", "%" + target + "%");
        q.exec();
        if (q.next()) {
            response = QString("J'ai trouvé le navire **%1**. C'est un %2 et son statut actuel est : %3.")
                       .arg(q.value(0).toString()).arg(q.value(1).toString()).arg(q.value(2).toString());
        } else {
            response = "Désolé, je n'ai trouvé aucun navire correspondant à votre recherche.";
        }
    }
    else if (query.contains("liste") || query.contains("tous les navires")) {
        QSqlQuery q("SELECT nom, immatriculation FROM navire", m_db);
        QStringList info;
        while (q.next()) info << QString("%1 (%2)").arg(q.value(0).toString()).arg(q.value(1).toString());
        
        if (info.isEmpty()) response = "La base de données est vide.";
        else response = "Voici la flotte actuelle : " + info.join(", ");
    }
    else if (query.contains("à quai") || query.contains("quai")) {
        QSqlQuery q("SELECT nom FROM navire WHERE statut = 'à quai'", m_db);
        QStringList atDock;
        while (q.next()) atDock << q.value(0).toString();
        if (atDock.isEmpty()) response = "Il n'y a aucun navire à quai pour le moment.";
        else response = "Navires à quai : " + atDock.join(", ");
    }
    else if (query.contains("interdit")) {
        QSqlQuery q("SELECT nom FROM navire WHERE statut = 'interdit'", m_db);
        QStringList banned;
        while (q.next()) banned << q.value(0).toString();
        
        if (banned.isEmpty()) response = "Aucun navire n'est interdit.";
        else response = "Navires interdits : " + banned.join(", ");
    }
    else if (query.contains("capacité") || query.contains("capacite")) {
        QSqlQuery q("SELECT SUM(capacite) FROM navire", m_db);
        if (q.next()) response = QString("La capacité totale de la flotte est de %1.").arg(q.value(0).toInt());
    }
    else {
        response = "Désolé, je ne comprends pas cette commande. Essayez 'liste', 'recommande', 'à quai' ou 'capacité'.";
    }

    appendMessage("Assistant", response);
}
