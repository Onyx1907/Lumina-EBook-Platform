#ifndef REGULARUSER_H
#define REGULARUSER_H

#include "user.h"
#include <QStringList>

class RegularUser : public User {
private:
    QStringList favoriteGenres;

public:
    RegularUser(int id, QString username, bool isBlocked = false);

    void setFavoriteGenres(const QStringList& genres);
    QStringList getFavoriteGenres() const;
};

#endif // REGULARUSER_H
