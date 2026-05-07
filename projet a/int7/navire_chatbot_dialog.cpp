#include "navire_chatbot_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QKeyEvent>

NavireChatbotDialog::NavireChatbotDialog(QSqlDatabase db, QWidget *parent)
    : QDialog(parent), m_db(db)
{
    setWindowTitle("🤖 Assistant Navires");
    resize(620, 520);
    setStyleSheet(R"(
        QDialog { background:#0f172a; }
        QTextEdit { background:#1e293b; color:#e2e8f0; border:none; border-radius:8px;
                    font-size:13px; padding:10px; }
        QLineEdit { background:#1e293b; color:#f1f5f9; border:1px solid #334155;
                    border-radius:8px; padding:8px 12px; font-size:13px; }
        QLineEdit:focus { border:1px solid #3b82f6; }
        QPushButton { background:#1d4ed8; color:white; border:none; border-radius:8px;
                      padding:8px 20px; font-weight:700; font-size:13px; }
        QPushButton:hover { background:#2563eb; }
        QLabel { color:#94a3b8; font-size:12px; }
    )");

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16,16,16,16);
    root->setSpacing(10);

    // Header
    auto *header = new QLabel("⚓  Assistant IA — Gestion des Navires");
    header->setStyleSheet("color:#f1f5f9;font-size:16px;font-weight:700;");
    root->addWidget(header);

    auto *hint = new QLabel("Posez vos questions : liste, statut, capacité, recherche, statistiques...");
    root->addWidget(hint);

    m_chat = new QTextEdit;
    m_chat->setReadOnly(true);
    root->addWidget(m_chat, 1);

    auto *inputRow = new QHBoxLayout;
    m_input = new QLineEdit;
    m_input->setPlaceholderText("Votre question...");
    auto *btnSend = new QPushButton("Envoyer ➤");
    inputRow->addWidget(m_input, 1);
    inputRow->addWidget(btnSend);
    root->addLayout(inputRow);

    connect(btnSend, &QPushButton::clicked, this, &NavireChatbotDialog::onSend);
    connect(m_input, &QLineEdit::returnPressed, this, &NavireChatbotDialog::onSend);

    appendMessage("Assistant",
        "Bonjour ! Je suis votre assistant navires. Je peux répondre à des questions comme :<br>"
        "• <i>liste des navires</i> &nbsp;• <i>navires en mer</i> &nbsp;• <i>navires à quai</i><br>"
        "• <i>navires interdits</i> &nbsp;• <i>capacité totale</i> &nbsp;• <i>cherche Neptune</i><br>"
        "• <i>statistiques</i> &nbsp;• <i>combien de navires</i> &nbsp;• <i>recommande un navire</i>");
}

void NavireChatbotDialog::appendMessage(const QString &sender, const QString &msg)
{
    QString ts = QDateTime::currentDateTime().toString("HH:mm");
    bool isUser = (sender == "Vous");
    QString color  = isUser ? "#60a5fa" : "#34d399";
    QString align  = isUser ? "right" : "left";
    QString bubble = isUser ? "#1e3a8a" : "#134e4a";

    m_chat->append(QString(
        "<div style='text-align:%1;margin:4px 0;'>"
        "<span style='color:%2;font-weight:700;'>%3</span> "
        "<span style='color:#475569;font-size:11px;'>[%4]</span><br>"
        "<span style='background:%5;color:#f1f5f9;border-radius:8px;"
        "padding:4px 10px;display:inline-block;max-width:80%%;'>%6</span>"
        "</div>"
    ).arg(align, color, sender, ts, bubble, msg));
}

void NavireChatbotDialog::onSend()
{
    QString text = m_input->text().trimmed();
    if (text.isEmpty()) return;
    appendMessage("Vous", text);
    m_input->clear();
    processQuery(text.toLower());
}

void NavireChatbotDialog::processQuery(const QString &q)
{
    QString r;

    // ── Salutations ──────────────────────────────────────────────────────────
    if (q.contains("bonjour") || q.contains("salut") || q.contains("hello")) {
        r = "Bonjour ! Comment puis-je vous aider avec la flotte de navires ?";
    }
    // ── Aide ─────────────────────────────────────────────────────────────────
    else if (q.contains("aide") || q.contains("help") || q.contains("commande")) {
        r = "Commandes disponibles :<br>"
            "• <b>liste</b> — tous les navires<br>"
            "• <b>en mer</b> — navires en navigation<br>"
            "• <b>à quai</b> — navires amarrés<br>"
            "• <b>interdit</b> — navires interdits<br>"
            "• <b>capacité</b> — capacité totale/moyenne<br>"
            "• <b>statistiques</b> — résumé complet<br>"
            "• <b>cherche [nom]</b> — rechercher un navire<br>"
            "• <b>recommande</b> — meilleur navire disponible<br>"
            "• <b>type [type]</b> — filtrer par type";
    }
    // ── Liste complète ───────────────────────────────────────────────────────
    else if (q.contains("liste") || q.contains("tous les navires") || q.contains("affiche")) {
        QSqlQuery sq("SELECT nom, immatriculation, type, statut FROM navire ORDER BY nom", m_db);
        QStringList rows;
        while (sq.next())
            rows << QString("<b>%1</b> (%2) — %3 — <i>%4</i>")
                        .arg(sq.value(0).toString(), sq.value(1).toString(),
                             sq.value(2).toString(), sq.value(3).toString());
        r = rows.isEmpty() ? "Aucun navire enregistré." : "Flotte complète :<br>" + rows.join("<br>");
    }
    // ── Combien ──────────────────────────────────────────────────────────────
    else if (q.contains("combien") || q.contains("nombre") || q.contains("total navire")) {
        QSqlQuery sq("SELECT COUNT(*) FROM navire", m_db);
        if (sq.next()) r = QString("La flotte compte <b>%1 navire(s)</b> au total.").arg(sq.value(0).toInt());
    }
    // ── En mer ───────────────────────────────────────────────────────────────
    else if (q.contains("en mer") || q.contains("navigation") || q.contains("mer")) {
        QSqlQuery sq("SELECT nom, type FROM navire WHERE statut = 'en mer'", m_db);
        QStringList list;
        while (sq.next()) list << QString("<b>%1</b> (%2)").arg(sq.value(0).toString(), sq.value(1).toString());
        r = list.isEmpty() ? "Aucun navire n'est actuellement en mer."
                           : QString("<b>%1 navire(s)</b> en mer :<br>").arg(list.size()) + list.join("<br>");
    }
    // ── À quai ───────────────────────────────────────────────────────────────
    else if (q.contains("quai") || q.contains("amarr")) {
        QSqlQuery sq("SELECT nom, type FROM navire WHERE statut = 'à quai'", m_db);
        QStringList list;
        while (sq.next()) list << QString("<b>%1</b> (%2)").arg(sq.value(0).toString(), sq.value(1).toString());
        r = list.isEmpty() ? "Aucun navire n'est à quai."
                           : QString("<b>%1 navire(s)</b> à quai :<br>").arg(list.size()) + list.join("<br>");
    }
    // ── Interdits ────────────────────────────────────────────────────────────
    else if (q.contains("interdit") || q.contains("banni") || q.contains("bloqué")) {
        QSqlQuery sq("SELECT nom, immatriculation FROM navire WHERE statut = 'interdit'", m_db);
        QStringList list;
        while (sq.next()) list << QString("<b>%1</b> (%2)").arg(sq.value(0).toString(), sq.value(1).toString());
        r = list.isEmpty() ? "✅ Aucun navire interdit dans la flotte."
                           : QString("⚠️ <b>%1 navire(s) interdit(s)</b> :<br>").arg(list.size()) + list.join("<br>");
    }
    // ── Capacité totale ──────────────────────────────────────────────────────
    else if (q.contains("capacité totale") || q.contains("capacite totale")) {
        QSqlQuery sq("SELECT SUM(capacite) FROM navire", m_db);
        if (sq.next()) r = QString("Capacité totale de la flotte : <b>%1 tonnes</b>.").arg(sq.value(0).toInt());
    }
    // ── Capacité moyenne ─────────────────────────────────────────────────────
    else if (q.contains("moyenne") || q.contains("capacité moy") || q.contains("capacite moy")) {
        QSqlQuery sq("SELECT AVG(capacite) FROM navire", m_db);
        if (sq.next()) r = QString("Capacité moyenne : <b>%1 tonnes</b>.").arg(sq.value(0).toDouble(), 0, 'f', 1);
    }
    // ── Capacité (générique) ─────────────────────────────────────────────────
    else if (q.contains("capacit")) {
        QSqlQuery sq("SELECT nom, capacite FROM navire ORDER BY capacite DESC", m_db);
        QStringList list;
        while (sq.next()) list << QString("<b>%1</b> : %2 t").arg(sq.value(0).toString()).arg(sq.value(1).toInt());
        r = list.isEmpty() ? "Aucune donnée." : "Capacités :<br>" + list.join("<br>");
    }
    // ── Statistiques ─────────────────────────────────────────────────────────
    else if (q.contains("stat") || q.contains("résumé") || q.contains("resume") || q.contains("rapport")) {
        QSqlQuery sq(m_db);
        int total=0, quai=0, mer=0, interdit=0; double cap=0;
        sq.exec("SELECT COUNT(*) FROM navire"); if(sq.next()) total=sq.value(0).toInt();
        sq.exec("SELECT COUNT(*) FROM navire WHERE statut='à quai'"); if(sq.next()) quai=sq.value(0).toInt();
        sq.exec("SELECT COUNT(*) FROM navire WHERE statut='en mer'"); if(sq.next()) mer=sq.value(0).toInt();
        sq.exec("SELECT COUNT(*) FROM navire WHERE statut='interdit'"); if(sq.next()) interdit=sq.value(0).toInt();
        sq.exec("SELECT SUM(capacite) FROM navire"); if(sq.next()) cap=sq.value(0).toDouble();
        r = QString(
            "📊 <b>Résumé de la flotte</b><br>"
            "• Total : <b>%1</b> navires<br>"
            "• À quai : <b>%2</b> &nbsp;|&nbsp; En mer : <b>%3</b> &nbsp;|&nbsp; Interdits : <b>%4</b><br>"
            "• Capacité totale : <b>%5 t</b>"
        ).arg(total).arg(quai).arg(mer).arg(interdit).arg((int)cap);
    }
    // ── Recommandation ───────────────────────────────────────────────────────
    else if (q.contains("recommand") || q.contains("conseil") || q.contains("meilleur")) {
        QSqlQuery sq("SELECT nom, type, capacite FROM navire WHERE statut='à quai' ORDER BY capacite DESC", m_db);
        if (sq.next())
            r = QString("Je recommande <b>%1</b> (%2) — capacité <b>%3 t</b>, actuellement à quai et disponible.")
                    .arg(sq.value(0).toString(), sq.value(1).toString()).arg(sq.value(2).toInt());
        else
            r = "Aucun navire disponible à quai pour le moment.";
    }
    // ── Plus grand ───────────────────────────────────────────────────────────
    else if (q.contains("plus grand") || q.contains("plus gros") || q.contains("plus grande capacit")) {
        QSqlQuery sq("SELECT nom, capacite, type FROM navire ORDER BY capacite DESC", m_db);
        if (sq.next())
            r = QString("Le plus grand navire est <b>%1</b> (%2) avec <b>%3 t</b>.")
                    .arg(sq.value(0).toString(), sq.value(2).toString()).arg(sq.value(1).toInt());
    }
    // ── Plus petit ───────────────────────────────────────────────────────────
    else if (q.contains("plus petit") || q.contains("moins de capacit")) {
        QSqlQuery sq("SELECT nom, capacite, type FROM navire ORDER BY capacite ASC", m_db);
        if (sq.next())
            r = QString("Le plus petit navire est <b>%1</b> (%2) avec <b>%3 t</b>.")
                    .arg(sq.value(0).toString(), sq.value(2).toString()).arg(sq.value(1).toInt());
    }
    // ── Recherche par nom ────────────────────────────────────────────────────
    else if (q.contains("cherche") || q.contains("trouve") || q.contains("recherche")) {
        // extract last word as search term
        QStringList words = q.split(' ', Qt::SkipEmptyParts);
        QString term = words.last();
        QSqlQuery sq(m_db);
        sq.prepare("SELECT nom, immatriculation, type, capacite, statut FROM navire WHERE LOWER(nom) LIKE :t");
        sq.bindValue(":t", "%" + term + "%");
        sq.exec();
        QStringList list;
        while (sq.next())
            list << QString("<b>%1</b> (%2) — %3 — %4 t — <i>%5</i>")
                        .arg(sq.value(0).toString(), sq.value(1).toString(),
                             sq.value(2).toString()).arg(sq.value(3).toInt())
                        .arg(sq.value(4).toString());
        r = list.isEmpty() ? QString("Aucun navire trouvé pour « %1 ».").arg(term)
                           : "Résultats :<br>" + list.join("<br>");
    }
    // ── Filtre par type ──────────────────────────────────────────────────────
    else if (q.contains("type")) {
        QStringList words = q.split(' ', Qt::SkipEmptyParts);
        QString term = words.last();
        QSqlQuery sq(m_db);
        sq.prepare("SELECT nom, capacite, statut FROM navire WHERE LOWER(type) LIKE :t");
        sq.bindValue(":t", "%" + term + "%");
        sq.exec();
        QStringList list;
        while (sq.next())
            list << QString("<b>%1</b> — %2 t — <i>%3</i>")
                        .arg(sq.value(0).toString()).arg(sq.value(1).toInt())
                        .arg(sq.value(2).toString());
        r = list.isEmpty() ? QString("Aucun navire de type « %1 ».").arg(term)
                           : "Navires de type " + term + " :<br>" + list.join("<br>");
    }
    // ── Fallback ─────────────────────────────────────────────────────────────
    else {
        r = "Je n'ai pas compris votre question. Tapez <b>aide</b> pour voir les commandes disponibles.";
    }

    appendMessage("Assistant", r);
}
