#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QString>
#include <QStringList>
#include "models/employe.h"

/**
 * @class SessionManager
 * @brief Manages user authentication and session state
 * 
 * Stores current logged-in user information and tracks
 * user permissions based on role in a singleton pattern
 */
class SessionManager {
public:
    /**
     * @enum UserRole
     * @brief Role types with hierarchical permissions
     */
    enum UserRole {
        NotLoggedIn = -1,
        Pecheur = 0,          // Employee - views captures
        Matelot = 1,          // Sailor - views ships
        Docker = 2,           // Dockworker - views dock
        ChefDeQuai = 3,       // Quay manager - views dock + employees
        Superviseur = 4,      // Supervisor - views dock + employees + analytics
        HRManager = 5,        // HR Manager - full employee management + analytics
        DirecteurOperationnel = 6  // Operations Director - full access to all
    };

    /**
     * @enum AccessPage
     * @brief Application pages that can be access-controlled
     */
    enum AccessPage {
        PageEmployees = 0,
        PageCaptures = 1,
        PageNavires = 2,
        PageQuai = 3,
        PageAnalytics = 4,
        PageDashboard = 5
    };

    /**
     * @brief Get singleton instance
     */
    static SessionManager& getInstance();

    /**
     * @brief Authenticate user with username and password
     * @param username Login username
     * @param password Plain text password
     * @return true if authentication successful, false otherwise
     */
    bool authenticate(const QString &username, const QString &password);

    /**
     * @brief Logout current user and clear session
     */
    void logout();

    /**
     * @brief Check if user is currently logged in
     * @return true if session is active
     */
    bool isLoggedIn() const;

    /**
     * @brief Get current logged-in user
     * @return User employee record
     */
    Employe getCurrentUser() const;

    /**
     * @brief Get current user's role
     * @return Current user's role enum
     */
    UserRole getCurrentRole() const;

    /**
     * @brief Get current user's role as string
     * @return Role name (e.g., "Directeur Opérationnel")
     */
    QString getCurrentRoleString() const;

    /**
     * @brief Check if current user can access a specific page
     * @param page Page enum to check
     * @return true if user has access to page
     */
    bool canAccessPage(AccessPage page) const;

    /**
     * @brief Get list of accessible pages for current role
     * @return QList of page enums user can access
     */
    QList<AccessPage> getAccessiblePages() const;

    /**
     * @brief Convert username to role (for demonstration)
     * @param username User login name
     * @return UserRole corresponding to username
     */
    static UserRole getRoleFromUsername(const QString &username);

private:
    // Private constructor (singleton pattern)
    SessionManager();
    ~SessionManager();

    // Prevent copying
    SessionManager(const SessionManager&) = delete;
    SessionManager& operator=(const SessionManager&) = delete;

    Employe currentUser;
    UserRole currentRole;
    bool loggedIn;
};

#endif // SESSIONMANAGER_H
