#ifndef NAVIRE_CHATBOT_DIALOG_H
#define NAVIRE_CHATBOT_DIALOG_H

#include <QDialog>
#include <QSqlDatabase>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>

class NavireChatbotDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NavireChatbotDialog(QSqlDatabase db, QWidget *parent = nullptr);

private slots:
    void onSend();

private:
    QSqlDatabase m_db;
    QTextEdit   *m_chat;
    QLineEdit   *m_input;

    void appendMessage(const QString &sender, const QString &msg);
    void processQuery(const QString &query);
};

#endif // NAVIRE_CHATBOT_DIALOG_H
