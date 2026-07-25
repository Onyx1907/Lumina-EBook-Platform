#ifndef STORAGEUTILS_H
#define STORAGEUTILS_H

#include <QString>
#include <QLabel>

class StorageUtils
{
public:
    // تابع استاتیک برای رندر کردن کاور کتاب
    static void displayBookCover(const QString& absolutePathFromServer, QLabel* labelTarget);
};

#endif // STORAGEUTILS_H
