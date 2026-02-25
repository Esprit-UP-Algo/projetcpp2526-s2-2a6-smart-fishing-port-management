#ifndef CAPTURESDIALOG_H
#define CAPTURESDIALOG_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class CapturesDialog : public QWidget
{
    Q_OBJECT
public:
    explicit CapturesDialog(QWidget *parent = nullptr);
    ~CapturesDialog();

signals:
    void closeClicked();

private slots:
    void loadData();
    void onAdd();
    void onEdit();
    void onDelete();
    void onSearch(const QString &text);
    void onSortByNavire();
    void onSortByDate();
    void onSortByQuantite();
    void updateStats();

private:
    QTableWidget *table;
    QLineEdit *lineSearch;
    QLabel *statTotal;
    QLabel *statTotalQuantite;
    QLabel *statMoyenneQuantite;
    QLabel *statMaxQuantite;
    QLabel *statNavireCount;
    void ensureTableExists();
    int selectedCaptureId() const;
    void setupUI();
};

#endif // CAPTURESDIALOG_H
