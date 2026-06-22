#ifndef USER_H
#define USER_H

#include <QString>
#include "constants.h"

class User {
protected:
    int id;
    QString username;
    UserRole role;
    bool isBlocked;

public:
    User(int id, QString username, UserRole role, bool isBlocked = false);
    virtual ~User() = default;

    int getId() const;
    QString getUsername() const;
    UserRole getRole() const;
    bool getIsBlocked() const;
    void setBlockStatus(bool status);
};

#endif // USER_H
