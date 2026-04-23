#ifndef CHATBOT_DIALOG_H
#define CHATBOT_DIALOG_H

#include <QDialog>
#include <QSqlDatabase>

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

private:
    Ui::ChatbotDialog *ui;
    QSqlDatabase m_db;

    void appendMessage(const QString &sender, const QString &message);
    void processLocalQuery(const QString &query);
};

#endif // CHATBOT_DIALOG_H
