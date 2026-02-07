#include "mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>
#include <QStatusBar>

// ================= CONSTRUCTEUR =================
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(1100,650);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *root = new QHBoxLayout(central);

    // ===== MENU =====
    QWidget *menu = new QWidget;
    menu->setObjectName("menu");
    menu->setFixedWidth(240);

    QLabel *title = new QLabel("⚓ Fishtech");
    title->setObjectName("title");

    btnAdd    = new QPushButton("➕ Ajouter un quai");
    btnEdit   = new QPushButton("✏️ Modifier un quai");
    btnDelete = new QPushButton("🗑️ Supprimer un quai");
    btnView   = new QPushButton("📋 Consulter les quais");

    QVBoxLayout *menuLayout = new QVBoxLayout(menu);
    menuLayout->addWidget(title);
    menuLayout->addWidget(btnAdd);
    menuLayout->addWidget(btnEdit);
    menuLayout->addWidget(btnDelete);
    menuLayout->addWidget(btnView);
    menuLayout->addStretch();

    // ===== STACK =====
    pages = new QStackedWidget;

    pageAjouter   = new QWidget;
    pageModifier  = new QWidget;
    pageSupprimer = new QWidget;
    pageConsulter = new QWidget;

    pages->addWidget(pageAjouter);
    pages->addWidget(pageModifier);
    pages->addWidget(pageSupprimer);
    pages->addWidget(pageConsulter);

    root->addWidget(menu);
    root->addWidget(pages);

    // ================= AJOUT =================
    idEdit = new QLineEdit;
    nomEdit = new QLineEdit;
    longueurEdit = new QLineEdit;
    dispoBox = new QComboBox;
    dispoBox->addItems({"Disponible","Occupé"});
    QPushButton *btnSave = new QPushButton("💾 Enregistrer");

    QFormLayout *formAdd = new QFormLayout;
    formAdd->addRow("ID :", idEdit);
    formAdd->addRow("Nom :", nomEdit);
    formAdd->addRow("Longueur :", longueurEdit);
    formAdd->addRow("État :", dispoBox);
    formAdd->addRow(btnSave);

    pageAjouter->setLayout(formAdd);

    // ================= TABLES =================
    tableConsulter = new QTableWidget(0,4);
    tableSupprimer = new QTableWidget(0,4);
    tableModifier  = new QTableWidget(0,4);

    QStringList headers = {"ID","Nom","Longueur","État"};
    tableConsulter->setHorizontalHeaderLabels(headers);
    tableSupprimer->setHorizontalHeaderLabels(headers);
    tableModifier->setHorizontalHeaderLabels(headers);

    tableConsulter->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableSupprimer->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableModifier->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    tableModifier->setSelectionBehavior(QAbstractItemView::SelectRows);

    // ================= CONSULTER =================
    QVBoxLayout *viewLay = new QVBoxLayout(pageConsulter);
    viewLay->setContentsMargins(0,0,0,0);
    viewLay->addWidget(tableConsulter,1);

    // ================= SUPPRIMER =================
    QPushButton *btnDelRow = new QPushButton("🗑️ Supprimer sélection");
    QVBoxLayout *supLay = new QVBoxLayout(pageSupprimer);
    supLay->addWidget(tableSupprimer,1);
    supLay->addWidget(btnDelRow);

    // ================= MODIFIER =================
    idEditM = new QLineEdit;
    nomEditM = new QLineEdit;
    longueurEditM = new QLineEdit;
    dispoBoxM = new QComboBox;
    dispoBoxM->addItems({"Disponible","Occupé"});
    QPushButton *btnUpdate = new QPushButton("✔️ Modifier");

    QFormLayout *formM = new QFormLayout;
    formM->addRow("ID :", idEditM);
    formM->addRow("Nom :", nomEditM);
    formM->addRow("Longueur :", longueurEditM);
    formM->addRow("État :", dispoBoxM);
    formM->addRow(btnUpdate);

    QWidget *formWidget = new QWidget;
    formWidget->setLayout(formM);
    formWidget->setFixedHeight(220);

    QVBoxLayout *modLay = new QVBoxLayout(pageModifier);
    modLay->addWidget(tableModifier,1);
    modLay->addWidget(formWidget);

    // ================= STATUS =================
    statusBar()->showMessage("Fishtech – Gestion des quais");

    // ================= CONNECTIONS =================
    connect(btnAdd,&QPushButton::clicked,this,&MainWindow::showAjouter);
    connect(btnEdit,&QPushButton::clicked,this,&MainWindow::showModifier);
    connect(btnDelete,&QPushButton::clicked,this,&MainWindow::showSupprimer);
    connect(btnView,&QPushButton::clicked,this,&MainWindow::showConsulter);

    connect(btnSave,&QPushButton::clicked,this,&MainWindow::ajouterQuai);
    connect(btnDelRow,&QPushButton::clicked,this,&MainWindow::supprimerQuai);
    connect(tableModifier,&QTableWidget::cellClicked,
            this,&MainWindow::remplirFormulaireDepuisTable);
    connect(btnUpdate,&QPushButton::clicked,this,&MainWindow::modifierQuai);

    pages->setCurrentWidget(pageConsulter);
}

