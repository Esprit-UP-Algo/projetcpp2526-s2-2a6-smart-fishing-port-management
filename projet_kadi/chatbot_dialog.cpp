#include "chatbot_dialog.h"
#include "ui_chatbot_dialog.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>

static const QString GEMINI_API_KEY = "AIzaSyAAvDg87Cv3O_-pxWbpU7v3n9podxtp0KM";
static const QString GEMINI_URL = "https://generativelanguage.googleapis.com/v1beta/models/gemini-3-flash-preview:generateContent?key=" + GEMINI_API_KEY;

ChatbotDialog::ChatbotDialog(QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChatbotDialog),
    m_db(db),
    m_networkManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    connect(m_networkManager, &QNetworkAccessManager::finished, this, &ChatbotDialog::onGeminiReply);
    appendMessage("Assistant", "Bonjour ! Je suis votre assistant IA. Posez-moi n'importe quelle question sur la flotte de navires.");
}

ChatbotDialog::~ChatbotDialog()
{
    delete ui;
}

void ChatbotDialog::appendMessage(const QString &sender, const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm");
    QString color = (sender == "Vous") ? "#1E3A8A" : "#10B981";
    ui->chatArea->append(QString("<b style='color:%1;'>%2 [%3]:</b> %4")
                         .arg(color, sender, timestamp, message));
}

QString ChatbotDialog::buildDbContext() const
{
    QString context = "Voici les données actuelles de la flotte de navires :\n\n";
    context += "ID | Nom | Immatriculation | Type | Capacité | Tirant d'eau (m) | Statut\n";
    context += "---|-----|-----------------|------|----------|------------------|-------\n";

    QSqlQuery q(m_db);
    q.exec("SELECT id_navire, nom, immatriculation, type, capacite, tiran_deau, statut FROM navire ORDER BY id_navire");
    while (q.next()) {
        context += QString("%1 | %2 | %3 | %4 | %5 | %6 | %7\n")
                   .arg(q.value(0).toString())
                   .arg(q.value(1).toString())
                   .arg(q.value(2).toString())
                   .arg(q.value(3).toString())
                   .arg(q.value(4).toString())
                   .arg(q.value(5).toString())
                   .arg(q.value(6).toString());
    }

    context += "\nTu es un assistant spécialisé dans la gestion portuaire. "
               "Réponds en français, de manière concise et précise, en te basant uniquement sur ces données. "
               "Si une information n'est pas dans les données, dis-le clairement.";
    return context;
}

void ChatbotDialog::sendToGemini(const QString &userMessage)
{
    QJsonObject systemInstruction;
    QJsonArray sysParts;
    QJsonObject sysPart;
    sysPart["text"] = buildDbContext();
    sysParts.append(sysPart);
    systemInstruction["parts"] = sysParts;

    QJsonObject userPart;
    userPart["text"] = userMessage;
    QJsonArray userParts;
    userParts.append(userPart);

    QJsonObject userContent;
    userContent["role"] = "user";
    userContent["parts"] = userParts;

    QJsonArray contents;
    contents.append(userContent);

    QJsonObject body;
    body["system_instruction"] = systemInstruction;
    body["contents"] = contents;

    QUrl url(GEMINI_URL);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    m_networkManager->post(request, QJsonDocument(body).toJson());
}

void ChatbotDialog::onGeminiReply(QNetworkReply *reply)
{
    reply->deleteLater();

    QByteArray responseData = reply->readAll();

    if (reply->error() != QNetworkReply::NoError) {
        QString errorDetail = reply->errorString();
        QJsonDocument errDoc = QJsonDocument::fromJson(responseData);
        if (!errDoc.isNull() && errDoc.isObject())
            errorDetail += " — " + errDoc["error"]["message"].toString();
        appendMessage("Assistant", "Erreur : " + errorDetail);
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    QString text;

    QJsonArray parts = doc["candidates"][0]["content"]["parts"].toArray();
    for (const QJsonValue &part : parts) {
        // Les modèles "thinking" ont une part {thought:true, text:...} à ignorer
        if (!part["thought"].toBool()) {
            text = part["text"].toString().trimmed();
            if (!text.isEmpty()) break;
        }
    }

    if (text.isEmpty())
        text = "Je n'ai pas pu générer de réponse. Veuillez réessayer.";

    appendMessage("Assistant", text);
}

void ChatbotDialog::on_pushButton_send_clicked()
{
    QString userText = ui->lineEdit_input->text().trimmed();
    if (userText.isEmpty()) return;

    appendMessage("Vous", userText);
    ui->lineEdit_input->clear();

    appendMessage("Assistant", "<i>En train de répondre...</i>");
    sendToGemini(userText);
}
