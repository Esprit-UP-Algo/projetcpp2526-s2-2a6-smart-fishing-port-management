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
    // New ctor that accepts an existing capture id when editing
    explicit AddEditCaptureDialog(QWidget *parent, bool isEdit, int captureId, bool allowExport = false);
    
    QString getNavire() const;
    QDate getDateCapture() const;
    QString getTypePoisson() const;
    int getQuantite() const;

    // Backwards-compatible API: an optional fifth field may be provided by callers
    // (keeps binary compatibility with older build artifacts that expect 5 args)
    void setCaptureData(const QString &navire, const QDate &date, const QString &type, int quantite, const QString &extra = QString());

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
    int m_captureId = -1; // -1 means new capture
};

#endif // ADDEDIT_CAPTUREDIALOG_H
