#include "admin.h"

Admin::Admin(int id, QString username, bool isBlocked)
    : User(id, username, UserRole::Admin, isBlocked) {}
