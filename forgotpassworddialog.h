#ifndef FORGOTPASSWORDDIALOG_H
#define FORGOTPASSWORDDIALOG_H

#include <QDialog>
#include <QLineEdit>

namespace Ui { class ForgotPasswordDialog; }

class ForgotPasswordDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ForgotPasswordDialog(QWidget *parent = nullptr);
    ~ForgotPasswordDialog();

    QString getName() const;
    QString getId() const;
    QString getPoste() const;
    QString getEmail() const;
    QString getTelephone() const;

private slots:
    void onAccepted();

private:
    QWidget *contentWidget; 
    QLineEdit *lineName; 
    QLineEdit *lineId; 
    QLineEdit *linePoste;  
    QLineEdit *lineEmail; 
    QLineEdit *linePhone; 
};

#endif // FORGOTPASSWORDDIALOG_H
