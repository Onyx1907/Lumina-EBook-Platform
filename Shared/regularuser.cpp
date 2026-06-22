#include "regularuser.h"

RegularUser::RegularUser(int id, QString username, bool isBlocked)
    : User(id, username, UserRole::RegularUser, isBlocked) {}

void RegularUser::setFavoriteGenres(const QStringList& genres) { favoriteGenres = genres; }

QStringList RegularUser::getFavoriteGenres() const { return favoriteGenres; }
