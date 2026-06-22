#ifndef PUBLISHER_H
#define PUBLISHER_H

#include "user.h"
#include <QList>

class Publisher : public User {
private:
    QString companyName;
    QList<int> publishedBookIds;

public:
    Publisher(int id, QString username, QString companyName, bool isBlocked = false);

    QString getCompanyName() const;
    void addPublishedBook(int bookId);
    QList<int> getPublishedBookIds() const;
};

#endif // PUBLISHER_H
