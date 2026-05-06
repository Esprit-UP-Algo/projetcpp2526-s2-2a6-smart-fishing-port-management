#ifndef CHATBOT_DIALOG_H
#define CHATBOT_DIALOG_H

#include <QDialog>
#include <QSqlDatabase>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace Ui {
class ChatbotDialog;
}

class ChatbotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatbotDialog(QSqlDatabase db, QWidget *parent = nullptr);
    ~ChatbotDialog();

private slots:
    void on_pushButton_send_clicked();
    void onGeminiReply(QNetworkReply *reply);

private:
    Ui::ChatbotDialog *ui;
    QSqlDatabase m_db;
    QNetworkAccessManager *m_networkManager;

    void appendMessage(const QString &sender, const QString &message);
    void sendToGemini(const QString &userMessage);
    QString buildDbContext() const;
};

#endif // CHATBOT_DIALOG_H
