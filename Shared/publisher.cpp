#include "publisher.h"

Publisher::Publisher(int id, QString username, QString companyName, bool isBlocked)
    : User(id, username, UserRole::Publisher, isBlocked), companyName(companyName) {}

QString Publisher::getCompanyName() const { return companyName; }

void Publisher::addPublishedBook(int bookId) { if (!publishedBookIds.contains(bookId)) publishedBookIds.append(bookId); }

QList<int> Publisher::getPublishedBookIds() const { return publishedBookIds; }
