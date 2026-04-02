#include "sessionmanager.h"
#include "database/connection.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>
#include <QCryptographicHash>

SessionManager& SessionManager::getInstance() {
    static SessionManager instance;
    return instance;
}

SessionManager::SessionManager() : currentRole(NotLoggedIn), loggedIn(false) {
}

SessionManager::~SessionManager() {
}

bool SessionManager::authenticate(const QString &username, const QString &password) {
    Connection conn;
    if (!conn.createconnect()) {
        qDebug() << "❌ Database connection failed";
        return false;
    }

    // Clean up inputs - remove extra quotes/whitespace
    QString cleanUsername = username.trimmed();
    QString cleanPassword = password.trimmed();
    
    if (cleanUsername.startsWith('"') && cleanUsername.endsWith('"')) {
        cleanUsername = cleanUsername.mid(1, cleanUsername.length() - 2);
    }
    if (cleanPassword.startsWith('"') && cleanPassword.endsWith('"')) {
        cleanPassword = cleanPassword.mid(1, cleanPassword.length() - 2);
    }
    if (cleanUsername.startsWith("'") && cleanUsername.endsWith("'")) {
        cleanUsername = cleanUsername.mid(1, cleanUsername.length() - 2);
    }
    if (cleanPassword.startsWith("'") && cleanPassword.endsWith("'")) {
        cleanPassword = cleanPassword.mid(1, cleanPassword.length() - 2);
    }
    
    cleanUsername = cleanUsername.trimmed();
    cleanPassword = cleanPassword.trimmed();

    QSqlQuery query;
    // Try to find user by name OR email
    query.prepare("SELECT * FROM employes WHERE nom = ? OR email = ?");
    query.addBindValue(cleanUsername);
    query.addBindValue(cleanUsername);

    if (!query.exec()) {
        qDebug() << "❌ Query failed:" << query.lastError().text();
        return false;
    }

    if (!query.next()) {
        qDebug() << "❌ User not found:" << cleanUsername << "(searched by name and email)";
        return false;
    }

    qDebug() << "\n========== AUTH DEBUG ==========";
    qDebug() << "✅ User found in database";
    qDebug() << "   NOM:" << query.value("nom").toString();
    qDebug() << "   EMAIL:" << query.value("email").toString();
    
    // Verify password (simple hash comparison - consider bcrypt in production)
    QString storedHashedPassword = query.value("PASSWORD_HASH").toString().toLower().trimmed();
    QString providedHash = QString(QCryptographicHash::hash(cleanPassword.toUtf8(), 
                                                             QCryptographicHash::Sha256).toHex()).toLower();

    qDebug() << "\n[Password Verification]";
    qDebug() << "   Input password: '" << cleanPassword << "'";
    qDebug() << "   Calculated SHA256: " << providedHash;
    qDebug() << "   DB Stored hash: " << storedHashedPassword;
    qDebug() << "   Hash match: " << (storedHashedPassword == providedHash ? "✅ YES" : "❌ NO");

    if (storedHashedPassword.isEmpty()) {
        qDebug() << "⚠️  WARNING: PASSWORD_HASH is empty/NULL in database!";
        qDebug() << "   To fix this, run this SQL:";
        qDebug() << "   UPDATE employes SET PASSWORD_HASH = '" << providedHash << "' WHERE email = '" << cleanUsername << "';";
        qDebug() << "================================\n";
        return false;
    }

    // Backward compatible MD5 support for existing users
    QString md5Hash = QString(QCryptographicHash::hash(cleanPassword.toUtf8(), QCryptographicHash::Md5).toHex()).toLower();
    if (storedHashedPassword == md5Hash) {
        qDebug() << "⚠️  WARNING: Found old MD5 password hash, upgrading to SHA256...";
        qDebug() << "   Executing DB upgrade SQL:";
        qDebug() << "   UPDATE employes SET PASSWORD_HASH = '" << providedHash << "' WHERE email = '" << cleanUsername << "';";
        QSqlQuery upgradeQuery;
        if (upgradeQuery.exec(QString("UPDATE employes SET PASSWORD_HASH = '%1' WHERE email = '%2'")
                              .arg(providedHash, cleanUsername))) {
            qDebug() << "✅ Password hash upgraded to SHA256 for" << cleanUsername;
        } else {
            qDebug() << "❌ Failed to upgrade password hash:" << upgradeQuery.lastError().text();
        }
        // Continue login success path
    } else if (storedHashedPassword != providedHash) {
        qDebug() << "❌ HASH MISMATCH!";
        qDebug() << "   Stored:   " << storedHashedPassword;
        qDebug() << "   Expected: " << providedHash;
        qDebug() << "\n   To fix this, run this SQL:";
        qDebug() << "   UPDATE employes SET PASSWORD_HASH = '" << providedHash << "' WHERE email = '" << cleanUsername << "';";
        qDebug() << "================================\n";
        return false;
    }

    qDebug() << "✅ PASSWORD CORRECT - Authentication successful!";
    qDebug() << "================================\n";

    // Authentication successful - load user data
    currentUser.id = query.value("id").toString();
    currentUser.nom = query.value("nom").toString();
    currentUser.poste = query.value("poste").toString();
    currentUser.email = query.value("email").toString();
    currentUser.telephone = query.value("telephone").toString();
    currentUser.salaire = query.value("salaire").toDouble();
    currentUser.genre = query.value("genre").toString();

    // Set role based on position
    currentRole = getRoleFromUsername(currentUser.poste);
    loggedIn = true;

    qDebug() << "✅ User logged in:" << currentUser.nom << "as" << getCurrentRoleString();
    return true;
}

