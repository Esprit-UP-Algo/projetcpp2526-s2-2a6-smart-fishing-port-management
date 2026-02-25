#ifndef ADDEDIT_CAPTUREDIALOG_H
#define ADDEDIT_CAPTUREDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QSpinBox>
#include <QComboBox>

class AddEditCaptureDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddEditCaptureDialog(QWidget *parent = nullptr, bool isEdit = false);
    explicit AddEditCaptureDialog(QWidget *parent, bool isEdit, bool allowExport);
    
    QString getNavire() const;
    QDate getDateCapture() const;
    QString getTypePoisson() const;
    int getQuantite() const;

    void setCaptureData(const QString &navire, const QDate &date, const QString &type, int quantite);

private slots:
    void onAccepted();
    void exportToPdf();

private:
    QLineEdit *lineNavire;
    QDateEdit *dateEdit;
    QComboBox *comboType;
    QSpinBox *spinQuantite;
    bool isEditMode;
    bool m_allowExport = false;
};

#endif // ADDEDIT_CAPTUREDIALOG_H
