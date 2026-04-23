#ifndef NAVIRE_H
#define NAVIRE_H

#include <QMainWindow>
#include <QModelIndex>
#include <QSystemTrayIcon>
#include "connection.h"

class QSqlQueryModel;

QT_BEGIN_NAMESPACE
namespace Ui {
class NAVIRE;
}
QT_END_NAMESPACE

class NAVIRE : public QMainWindow
{
    Q_OBJECT

public:
    NAVIRE(QWidget *parent = nullptr);
    ~NAVIRE();

private:
    Ui::NAVIRE *ui;
    Connection m_connection;
    QSqlQueryModel *m_model;
    QSystemTrayIcon *m_trayIcon;

    void initialiserTable();
    void rafraichirTable(const QString &filtreNom = "", const QString &filtreStatut = "Tous les statuts", const QString &triOrder = "id_navire ASC");
    void viderFormulaire();
    int idSelectionne() const;

    QString m_currentSort;

private slots:
    void controlerSaisie();
    void on_pushButton_ajouter_clicked();
    void on_pushButton_modifier_clicked();
    void on_pushButton_supprimer_clicked();
    void on_tableView_navires_clicked(const QModelIndex &index);

    void on_lineEdit_recherche_textChanged(const QString &arg1);
    void on_comboBox_filtre_statut_currentIndexChanged(int index);
    void on_pushButton_tri_asc_clicked();
    void on_pushButton_tri_desc_clicked();
    void on_pushButton_export_pdf_clicked();
    void on_pushButton_stats_clicked();
    void on_pushButton_historique_clicked();
    void on_pushButton_chatbot_clicked();
};
#endif // NAVIRE_H
