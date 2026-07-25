#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

const QString NETWORK_SECRET_KEY = "LuminaDefaultSecretKey123!";

const unsigned short SERVER_PORT = 12345;

const QString SERVER_IP = "127.0.0.1";



// نقش‌های کاربری
enum class UserRole {
    RegularUser,
    Publisher,
    Admin
};

// ژانرهای استاندارد کتاب
enum class BookGenre {
    Fiction,
    SciFi,
    Psychology,
    History,
    Educational,
    Biography,
    Unknown
};

// یک تابع کمکی ساده برای تبدیل Enum به متن جهت نمایش در UI یا دیتابیس
inline QString genreToString(BookGenre genre) {
    switch (genre) {
    case BookGenre::Fiction:     return "Fiction";
    case BookGenre::SciFi:       return "SciFi";
    case BookGenre::Psychology:  return "Psychology";
    case BookGenre::History:     return "History";
    case BookGenre::Educational: return "Educational";
    case BookGenre::Biography:   return "Biography";
    default:                     return "Unknown";
    }
}

// تابع تبدیل متن به Enum (موقع خواندن از دیتابیس)
inline BookGenre stringToGenre(const QString& genreStr) {
    if (genreStr == "Fiction")     return BookGenre::Fiction;
    if (genreStr == "SciFi")       return BookGenre::SciFi;
    if (genreStr == "Psychology")  return BookGenre::Psychology;
    if (genreStr == "History")     return BookGenre::History;
    if (genreStr == "Educational") return BookGenre::Educational;
    if (genreStr == "Biography")   return BookGenre::Biography;
    return BookGenre::Unknown;
}

// inline برای جلوگیری از خطای ناشی از تکرار تابع بر اثر تعدد include
// و همچنین سرعت


const QString ADMIN_USERNAME = "AdminSystem";
const QString ADMIN_PASSWORD_HASH = "defcf100db14c5cc3b00e33cb098642a94af1e3fecc61a1176ca4becca3afce4"; //SystemAdmin@2026


#endif // CONSTANTS_H
