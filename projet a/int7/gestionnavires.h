#ifndef GESTIONNAVIRES_H
#define GESTIONNAVIRES_H

#include <QWidget>
#include <QModelIndex>
#include <QSystemTrayIcon>

class QSqlQueryModel;
class QTableView;
class QLineEdit;
class QSpinBox;
class QComboBox;
class QPushButton;

class GestionNavires : public QWidget
{
    Q_OBJECT

public:
    explicit GestionNavires(QWidget *parent = nullptr);
    void loadFromDb();

    // kept for dashboard compatibility
    struct NaviresStats { int total; int quai; int mer; int interdit; };
    NaviresStats getStats() const;

private:
    QSqlQueryModel *m_model;
    QSystemTrayIcon *m_trayIcon;
    QString m_currentSort;

    // form widgets
    QSpinBox   *spinBox_id_navire;
    QLineEdit  *lineEdit_nom;
    QLineEdit  *lineEdit_immatriculation;
    QLineEdit  *lineEdit_type;
    QSpinBox   *spinBox_capacite;
    QComboBox  *comboBox_statut;

    // right-side widgets
    QLineEdit  *lineEdit_recherche;
    QComboBox  *comboBox_filtre_statut;
    QTableView *tableView_navires;

    void initialiserTable();
    void rafraichirTable(const QString &filtreNom = "",
                         const QString &filtreStatut = "Tous les statuts",
                         const QString &triOrder = "id_navire ASC");
    void viderFormulaire();
    int  idSelectionne() const;
    void controlerSaisie();

private slots:
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

#endif // GESTIONNAVIRES_H
