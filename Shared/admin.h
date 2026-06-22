#ifndef ADMIN_H
#define ADMIN_H

#include "user.h"

class Admin : public User {
public:
    Admin(int id, QString username, bool isBlocked = false);
};

#endif // ADMIN_H