void SessionManager::logout() {
    loggedIn = false;
    currentRole = NotLoggedIn;
    currentUser = Employe();
    qDebug() << "✅ User logged out";
}

bool SessionManager::isLoggedIn() const {
    return loggedIn;
}

Employe SessionManager::getCurrentUser() const {
    return currentUser;
}

SessionManager::UserRole SessionManager::getCurrentRole() const {
    return currentRole;
}

QString SessionManager::getCurrentRoleString() const {
    switch (currentRole) {
        case Pecheur: return "Pêcheur";
        case Matelot: return "Matelot";
        case Docker: return "Docker";
        case ChefDeQuai: return "Chef de quai";
        case Superviseur: return "Superviseur";
        case HRManager: return "Responsable RH";
        case DirecteurOperationnel: return "Directeur Opérationnel";
        default: return "Not Logged In";
    }
}

bool SessionManager::canAccessPage(AccessPage page) const {
    if (!loggedIn) return false;

    // Define access control matrix - be specific about each role
    switch (currentRole) {
        case Pecheur:
            return page == PageCaptures;
        
        case Matelot:
            return page == PageNavires;
        
        case Docker:
            return page == PageQuai;
        
        case ChefDeQuai:
            return page == PageQuai || page == PageEmployees;
        
        case Superviseur:
            return page == PageCaptures || page == PageNavires || page == PageQuai || page == PageAnalytics;
        
        case HRManager:
            return page == PageEmployees;  // ONLY employees
        
        case DirecteurOperationnel:
            return true;  // Full access to all pages
        
        default:
            return false;
    }
}

QList<SessionManager::AccessPage> SessionManager::getAccessiblePages() const {
    QList<AccessPage> pages;

    if (!loggedIn) return pages;

    // Add pages based on specific role
    switch (currentRole) {
        case Pecheur:
            pages.append(PageCaptures);
            break;
        
        case Matelot:
            pages.append(PageNavires);
            break;
        
        case Docker:
            pages.append(PageQuai);
            break;
        
        case ChefDeQuai:
            pages.append(PageQuai);
            pages.append(PageEmployees);
            break;
        
        case Superviseur:
            pages.append(PageCaptures);
            pages.append(PageNavires);
            pages.append(PageQuai);
            pages.append(PageAnalytics);
            break;
        
        case HRManager:
            pages.append(PageEmployees);  // ONLY employees
            break;
        
        case DirecteurOperationnel:
            pages.append(PageEmployees);
            pages.append(PageCaptures);
            pages.append(PageNavires);
            pages.append(PageQuai);
            pages.append(PageAnalytics);
            pages.append(PageDashboard);
            break;
        
        default:
            break;
    }

    return pages;
}

SessionManager::UserRole SessionManager::getRoleFromUsername(const QString &username) {
    if (username == "Pêcheur") return Pecheur;
    if (username == "Matelot") return Matelot;
    if (username == "Docker") return Docker;
    if (username == "Chef de quai") return ChefDeQuai;
    if (username == "Superviseur") return Superviseur;
    // Additional check for special roles by employee title/note
    if (username.contains("HR") || username.contains("RH")) return HRManager;
    if (username.contains("Director") || username.contains("Directeur")) return DirecteurOperationnel;
    return Pecheur; // Default
}
