#ifndef CRYPTOHELPER_H
#define CRYPTOHELPER_H

#include <QString>
#include <QByteArray>

class CryptoHelper {
public:
    // ۱. هش یک‌طرفه برای پسورد (SHA-256) - غیرقابل بازگشت
    static QString hashPassword(const QString& password);

    // ۲. رمزنگاری دوطرفه ساده (برای پیام‌ها یا دیتای شبکه) - قابل برگشت
    static QByteArray encryptData(const QString& plainText, const QString& key);
    static QString decryptData(const QByteArray& cipherData, const QString& key);
};

#endif // CRYPTOHELPER_H
