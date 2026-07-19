#ifndef STORAGEUTILS_H
#define STORAGEUTILS_H

#include <QString>
#include <QLabel>

class StorageUtils
{
public:
    // تابع استاتیک برای رندر کردن کاور کتاب
    static void displayBookCover(const QString& absolutePathFromServer, QLabel* labelTarget);

    // تابع استاتیک برای باز کردن PDF کتاب
    static void openBookPdf(const QString& absolutePdfPathFromServer);
};

#endif // STORAGEUTILS_H
