#include "cryptohelper.h"
#include <QCryptographicHash>

// ۱. پیاده‌سازی هش یک‌طرفه پسورد
QString CryptoHelper::hashPassword(const QString& password) {
    QByteArray passwordBytes = password.toUtf8();

    // هش کردن با الگوریتم SHA-256
    QByteArray hashBytes = QCryptographicHash::hash(passwordBytes, QCryptographicHash::Sha256);

    // تبدیل بایت‌ها به یک متن هگزا دسیمال (Hex) قابل ذخیره در دیتابیس
    return QString(hashBytes.toHex());
}

// ۲. رمزنگاری دوطرفه (کلید مشترک)
QByteArray CryptoHelper::encryptData(const QString& plainText, const QString& key) {
    QByteArray output = plainText.toUtf8();
    QByteArray keyBytes = key.toUtf8();

    // اعمال الگوریتم چرخشی XOR با کلید
    for (int i = 0; i < output.size(); ++i) {
        output[i] = output[i] ^ keyBytes[i % keyBytes.size()];
    }

    return output; // خروجی بایت‌های رمزگذاری شده
}

// ۳. رمزگشایی دوطرفه (برگرداندن به حالت اول با همان کلید)
QString CryptoHelper::decryptData(const QByteArray& cipherData, const QString& key) {
    QByteArray output = cipherData;
    QByteArray keyBytes = key.toUtf8();

    // در XOR، اگر دوباره همان عملیات را با همان کلید تکرار کنی، متن اصلی برمی‌گردد
    for (int i = 0; i < output.size(); ++i) {
        output[i] = output[i] ^ keyBytes[i % keyBytes.size()];
    }

    return QString::fromUtf8(output);
}
