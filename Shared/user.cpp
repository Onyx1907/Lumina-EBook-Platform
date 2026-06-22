#include "user.h"

User::User(int id, QString username, UserRole role, bool isBlocked)
    : id(id), username(username), role(role), isBlocked(isBlocked) {}

int User::getId() const { return id; }

QString User::getUsername() const { return username; }

UserRole User::getRole() const { return role; }

bool User::getIsBlocked() const { return isBlocked; }

void User::setBlockStatus(bool status) { isBlocked = status; }