// ================= NAVIGATION =================
void MainWindow::showAjouter(){ pages->setCurrentWidget(pageAjouter); }
void MainWindow::showConsulter(){ pages->setCurrentWidget(pageConsulter); }

void MainWindow::showSupprimer(){
    pages->setCurrentWidget(pageSupprimer);
    tableSupprimer->setRowCount(tableConsulter->rowCount());
    for(int i=0;i<tableConsulter->rowCount();i++)
        for(int j=0;j<4;j++)
            tableSupprimer->setItem(i,j,
                                    new QTableWidgetItem(tableConsulter->item(i,j)->text()));
}

void MainWindow::showModifier(){
    pages->setCurrentWidget(pageModifier);
    tableModifier->setRowCount(tableConsulter->rowCount());
    for(int i=0;i<tableConsulter->rowCount();i++)
        for(int j=0;j<4;j++)
            tableModifier->setItem(i,j,
                                   new QTableWidgetItem(tableConsulter->item(i,j)->text()));
}

// ================= AJOUT =================
void MainWindow::ajouterQuai(){
    if(idEdit->text().isEmpty() ||
        nomEdit->text().isEmpty() ||
        longueurEdit->text().isEmpty())
    {
        QMessageBox::warning(this,"Erreur",
                             "Veuillez remplir tous les champs.");
        return;
    }

    int r = tableConsulter->rowCount();
    tableConsulter->insertRow(r);

    tableConsulter->setItem(r,0,new QTableWidgetItem(idEdit->text()));
    tableConsulter->setItem(r,1,new QTableWidgetItem(nomEdit->text()));
    tableConsulter->setItem(r,2,new QTableWidgetItem(longueurEdit->text()));
    tableConsulter->setItem(r,3,new QTableWidgetItem(dispoBox->currentText()));

    idEdit->clear();
    nomEdit->clear();
    longueurEdit->clear();
}

// ================= SUPPRIMER =================
void MainWindow::supprimerQuai(){
    int r = tableSupprimer->currentRow();
    if(r < 0) return;

    tableConsulter->removeRow(r);
    tableSupprimer->removeRow(r);
    tableModifier->removeRow(r);
}

// ================= REMPLIR =================
void MainWindow::remplirFormulaireDepuisTable(int r,int){
    idEditM->setText(tableModifier->item(r,0)->text());
    nomEditM->setText(tableModifier->item(r,1)->text());
    longueurEditM->setText(tableModifier->item(r,2)->text());
    dispoBoxM->setCurrentText(tableModifier->item(r,3)->text());
}

// ================= MODIFIER =================
void MainWindow::modifierQuai(){
    int r = tableModifier->currentRow();
    if(r < 0) return;

    QString v[4] = {
        idEditM->text(),
        nomEditM->text(),
        longueurEditM->text(),
        dispoBoxM->currentText()
    };

    for(int j=0;j<4;j++){
        tableConsulter->item(r,j)->setText(v[j]);
        tableSupprimer->item(r,j)->setText(v[j]);
        tableModifier->item(r,j)->setText(v[j]);
    }
}
