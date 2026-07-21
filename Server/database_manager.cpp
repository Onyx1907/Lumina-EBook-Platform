#include "database_manager.h"

//*********************************************احراز هویت مرکزی***************************************************


// سازنده کلاس: تنظیمات اولیه پایگاه داده را انجام میدهد
DatabaseManager::DatabaseManager(const QString& connectionName)
{
    db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName("bookclub.db");
}

bool DatabaseManager::initDatabase() {
    if (!db.open()) {
        qDebug() << "db open failed:" << db.lastError().text();
        return false;
    }

    db.exec("PRAGMA journal_mode=WAL;");
    db.exec("PRAGMA synchronous=NORMAL;");

    db.setConnectOptions("QSQLITE_BUSY_TIMEOUT=5000");
    return createTables();
}

// ایجاد جدول های مورد نیاز در پایگاه داده (در صورت عدم وجود)
bool DatabaseManager::createTables(){
    QSqlQuery q(db);
    q.exec("PRAGMA foreign_keys = ON;");

    //--------------جدول کاربران--------------
    QString createUsers =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"                 // شناسه یکتا و خودکار
        "username TEXT UNIQUE NOT NULL,"                       // نام کاربری (یکتا و اجباری)
        "password_hash TEXT NOT NULL,"                        //هش رمز عبور (عدم ذخیره متن خام)
        "role TEXT NOT NULL,"                                  // نقش کاربر در سیستم
        "is_blocked INTEGER NOT NULL DEFAULT 0,"               // وضعیت مسدود بودن (پیش فرض: فعال)
        "security_question TEXT,"                              // سوال امنیتی برای بازیابی رمز
        "security_answer_encrypted BLOB,"                      // پاسخ امنیتی رمزنگاری شده به صورت باینری
        "registration_date TEXT NOT NULL,"                     // تاریخ ثبت نام
        "first_login INTEGER DEFAULT 1,"                       // وضعیت اولین ورود کاربر (پیش فرض ۱ برای اولین ورود)
        "favorite_genres TEXT,"                                // لیست ژانرهای مورد علاقه کاربر به صورت متن
        "name TEXT,"                                           // برای ماژول پروفایل اضافه شد (نام کاربر)
        "email TEXT"                                           // برای ماژول پروفایل اضافه شد (ایمیل کاربر)
        ");";

    if(!q.exec(createUsers)){
        qDebug() << "Create users failed: " << q.lastError().text();
        return false;
    }
    // اضافه کردن ستون حذف منطقی به جدول کاربران (اگر از قبل وجود نداشته باشد)
    if (!q.exec("ALTER TABLE users ADD COLUMN is_deleted INTEGER NOT NULL DEFAULT 0")) {
        // اگر ستون از قبل وجود داشته باشد دیتابیس خطا می‌دهد که طبیعی است، پس برنامه را متوقف نمی کنیم
        qDebug() << "Note: is_deleted column might already exist.";
    }

    //--------------جدول اعلان ها--------------
    QString createNotifications =
        "CREATE TABLE IF NOT EXISTS notifications ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"                                        //شناسه یکتا، عددی و خودکارافزایش (کلید اصلی)
        "user_id INTEGER NOT NULL,"                                                   //شناسه کاربر
        "username TEXT NOT NULL,"                                                    //نام کاربری دریافت کننده اعلان (متنی و اجباری)
        "role TEXT,"                                                                //نقش کاربری (متنی و اختیاری)
        "type TEXT NOT NULL,"                                                      //نوع اعلان مثلاً سیستم، پیام یا هشدار (متنی و اجباری)
        "message TEXT NOT NULL,"                                                  //متن اصلی اعلان (متنی و اجباری)
        "related_id INTEGER,"                                                    //شناسه مرتبط با اعلان مثل شناسه پست یا فاکتور (عددی و اختیاری)
        "is_read INTEGER NOT NULL DEFAULT 0,"                                   //وضعیت خوانده شدن: 0 یعنی خوانده نشده، 1 یعنی خوانده‌شده (پیش فرض 0)
        "created_at TEXT NOT NULL"                                             //تاریخ و زمان ثبت اعلان (متنی و اجباری)
        ");";

    if (!q.exec(createNotifications)) {
        qDebug() << "Create notifications failed:" << q.lastError().text();
        return false;
    }

    //--------------جدول کتاب ها--------------
    QString createBooks =
        "CREATE TABLE IF NOT EXISTS books ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"                                           //شناسه یکتا، کلید اصلی و افزایش خودکار برای هر کتاب
        "title TEXT NOT NULL,"                                                           //عنوان کتاب (اجباری)
        "author TEXT NOT NULL,"                                                         //نام نویسنده (اجباری)
        "genre TEXT,"                                                                  //ژانر یا دسته بندی کتاب (اختیاری)
        "description TEXT,"                                                           //توضیحات یا خلاصه کتاب (اختیاری)
        "price REAL NOT NULL,"                                                       //قیمت اصلی کتاب به صورت عدد اعشاری (اجباری)
        "discountPercent REAL DEFAULT 0,"                                           //درصد تخفیف با مقدار پیش فرض صفر
        "discountAmount REAL DEFAULT 0,"                                           //مبلغ تخفیف با مقدار پیش فرض صفر
        "coverImagePath TEXT,"                                                    //مسیر ذخیره سازی تصویر جلد کتاب (اختیاری)
        "pdfPath TEXT NOT NULL,"                                                 //مسیر ذخیره سازی فایل پی دی اف کتاب (اجباری)
        "publisher_id INTEGER NOT NULL,"                                        //شناسه ناشر متصل به جدول کاربران (اجباری)
        "isActive INTEGER NOT NULL DEFAULT 1,"                                 //وضعیت فعال بودن کتاب (۱ برای فعال، ۰ برای غیرفعال)
        "averageRating REAL DEFAULT 0.0,"
        "ratingCount INTEGER DEFAULT 0,"
        "created_at TEXT DEFAULT (DATETIME('now', 'localtime')),"
        "FOREIGN KEY (publisher_id) REFERENCES users(id)"                 //تعریف کلید خارجی برای اتصال شناسه ناشر به شناسه کاربر در جدول کاربران
        ")";
    if (!q.exec(createBooks)) {
        qDebug() << "Create books failed:" << q.lastError().text();
        return false;
    }
    // اضافه کردن ستون حذف منطقی به جدول کتاب ها (اگر از قبل وجود نداشته باشد)
    if (!q.exec("ALTER TABLE books ADD COLUMN is_deleted INTEGER NOT NULL DEFAULT 0")) {
        qDebug() << "Note: is_deleted column in books might already exist.";
    }

    //--------------جدول کامنت ها--------------
    QString createComments =
        "CREATE TABLE IF NOT EXISTS comments ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"                         //شناسه یکتا، کلید اصلی و افزایشی خودکار برای هر نظر
        "book_id INTEGER NOT NULL,"                                    //شناسه کتابی که نظر برای آن ثبت شده (اجباری)
        "user_id INTEGER NOT NULL,"                                   //شناسه کاربری که نظر را ثبت کرده است (اجباری)
        "comment_text TEXT NOT NULL,"                                //متن اصلی نظر کاربر (متنی و اجباری)
        "rating INTEGER NOT NULL,"                                  //امتیازی که کاربر به کتاب داده (عددی و اجباری)
        "created_at TEXT NOT NULL,"                                //تاریخ و زمان ثبت اولیه نظر (متنی و اجباری)
        "updated_at TEXT NOT NULL,"                               //تاریخ و زمان آخرین ویرایش نظر(متنی و اجباری)
        "FOREIGN KEY (book_id) REFERENCES books(id),"            //اتصال شناسه کتاب به کلید اصلی جدول کتاب ها
        "FOREIGN KEY (user_id) REFERENCES users(id)"            //اتصال شناسه کاربر به کلید اصلی جدول کاربران
        ")";
    if (!q.exec(createComments)) {
        qDebug() << "Create comments failed:" << q.lastError().text();
        return false;
    }

    //--------------جدول سبد خرید--------------
    QString createCart =
        "CREATE TABLE IF NOT EXISTS cart ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"                                        //شناسه یکتا و خودکار برای هر ردیف سبد خرید
        "user_id INTEGER NOT NULL,"                                                   //شناسه کاربر (نمی تواند خالی باشد)
        "book_id INTEGER NOT NULL,"                                                  //شناسه کتاب (نمی تواند خالی باشد)
        "FOREIGN KEY (user_id) REFERENCES users(id),"                               //اتصال شناسه کاربر به جدول کاربران (کلید خارجی)
        "FOREIGN KEY (book_id) REFERENCES books(id)"                               //اتصال شناسه کتاب به جدول کتاب ها (کلید خارجی)
        ")";
    if (!q.exec(createCart)) {
        qDebug() << "Create cart failed:" << q.lastError().text();
        return false;
    }

    //--------------جدول کتابخانه شخصی--------------
    QString createLibrary =
        "CREATE TABLE IF NOT EXISTS library ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"                                             //شناسه یکتا و خودکار برای هر ردیف جدول
        "user_id INTEGER NOT NULL,"                                                        //شناسه کاربر خریدار (اجباری)
        "book_id INTEGER NOT NULL,"                                                       //شناسه کتاب خریداری شده (اجباری)
        "purchase_date TEXT NOT NULL,"                                                   //تاریخ و زمان خرید به صورت متن (اجباری)
        "FOREIGN KEY (user_id) REFERENCES users(id),"                                   //اتصال به جدول کاربران برای احراز هویت خریدار
        "FOREIGN KEY (book_id) REFERENCES books(id)"                                   //اتصال به جدول کتاب ها برای مشخص شدن کالا
        ")";
    if (!q.exec(createLibrary)) {
        qDebug() << "Create library failed:" << q.lastError().text();
        return false;
    }

    //--------------جدول کتاب های ذخیره شده--------------
    QString createSavedBooks =
        "CREATE TABLE IF NOT EXISTS saved_books ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"                                              //شناسه یکتا و خودکار برای هر ردیف
        "user_id INTEGER NOT NULL,"                                                         //شناسه کاربر (اجباری)
        "book_id INTEGER NOT NULL,"                                                        //شناسه کتاب (اجباری)
        "FOREIGN KEY (user_id) REFERENCES users(id),"                                     //اتصال به شناسه در جدول کاربران
        "FOREIGN KEY (book_id) REFERENCES books(id)"                                     //اتصال به شناسه در جدول کتاب ها
        ")";
    if (!q.exec(createSavedBooks)) {
        qDebug() << "Create saved_books failed:" << q.lastError().text();
        return false;
    }

    //--------------جدول قفسه ها--------------
    QString createShelves =
        "CREATE TABLE IF NOT EXISTS shelves ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"                                         //شناسه یکتا و خودکار قفسه
        "user_id INTEGER NOT NULL,"                                                    //شناسه کاربر صاحب قفسه (اجباری)
        "name TEXT NOT NULL,"                                                         //نام قفسه (اجباری)
        "FOREIGN KEY (user_id) REFERENCES users(id)"                                 //اتصال قفسه به شناسه کاربر در جدول کاربران
        ")";
    if (!q.exec(createShelves)) {
        qDebug() << "Create shelves failed:" << q.lastError().text();
        return false;
    }

    //--------------جدول کتاب های قفسه--------------
    QString createShelfBooks =
        "CREATE TABLE IF NOT EXISTS shelf_books ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"                                          //شناسه یکتا و خودکار برای هر ردیف اتصال
        "shelf_id INTEGER NOT NULL,"                                                    //شناسه قفسه مورد نظر (اجباری)
        "book_id INTEGER NOT NULL,"                                                    //شناسه کتاب مورد نظر (اجباری)
        "FOREIGN KEY (shelf_id) REFERENCES shelves(id),"                              //اتصال به شناسه قفسه در جدول قفسه ها
        "FOREIGN KEY (book_id) REFERENCES books(id)"                                 //اتصال به شناسه کتاب در جدول کتاب ها
        ")";
    if (!q.exec(createShelfBooks)) {
        qDebug() << "Create shelf_books failed:" << q.lastError().text();
        return false;
    }

    //--------------جدول آخرین صفحه مطالعه شده--------------
    QString createReadingProgress =
        "CREATE TABLE IF NOT EXISTS reading_progress ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"                                             //شناسه یکتا و خودکار برای هر ردیف
        "user_id INTEGER NOT NULL,"                                                        //شناسه کاربر (اجباری)
        "book_id INTEGER NOT NULL,"                                                       //شناسه کتاب (اجباری)
        "last_page INTEGER NOT NULL,"                                                    //آخرین صفحه مطالعه شده (اجباری)
        "FOREIGN KEY (user_id) REFERENCES users(id),"                                   //اتصال شناسه کاربر به جدول کاربران
        "FOREIGN KEY (book_id) REFERENCES books(id)"                                   //اتصال شناسه کتاب به جدول کتاب ها
        ")";
    if (!q.exec(createReadingProgress)) {
        qDebug() << "Create reading_progress failed:" << q.lastError().text();
        return false;
    }


    QSqlQuery walQuery(db);
    if (walQuery.exec("PRAGMA journal_mode=WAL;")) {
        qDebug() << "SQLite WAL mode activated successfully!";
    } else {
        qDebug() << "Failed to activate WAL mode:" << walQuery.lastError().text();
    }


    return true;
}
// بررسی تکراری نبودن نام کاربری هنگام ثبت نام
bool DatabaseManager::isUsernameTaken(const QString& username){
    QSqlQuery q(db);
    q.prepare("SELECT id FROM users WHERE username = :u");
    q.bindValue(":u", username);
    if(!q.exec()) return false;
    return q.next();
}
// ثبت نام کاربر جدید در سامانه
bool DatabaseManager::registerUser(const QString& username,const QString& plainPassword,UserRole role,
                                   const QString& securityQuestion,const QString& securityAnswerPlain){

    if(isUsernameTaken(username)) return false;
    QString passwordHash = CryptoHelper::hashPassword(plainPassword);
    QByteArray encryptedAnswer =CryptoHelper::encryptData(securityAnswerPlain, NETWORK_SECRET_KEY);

    QString now = QDateTime::currentDateTime().toString(Qt::ISODate);
    QString roleStr;
    if (role == UserRole::Admin) {
        roleStr = "Admin";
    } else if (role == UserRole::Publisher) {
        roleStr = "Publisher";
    } else {
        roleStr = "RegularUser";
    }

    QSqlQuery q(db);
    q.prepare("INSERT INTO users "
              "(username, password_hash, role, is_blocked, security_question, security_answer_encrypted,  registration_date, first_login) "
              "VALUES (:u, :ph, :r, 0, :sq, :sa, :rd, 1)");

    q.bindValue(":u", username);
    q.bindValue(":ph", passwordHash);
    q.bindValue(":r", roleStr);
    q.bindValue(":sq", securityQuestion);
    q.bindValue(":sa", encryptedAnswer);
    q.bindValue(":rd", now);

    if(!q.exec()){
        qDebug() << "Register failed: " << q.lastError().text();
        return false;
    }
    return true;
}
// احراز هویت کاربر هنگام ورود به سیستم
bool DatabaseManager::verifyUser(const QString& username, const QString& plainPassword,
                                 UserRole& outRole, bool& outIsBlocked, int& outUserId, int& outFirstLogin)
{
    QSqlQuery q(db);
    q.prepare("SELECT id, password_hash, role, is_blocked, first_login FROM users WHERE username = :u");
    q.bindValue(":u", username);

    if (!q.exec()) return false;
    if (!q.next()) return false;

    outUserId = q.value("id").toInt();
    QString storedHash = q.value("password_hash").toString();

    QString roleStr = q.value("role").toString();
    if (roleStr == "Admin") {
        outRole = UserRole::Admin;
    } else if (roleStr == "Publisher") {
        outRole = UserRole::Publisher;
    } else {
        outRole = UserRole::RegularUser;
    }

    outIsBlocked = q.value("is_blocked").toInt() != 0;
    outFirstLogin = q.value("first_login").toInt();

    if(outIsBlocked) return false;

    QString inputHash = CryptoHelper::hashPassword(plainPassword);
    return (inputHash == storedHash);
}

//اولین ورود
bool DatabaseManager::setFirstLoginFalse(int userId)
{
    QSqlQuery q(db);
    q.prepare("UPDATE users SET first_login = 0 WHERE id = :id");
    q.bindValue(":id", userId);
    return q.exec();
}

// دریافت سوال امنیتی کاربر برای فرآیند بازیابی رمز عبور
bool DatabaseManager::getSecurityQuestion(const QString& username,QString& outQuestion){
    QSqlQuery q(db);
    q.prepare("SELECT security_question FROM users WHERE username = :u");
    q.bindValue(":u", username);
    if (!q.exec()) return false;
    if (!q.next()) return false;
    outQuestion = q.value(0).toString();
    return true;
}
// تایید پاسخ امنیتی و تغییر رمز عبور در صورت صحت اطلاعات
bool DatabaseManager::verifySecurityAnswerAndResetPassword(const QString& username, const QString& answerPlain, const QString& newPlainPassword){
    QSqlQuery q(db);
    q.prepare("SELECT security_answer_encrypted FROM users WHERE username = :u");
    q.bindValue(":u", username);
    if (!q.exec()) return false;
    if (!q.next()) return false;

    QByteArray dbEncrypted = q.value(0).toByteArray();

    QByteArray inputEncrypted = CryptoHelper::encryptData(answerPlain, NETWORK_SECRET_KEY);

    // مقایسه باینری دو مقدار رمزگذاری شده
    if (dbEncrypted != inputEncrypted) {
        return false;
    }

    QString newHash = CryptoHelper::hashPassword(newPlainPassword);

    QSqlQuery q2(db);
    q2.prepare("UPDATE users SET password_hash = :ph WHERE username = :u");
    q2.bindValue(":ph", newHash);
    q2.bindValue(":u", username);
    return q2.exec();

}


//*********************************************پنل کاربر عادی ( ماژول 1 )****************************************************


bool DatabaseManager::setFirstLoginFalseByUsername(const QString& username)
{
    QSqlQuery q(db);
    q.prepare("UPDATE users SET first_login = 0 WHERE username = :u");
    q.bindValue(":u", username);
    return q.exec();
}

//تایید موفقیت یا شکست عملیات ذخیره لیست ژانرها برای یک کاربر خاص
bool DatabaseManager::setFavoriteGenres(const QString& username, const QStringList& genres){
    QJsonArray arr;
    for(const QString& g : genres)
        arr.append(g);
    QJsonDocument doc(arr);
    QString json = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));

    QSqlQuery q(db);
    q.prepare("UPDATE users SET favorite_genres = :g WHERE username = :u");
    q.bindValue(":g", json);
    q.bindValue(":u", username);
    return q.exec();
}
// بازیابی و استخراج ژانرهای مورد علاقه کاربر به صورت لیست متنی در سی پلاس پلاس
QStringList DatabaseManager::getFavoriteGenres(int userId){
    QStringList result;
    QSqlQuery q(db);
    q.prepare("SELECT favorite_genres FROM users WHERE id = :id");
    q.bindValue(":id", userId);
    if (!q.exec() || !q.next())
        return result;

    const QString json = q.value(0).toString();
    if(json.isEmpty())
        return result;

    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    if(!doc.isArray())
        return result;

    for(const QJsonValue& v : doc.array())
        result.append(v.toString());
    return result;
}
//JSON تابع کمکی استاتیک برای نگاشت و تبدیل مستقیم یک ردیف از جدول کتاب به شیء متنی

static QJsonObject bookFromQuery(const QSqlQuery& q) {
    QJsonObject obj;
    obj["id"] = q.value("id").toInt();
    obj["title"] = q.value("title").toString();
    obj["author"] = q.value("author").toString();
    obj["genre"] = q.value("genre").toString();
    obj["price"] = q.value("price").toDouble();
    obj["discount_percentage"] = q.value("discountPercent").toDouble();
    obj["cover_image_path"] = q.value("coverImagePath").toString();
    obj["publisher_name"] = q.value("publisher_name").toString();

    return obj;
}

static QJsonObject bookFromQueryWithoutPdf(const QSqlQuery& q) {
    QJsonObject obj;
    obj["id"] = q.value("id").toInt();
    obj["title"] = q.value("title").toString();
    obj["author"] = q.value("author").toString();
    obj["genre"] = q.value("genre").toString();
    obj["price"] = q.value("price").toDouble();
    obj["discount_percentage"] = q.value("discountPercent").toDouble();
    obj["cover_image_path"] = q.value("coverImagePath").toString();

    return obj;
}

// دریافت لیست کتاب های پیشنهادی بر اساس لیستی از ژانرهای ورودی کاربر
QList<QJsonObject> DatabaseManager::getRecommendedBooks(const QStringList& genres){
    QList<QJsonObject> list;
    if(genres.isEmpty())
        return list;

    QStringList placeholders;
    for (int i = 0; i < genres.size(); ++i)
        placeholders << QString(":g%1").arg(i);

    QString sql = QString("SELECT * FROM books WHERE genre IN (%1) AND isActive = 1 LIMIT 20").arg(placeholders.join(","));
    QSqlQuery q(db);
    q.prepare(sql);
    for (int i = 0; i < genres.size(); ++i)
        q.bindValue(QString(":g%1").arg(i), genres[i]);
    if(!q.exec())
        return list;
    while(q.next())
        list.append(bookFromQueryWithoutPdf(q));
    return list;
}

// فیلتراسیون و دریافت کتاب ها بر اساس یک ژانر مشخص شده
QList<QJsonObject> DatabaseManager::getBooksByGenre(const QString& genre) {
    QList<QJsonObject> list;

    QString sql = "SELECT b.*, u.name AS publisher_name "
                  "FROM books b "
                  "LEFT JOIN users u ON b.publisher_id = u.id "
                  "WHERE b.genre = :g AND b.isActive = 1 "
                  "LIMIT 20";

    QSqlQuery q(db);
    q.prepare(sql);
    q.bindValue(":g", genre);

    if (!q.exec())
        return list;

    while (q.next()) {
        list.append(bookFromQuery(q));
    }
    return list;
}
// بازیابی لیست تمام کتاب های نشانه گذاری شده به عنوان محبوب
QList<QJsonObject> DatabaseManager::getPopularBooks(){
    QList<QJsonObject> list;
    QSqlQuery q(db);
    q.prepare("SELECT * FROM books WHERE isActive = 1 ORDER BY averageRating DESC LIMIT 20");
    if(!q.exec())
        return list;
    while(q.next())
        list.append(bookFromQueryWithoutPdf(q));
    return list;
}
// بازیابی لیست تمام کتاب های تازه اضافه شده به سیستم
QList<QJsonObject> DatabaseManager::getNewBooks(){
    QList<QJsonObject> list;
    QSqlQuery q(db);
    q.prepare("SELECT * FROM books WHERE isActive = 1 ORDER BY created_at DESC LIMIT 20");
    if(!q.exec())
        return list;
    while(q.next())
        list.append(bookFromQueryWithoutPdf(q));
    return list;
}
// بازیابی لیست پرفروش ترین کتاب های موجود در پایگاه داده
QList<QJsonObject> DatabaseManager::getBestsellers(){
    QList<QJsonObject> list;
    QSqlQuery q(db);
    q.prepare("SELECT b.*, COUNT(l.id) AS salesCount "
              "FROM books b "
              "LEFT JOIN library l ON l.book_id = b.id "
              "WHERE b.isActive = 1 "
              "GROUP BY b.id "
              "ORDER BY salesCount DESC "
              "LIMIT 20");
    if(!q.exec())
        return list;
    while(q.next())
        list.append(bookFromQueryWithoutPdf(q));
    return list;
}
// دریافت لیست کتاب هایی که به صورت رایگان در اختیار کاربران قرار دارند
QList<QJsonObject> DatabaseManager::getFreeBooks(){
    QList<QJsonObject> list;
    QSqlQuery q(db);
    q.prepare("SELECT * FROM books WHERE price = 0 AND isActive = 1 LIMIT 20");
    if(!q.exec())
        return list;
    while(q.next())
        list.append(bookFromQueryWithoutPdf(q));
    return list;
}
//JSON دریافت و تجمیع اطلاعات پروفایل شخصی کاربر در قالب یک شیء خلاصه شده
QJsonObject DatabaseManager::getUserProfile(int userId) {
    QJsonObject obj;
    QSqlQuery q(db);

    // فقط فیلدهای مورد نیاز را بر اساس آیدی واکشی میکنیم
    q.prepare("SELECT name, email, favorite_genres FROM users WHERE id = :id");
    q.bindValue(":id", userId);

    if (!q.exec() || !q.next())
        return obj;

    obj["name"] = q.value("name").toString();
    obj["email"] = q.value("email").toString();

    const QString json = q.value("favorite_genres").toString();
    QJsonArray genresArr;
    if (!json.isEmpty()) {
        QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
        if (doc.isArray())
            genresArr = doc.array();
    }
    obj["favorite_genres"] = genresArr;

    obj["total_purchases"] = getTotalPurchases(userId);

    return obj;
}
//آپدیت پروفایل
bool DatabaseManager::updateUserProfile(int userId, const QString& newUsername, const QString& name, const QString& email) {
    QString trimmedUsername = newUsername.trimmed();
    QString trimmedName = name.trimmed();
    QString trimmedEmail = email.trimmed();

    //بررسی تکراری نبودن یوزرنیم (فقط اگر یوزرنیم جدیدی وارد شده باشد)
    if (!trimmedUsername.isEmpty()) {
        QSqlQuery checkUsername(db);
        checkUsername.prepare("SELECT 1 FROM users WHERE username = :username AND id != :id LIMIT 1");
        checkUsername.bindValue(":username", trimmedUsername);
        checkUsername.bindValue(":id", userId);
        if (checkUsername.exec() && checkUsername.next()) {
            qDebug() << "Database Error: Username is already taken!";
            return false;
        }
    }

    //بررسی تکراری نبودن ایمیل (فقط اگر ایمیل جدیدی وارد شده باشد)
    if (!trimmedEmail.isEmpty()) {
        QSqlQuery checkEmail(db);
        checkEmail.prepare("SELECT 1 FROM users WHERE email = :email AND id != :id LIMIT 1");
        checkEmail.bindValue(":email", trimmedEmail);
        checkEmail.bindValue(":id", userId);
        if (checkEmail.exec() && checkEmail.next()) {
            qDebug() << "Database Error: Email is already taken!";
            return false;
        }
    }

    //ساخت کاملاً پویا و دینامیک کوئری UPDATE
    QStringList updateFields;

    if (!trimmedUsername.isEmpty()) {
        updateFields.append("username = :u");
    }
    if (!trimmedName.isEmpty()) {
        updateFields.append("name = :n");
    }
    if (!trimmedEmail.isEmpty()) {
        updateFields.append("email = :e");
    }

    // اگر کاربر دکمه تایید را زده ولی عملاً هیچ فیلدی را تغییر نداده باشد
    if (updateFields.isEmpty()) {
        return true;
    }

    QString queryStr = "UPDATE users SET " + updateFields.join(", ") + " WHERE id = :id";

    QSqlQuery q(db);
    q.prepare(queryStr);

    // بایند کردن هوشمند مقادیر
    if (!trimmedUsername.isEmpty()) q.bindValue(":u", trimmedUsername);
    if (!trimmedName.isEmpty())     q.bindValue(":n", trimmedName);
    if (!trimmedEmail.isEmpty())    q.bindValue(":e", trimmedEmail);
    q.bindValue(":id", userId);

    if (!q.exec()) {
        qDebug() << "Database Query Failed:" << q.lastError().text();
        return false;
    }

    return true;
}

// فرآیند احراز هویت رمز عبور فعلی و ثبت رمز عبور جدید به صورت هش شده
bool DatabaseManager::changePassword(int userId,const QString& oldPasswordPlain,const QString& newPasswordPlain){
    QSqlQuery q(db);
    q.prepare("SELECT password_hash FROM users WHERE id = :id");
    q.bindValue(":id", userId);
    if (!q.exec() || !q.next())
        return false;

    const QString currentHash = q.value(0).toString();
    if (currentHash != CryptoHelper::hashPassword(oldPasswordPlain))
        return false;

    const QString newHash = CryptoHelper::hashPassword(newPasswordPlain);
    QSqlQuery q2(db);
    q2.prepare("UPDATE users SET password_hash = :p WHERE id = :id");
    q2.bindValue(":p", newHash);
    q2.bindValue(":id", userId);
    return q2.exec();
}
// استخراج تاریخچه کامل کتاب های خریداری شده توسط کاربر به همراه جزئیات فاکتور
QList<QJsonObject> DatabaseManager::getPurchaseHistory(int userId){
    QList<QJsonObject> list;
    QSqlQuery q(db);
    q.prepare("SELECT p.book_id, p.purchase_date, b.title, b.author, b.price "
              "FROM purchases p "
              "JOIN books b ON p.book_id = b.id "
              "WHERE p.user_id = :userId "
              "ORDER BY p.purchase_date DESC");

    q.bindValue(":userId", userId);
    if (!q.exec())
        return list;

    while (q.next()) {
        QJsonObject obj;
        obj["book_id"] = q.value("book_id").toInt();
        obj["purchase_date"] = q.value("purchase_date").toString();
        obj["title"] = q.value("title").toString();
        obj["author"] = q.value("author").toString();
        obj["price"] = q.value("price").toDouble();
        list.append(obj);
    }
    return list;
}
// محاسبه و شمارش تعداد کل فاکتورهای ثبت شده برای یک کاربر مشخص
int DatabaseManager::getTotalPurchases(int userId) {
    QSqlQuery q(db);

    q.prepare("SELECT COUNT(*) FROM purchases WHERE user_id = :userId");
    q.bindValue(":userId", userId);

    if (!q.exec() || !q.next())
        return 0;

    return q.value(0).toInt();
}

//بررسی اینکه کاربر کتاب را خریداری کرده و در کتابخانه شخصی اش دارد یا خیر
bool DatabaseManager::isBookPurchased(int userId, int bookId)
{
    QSqlQuery q(db);
    q.prepare("SELECT 1 FROM library WHERE user_id = :userId AND book_id = :bookId LIMIT 1");
    q.bindValue(":userId", userId);
    q.bindValue(":bookId", bookId);

    return (q.exec() && q.next());
}

// بررسی اینکه آیا کتاب در سبد خرید هست یا خیر
bool DatabaseManager::isBookInCart(int userId, int bookId) {
    QSqlQuery q(db);
    q.prepare("SELECT 1 FROM cart WHERE user_id = :u AND book_id = :b LIMIT 1");
    q.bindValue(":u", userId);
    q.bindValue(":b", bookId);

    if (q.exec() && q.next()) {
        return true; // کتاب در سبد خرید وجود دارد
    }
    return false; // کتاب در سبد خرید نیست
}

bool DatabaseManager::isBookSaved(int userId, int bookId)
{
    QSqlQuery q(db);
    q.prepare("SELECT 1 FROM saved_books WHERE user_id = :userId AND book_id = :bookId LIMIT 1");
    q.bindValue(":userId", userId);
    q.bindValue(":bookId", bookId);

    return (q.exec() && q.next());
}

//بررسی اکتیو بودن کتاب و گرفتن اطلاعات ناشر و ریتینگ از جدول
bool DatabaseManager::getActiveBookDetails(int bookId, QString &publisherName, double &rating, QString &coverPath, QString &description)
{
    QSqlQuery q(db);
    // اضافه کردن ستون آدرس عکس کتاب به کوئری
    q.prepare("SELECT u.name, b.averageRating, b.coverImagePath, b.description "
              "FROM books b "
              "JOIN users u ON b.publisher_id = u.id "
              "WHERE b.id = :bookId AND b.isActive = 1 AND b.is_deleted = 0 LIMIT 1");
    q.bindValue(":bookId", bookId);

    if (q.exec() && q.next()) {
        publisherName = q.value("name").toString();
        rating = q.value("averageRating").toDouble();
        coverPath = q.value("coverImagePath").toString(); // استخراج آدرس عکس از دیتابیس
        description = q.value("description").toString();
        return true;
    }
    return false;
}

//گرفتن مسیر فیزیکی فایل پی  دی اف از جدول کتاب ها
QString DatabaseManager::getBookPdfPath(int bookId)
{
    QSqlQuery q(db);
    q.prepare("SELECT pdfPath FROM books WHERE id = :bookId AND is_deleted = 0 LIMIT 1");
    q.bindValue(":bookId", bookId);

    if (q.exec() && q.next()) {
        return q.value("pdfPath").toString();
    }
    return "";
}


//*********************************************پنل کاربر عادی ( ماژول 2 )****************************************************


//تابع جستوجوی کتاب
QList<QJsonObject> DatabaseManager::searchBooks(const QString& title, const QString& author, const QString& publisherName) {
    QList<QJsonObject> list;
    QSqlQuery q(db);

    QString queryStr = "SELECT b.id, b.title, b.author, b.genre, b.description, b.price, "
                       "b.discountPercent, b.discountAmount, b.coverImagePath, b.pdfPath, b.publisher_id, b.isActive, "
                       "IFNULL(NULLIF(u.name, ''), u.username) AS publisher_name "
                       "FROM books b "
                       "JOIN users u ON b.publisher_id = u.id "
                       "WHERE b.isActive = 1";

    if (!title.isEmpty()) queryStr += " AND b.title LIKE :title";
    if (!author.isEmpty()) queryStr += " AND b.author LIKE :author";
    if (!publisherName.isEmpty()) queryStr += " AND (u.username LIKE :pub OR u.name LIKE :pub)";

    q.prepare(queryStr);
    if (!title.isEmpty()) q.bindValue(":title", "%" + title + "%");
    if (!author.isEmpty()) q.bindValue(":author", "%" + author + "%");
    if (!publisherName.isEmpty()) q.bindValue(":pub", "%" + publisherName + "%");

    if (!q.exec()) return list;

    while (q.next()) {
        // ابتدا اطلاعات پایه کتاب را از تابع مپینگ بدون دستکاری میگیریم
        QJsonObject obj = bookFromQueryWithoutPdf(q);

        //تزریق منطقی نام ناشر به شیء جیسون کتاب
        obj["publisher_name"] = q.value("publisher_name").toString();

        list.append(obj);
    }
    return list;
}

//*********************************************پنل کاربر عادی ( ماژول 3 )****************************************************


//اضافه کردن نظر
bool DatabaseManager::addComment(int bookId, int userId,
                                 const QString& text, int rating) {

    // بررسی وضعیت زنده بودن کتاب و مجاز بودن کاربر به صورت همزمان
    QSqlQuery qCheck(db);
    qCheck.prepare("SELECT 1 FROM books b, users u "
                   "WHERE b.id = :b AND b.isActive = 1 "
                   "AND u.id = :u AND u.is_blocked = 0 AND u.is_deleted = 0");
    qCheck.bindValue(":b", bookId);
    qCheck.bindValue(":u", userId);

    if (!qCheck.exec() || !qCheck.next()) {
        qDebug() << "Comment blocked: Book is inactive/deleted OR user is blocked/deleted.";
        return false; // اجازه ثبت نظر داده نمیشود
    }

    const QString now = QDateTime::currentDateTime().toString(Qt::ISODate);

    QSqlQuery q(db);
    q.prepare("INSERT INTO comments (book_id, user_id, comment_text, rating, created_at, updated_at) "
              "VALUES (:b, :u, :t, :r, :c, :u2)");
    q.bindValue(":b", bookId);
    q.bindValue(":u", userId);
    q.bindValue(":t", text);
    q.bindValue(":r", rating);
    q.bindValue(":c", now);
    q.bindValue(":u2", now);

    if (!q.exec())
        return false;

    return recalculateBookRating(bookId);
}

//ویرایش نظر
bool DatabaseManager::editComment(int commentId,
                                  const QString& newText, int newRating) {

    QSqlQuery qGet(db);
    qGet.prepare("SELECT c.book_id FROM comments c "
                 "JOIN books b ON c.book_id = b.id "
                 "JOIN users u ON c.user_id = u.id "
                 "WHERE c.id = :id AND b.isActive = 1 AND u.is_blocked = 0 AND u.is_deleted = 0");
    qGet.bindValue(":id", commentId);

    if (!qGet.exec() || !qGet.next()) {
        qDebug() << "Edit blocked: Associated book is inactive OR user is blocked/deleted.";
        return false;
    }

    int bookId = qGet.value(0).toInt();
    const QString now = QDateTime::currentDateTime().toString(Qt::ISODate);  //این خط کد، زمان و تاریخ فعلی سیستم را میگیرد
    //و آن را به یک متن (رشته) استاندارد و قابل فهم برای کامپیوتر تبدیل میکند

    QSqlQuery q(db);
    q.prepare("UPDATE comments SET comment_text = :t, rating = :r, updated_at = :u WHERE id = :id");
    q.bindValue(":t", newText);
    q.bindValue(":r", newRating);
    q.bindValue(":u", now);
    q.bindValue(":id", commentId);

    if (!q.exec())
        return false;

    return recalculateBookRating(bookId);
}

//حذف نظر
bool DatabaseManager::deleteComment(int commentId) {
    QSqlQuery qGet(db);
    qGet.prepare("SELECT book_id FROM comments WHERE id = :id");
    qGet.bindValue(":id", commentId);
    if (!qGet.exec() || !qGet.next())
        return false;

    int bookId = qGet.value(0).toInt();

    QSqlQuery q(db);
    q.prepare("DELETE FROM comments WHERE id = :id");
    q.bindValue(":id", commentId);

    if (!q.exec())
        return false;

    return recalculateBookRating(bookId);
}

//دیدن نظرات یک کتاب
QList<QJsonObject> DatabaseManager::getCommentsForBook(int bookId) {
    QList<QJsonObject> list;

    QSqlQuery q(db);
    q.prepare("SELECT c.id, c.comment_text, c.rating, c.created_at, c.updated_at, "
               "u.username, c.user_id "
               "FROM comments c "
               "JOIN users u ON c.user_id = u.id "
               "WHERE c.book_id = :b AND u.is_deleted = 0 "
               "ORDER BY c.created_at DESC");
    q.bindValue(":b", bookId);

    if (!q.exec())
        return list;

    while (q.next()) {
        QJsonObject obj;
        obj["id"] = q.value("id").toInt();
        obj["user_id"] = q.value("user_id").toInt();
        obj["text"] = q.value("comment_text").toString();
        obj["rating"] = q.value("rating").toInt();
        obj["created_at"] = q.value("created_at").toString();
        obj["updated_at"] = q.value("updated_at").toString();
        obj["username"] = q.value("username").toString();
        list.append(obj);
    }

    return list;
}

//محاسبه امتیاز کتاب
bool DatabaseManager::recalculateBookRating(int bookId) {
    QSqlQuery q(db);
    // محاسبه امتیازها فقط از روی کاربران حذف نشده سیستم
    q.prepare("SELECT c.rating FROM comments c "
              "JOIN users u ON c.user_id = u.id "
              "WHERE c.book_id = :b AND u.is_deleted = 0");
    q.bindValue(":b", bookId);

    if (!q.exec())
        return false;

    int count = 0;
    double sum = 0.0;
    while (q.next()) {
        sum += q.value(0).toInt();
        ++count;
    }

    double avg = (count == 0) ? 0.0 : (sum / count);

    QSqlQuery qUpdate(db);
    qUpdate.prepare("UPDATE books SET averageRating = :a, ratingCount = :c WHERE id = :b");
    qUpdate.bindValue(":a", avg);
    qUpdate.bindValue(":c", count);
    qUpdate.bindValue(":b", bookId);

    return qUpdate.exec();
}

//*********************************************پنل کاربر عادی ( ماژول 4 )****************************************************


//افزودن به سبد خرید
bool DatabaseManager::addToCart(int userId, int bookId) {
    QSqlQuery q(db);
    q.prepare("INSERT INTO cart (user_id, book_id) "
              "SELECT :u, id FROM books "
              "WHERE id = :b AND isActive = 1");
    q.bindValue(":u", userId);
    q.bindValue(":b", bookId);
    return q.exec();
}

//حذف از سبد خرید
bool DatabaseManager::removeFromCart(int userId, int bookId) {
    QSqlQuery q(db);
    q.prepare("DELETE FROM cart WHERE user_id = :u AND book_id = :b");
    q.bindValue(":u", userId);
    q.bindValue(":b", bookId);
    return q.exec();
}

//گرفتن یک لیست از سبد خرید
QList<QJsonObject> DatabaseManager::getCartItems(int userId) {
    QList<QJsonObject> list;

    QSqlQuery q(db);

    q.prepare("SELECT b.id, b.title, b.author, b.price, b.discountPercent, b.coverImagePath, b.isActive, u.name AS publisher_name "
              "FROM cart c "
              "JOIN books b ON c.book_id = b.id "
              "JOIN users u ON b.publisher_id = u.id "
              "WHERE c.user_id = :u AND b.is_deleted = 0 AND b.isActive = 1");
    q.bindValue(":u", userId);

    if (!q.exec())
        return list;

    while (q.next()) {
        QJsonObject obj;
        obj["id"] = q.value("id").toInt();
        obj["title"] = q.value("title").toString();
        obj["author"] = q.value("author").toString();
        obj["price"] = q.value("price").toDouble();
        obj["discount"] = q.value("discountPercent").toDouble();
        obj["coverImagePath"] = q.value("coverImagePath").toString(); // مسیر نسبی عکس
        obj["publisher_name"] = q.value("publisher_name").toString();

        // فرستادن وضعیت دسترسی به کلاینت (۱ یعنی موجود، ۰ یعنی مسدود/حذف شده)
        obj["isActive"] = (q.value("isActive").toInt() == 1);

        list.append(obj);
    }

    return list;
}

//پاک کردن سبد خرید
bool DatabaseManager::clearCart(int userId) {
    QSqlQuery q(db);
    q.prepare("DELETE FROM cart WHERE user_id = :u");
    q.bindValue(":u", userId);
    return q.exec();
}

//نهایی کردن خرید
bool DatabaseManager::finalizePurchase(int userId, double clientFinalPrice) {
    // به دیتابیس میگوییم: یک تراکنش امن باز کن
    if (!db.transaction()) return false;

    //بررسی وجود کتاب مسدود یا حذف منطقی شده در سبد کاربر
    QSqlQuery qCheck(db);
    qCheck.prepare("SELECT COUNT(*) FROM cart c "
                   "JOIN books b ON c.book_id = b.id "
                   "WHERE c.user_id = :u AND (b.isActive = 0 OR b.is_deleted = 1)");
    qCheck.bindValue(":u", userId);

    if (!qCheck.exec() || !qCheck.next()) {
        db.rollback();
        return false;
    }

    if (qCheck.value(0).toInt() > 0) {
        qDebug() << "Purchase failed: Inactive or deleted books detected.";
        db.rollback();
        return false;
    }

    //محاسبه مجموع قیمت زنده دیتابیس برای این سبد خرید
    QSqlQuery qPrice(db);
    qPrice.prepare("SELECT SUM(b.price - (b.price * (b.discountPercent / 100.0))) "
                   "FROM cart c "
                   "JOIN books b ON c.book_id = b.id "
                   "WHERE c.user_id = :u");
    qPrice.bindValue(":u", userId);

    if (!qPrice.exec() || !qPrice.next()) {
        db.rollback();
        return false;
    }

    double dbFinalPrice = qPrice.value(0).toDouble();

    // مقایسه قیمت کلاینت با قیمت واقعی دیتابیس (با احتساب خطای اعشار در دابل)
    if (qAbs(clientFinalPrice - dbFinalPrice) > 0.01) {
        qDebug() << "Purchase failed: Prices do not match. DB:" << dbFinalPrice << "Client:" << clientFinalPrice;
        db.rollback(); // لغو تراکنش به دلیل تغییر قیمت توسط ناشر
        return false;
    }

    // خواندن کتاب های داخل سبد خرید
    QSqlQuery q(db);
    q.prepare("SELECT book_id FROM cart WHERE user_id = :u");
    q.bindValue(":u", userId);

    if (!q.exec()) {
        db.rollback();
        return false;
    }

    const QString now = QDateTime::currentDateTime().toString(Qt::ISODate);

    // شروع حلقه روی کتاب های سبد خرید
    while (q.next()) {
        int bookId = q.value(0).toInt();

        // بررسی اینکه کاربر قبلاً این کتاب را نخریده باشد
        QSqlQuery check(db);
        check.prepare("SELECT 1 FROM library WHERE user_id = :u AND book_id = :b");
        check.bindValue(":u", userId);
        check.bindValue(":b", bookId);

        if (check.exec() && check.next()) {
            continue;
        }

        // درج کتاب در کتابخانه کاربر
        QSqlQuery insert(db);
        insert.prepare("INSERT INTO library (user_id, book_id, purchase_date) "
                       "VALUES (:u, :b, :d)");
        insert.bindValue(":u", userId);
        insert.bindValue(":b", bookId);
        insert.bindValue(":d", now);

        if (!insert.exec()) {
            db.rollback();
            return false;
        }
    }

    // پاک کردن سبد خرید بعد از اتمام حلقه
    if (!clearCart(userId)) {
        db.rollback();
        return false;
    }

    // همه چیز عالی بود! حالا تغییرات را ماندگار کن
    return db.commit();
}


//*********************************************پنل کاربر عادی ( ماژول 5 )****************************************************


//+++++کتاب های خریداری شده+++++

//دیدن کتاب های خریده شده
QList<QJsonObject> DatabaseManager::getPurchasedBooks(int userId) {
    QList<QJsonObject> list;
    QSqlQuery q(db);

    // انتخاب دقیق فیلدهای مسیر فیزیکی کتاب از دیتابیس
    q.prepare("SELECT b.id, b.title, b.author, b.genre, b.pdfPath, b.coverImagePath "
              "FROM library l "
              "JOIN books b ON l.book_id = b.id "
              "WHERE l.user_id = :u");
    q.bindValue(":u", userId);

    if (!q.exec()) {
        qDebug() << "getPurchasedBooks failed:" << q.lastError().text();
        return list;
    }

    while (q.next()) {
        QJsonObject o;
        o["id"] = q.value("id").toInt();
        o["title"] = q.value("title").toString();
        o["author"] = q.value("author").toString();
        o["genre"] = q.value("genre").toString();
        o["pdfPath"] = q.value("pdfPath").toString();                 // فیلد فیزیکی هارد سرور
        o["coverImagePath"] = q.value("coverImagePath").toString();   // فیلد فیزیکی هارد سرور
        list.append(o);
    }
    return list;
}

//+++++کتاب های ذخیره شده+++++

//ذخیره کتاب
bool DatabaseManager::saveBook(int userId, int bookId) {
    QSqlQuery q(db);
    q.prepare("INSERT INTO saved_books (user_id, book_id) VALUES (:u, :b)");
    q.bindValue(":u", userId);
    q.bindValue(":b", bookId);
    return q.exec();
}

//حذف کتاب ذخیره شده
bool DatabaseManager::removeSavedBook(int userId, int bookId) {
    QSqlQuery q(db);
    q.prepare("DELETE FROM saved_books WHERE user_id = :u AND book_id = :b");
    q.bindValue(":u", userId);
    q.bindValue(":b", bookId);
    return q.exec();
}

//دیدن لیستی از کتاب های ذخیره شده
QList<QJsonObject> DatabaseManager::getSavedBooks(int userId) {
    QList<QJsonObject> list;

    QSqlQuery q(db);

    q.prepare("SELECT b.id, b.title, b.author, b.genre, b.coverImagePath, "
              "b.price, b.discountPercent, u.name AS publisher_name "
              "FROM saved_books s "
              "JOIN books b ON s.book_id = b.id "
              "JOIN users u ON b.publisher_id = u.id "
              "WHERE s.user_id = :u AND b.is_deleted = 0");
    q.bindValue(":u", userId);

    if (!q.exec()) {
        qDebug() << "getSavedBooks failed:" << q.lastError().text();
        return list;
    }

    while (q.next()) {
        QJsonObject o;
        o["id"] = q.value("id").toInt();
        o["title"] = q.value("title").toString();
        o["author"] = q.value("author").toString();
        o["genre"] = q.value("genre").toString();
        o["coverImagePath"] = q.value("coverImagePath").toString();
        o["price"] = q.value("price").toDouble();
        o["discount"] = q.value("discountPercent").toDouble();
        o["publisher_name"] = q.value("publisher_name").toString();

        list.append(o);
    }

    return list;
}

//+++++قفسه ها+++++

//ایجاد قفسه
bool DatabaseManager::createShelf(int userId, const QString& name) {
    // بررسی اینکه آیا این کاربر قفسه ای با این نام دارد یا خیر
    QSqlQuery check(db);
    check.prepare("SELECT 1 FROM shelves WHERE user_id = :u AND name = :n");
    check.bindValue(":u", userId);
    check.bindValue(":n", name);

    if (check.exec() && check.next()) {
        qDebug() << ".قفسه ای با این نام برای این کاربر از قبل وجود دارد";
        return false;
    }

    QSqlQuery q(db);
    q.prepare("INSERT INTO shelves (user_id, name) VALUES (:u, :n)");
    q.bindValue(":u", userId);
    q.bindValue(":n", name);
    return q.exec();
}

//تغییر نام قفسه
bool DatabaseManager::renameShelf(int shelfId, const QString& newName) {
    //پیدا کردن شناسه کاربرِ صاحب قفسه
    QSqlQuery getUser(db);
    getUser.prepare("SELECT user_id FROM shelves WHERE id = :id");
    getUser.bindValue(":id", shelfId);
    if (!getUser.exec() || !getUser.next()) return false;
    int userId = getUser.value(0).toInt();

    //بررسی تکراری نبودن نام جدید در قفسه های دیگرِ این کاربر
    QSqlQuery check(db);
    check.prepare("SELECT 1 FROM shelves WHERE user_id = :u AND name = :n AND id != :id");
    check.bindValue(":u", userId);
    check.bindValue(":n", newName);
    check.bindValue(":id", shelfId);

    if (check.exec() && check.next()) {
        qDebug() << ".نام جدید با یکی از قفسه های دیگر شما تداخل دارد";
        return false;
    }

    QSqlQuery q(db);
    q.prepare("UPDATE shelves SET name = :n WHERE id = :id");
    q.bindValue(":n", newName);
    q.bindValue(":id", shelfId);
    return q.exec();
}

//حذف قفسه
bool DatabaseManager::deleteShelf(int shelfId) {
    QSqlQuery q1(db);
    q1.prepare("DELETE FROM shelf_books WHERE shelf_id = :id");
    q1.bindValue(":id", shelfId);
    q1.exec();

    QSqlQuery q2(db);
    q2.prepare("DELETE FROM shelves WHERE id = :id");
    q2.bindValue(":id", shelfId);
    return q2.exec();
}

//افزودن کتاب به قفسه
bool DatabaseManager::addBookToShelf(int shelfId, int bookId) {
    //بررسی اینکه آیا این کتاب از قبل در این قفسه مشخص وجود دارد یا خیر
    QSqlQuery check(db);
    check.prepare("SELECT 1 FROM shelf_books WHERE shelf_id = :s AND book_id = :b");
    check.bindValue(":s", shelfId);
    check.bindValue(":b", bookId);

    if (check.exec() && check.next()) {
        qDebug() << ".این کتاب از قبل در این قفسه موجود است و دوباره اضافه نمیشود";
        return false; // خروجی فالس یعنی عملیات اضافه کردن انجام نشد (چون تکراری بود)
    }

    QSqlQuery q(db);
    q.prepare("INSERT INTO shelf_books (shelf_id, book_id) VALUES (:s, :b)");
    q.bindValue(":s", shelfId);
    q.bindValue(":b", bookId);
    return q.exec();
}

//انتقال کتاب بین قفسه ها
bool DatabaseManager::moveBookBetweenShelves(int fromShelfId, int toShelfId, int bookId) {
    //بررسی اینکه آیا کتاب از قبل در قفسه مقصد هست یا خیر؟
    QSqlQuery check(db);
    check.prepare("SELECT 1 FROM shelf_books WHERE shelf_id = :s AND book_id = :b");
    check.bindValue(":s", toShelfId);
    check.bindValue(":b", bookId);

    if (check.exec() && check.next()) {
        qDebug() << ".این کتاب از قبل در این قفسه موجود است و دوباره اضافه نمیشود";

        //کتاب از قبل در قفسه مقصد هست، پس فقط باید از قفسه مبدأ حذفش کنیم تا تداخل ایجاد نشود
        QSqlQuery q1(db);
        q1.prepare("DELETE FROM shelf_books WHERE shelf_id = :s AND book_id = :b");
        q1.bindValue(":s", fromShelfId);
        q1.bindValue(":b", bookId);
        q1.exec();
        return false;
    }

    QSqlQuery q1(db);
    q1.prepare("DELETE FROM shelf_books WHERE shelf_id = :s AND book_id = :b");
    q1.bindValue(":s", fromShelfId);
    q1.bindValue(":b", bookId);
    q1.exec();

    return addBookToShelf(toShelfId, bookId);
}

//گرفتن لیست قفسه ها
QList<QJsonObject> DatabaseManager::getShelves(int userId) {
    QList<QJsonObject> list;

    QSqlQuery q(db);
    q.prepare("SELECT id, name FROM shelves WHERE user_id = :u");
    q.bindValue(":u", userId);

    if (!q.exec())
        return list;

    while (q.next()) {
        QJsonObject shelf;
        shelf["id"] = q.value("id").toInt();
        shelf["name"] = q.value("name").toString();
        list.append(shelf);
    }

    return list;
}

//گرفتن لیستی از کتاب های یک قفسه
QList<QJsonObject> DatabaseManager::getBooksInShelf(int shelfId) {
    QList<QJsonObject> list;

    QSqlQuery q(db);
    q.prepare("SELECT b.id, b.title, b.author, b.genre, b.coverImagePath "
              "FROM shelf_books sb "
              "JOIN books b ON sb.book_id = b.id "
              "WHERE sb.shelf_id = :s");
    q.bindValue(":s", shelfId);

    if (!q.exec())
        return list;

    while (q.next()) {
        QJsonObject book;
        book["id"] = q.value("id").toInt();
        book["title"] = q.value("title").toString();
        book["author"] = q.value("author").toString();
        book["genre"] = q.value("genre").toString();
        book["coverImagePath"] = q.value("coverImagePath").toString();

        list.append(book);
    }

    return list;
}


//*********************************************پنل کاربر عادی ( ماژول 6 )****************************************************


//گرفتن آخرین صفحه خوانده شده
int DatabaseManager::getLastReadPage(int userId, int bookId) {
    QSqlQuery q(db);
    q.prepare("SELECT last_page FROM reading_progress WHERE user_id = :u AND book_id = :b");
    q.bindValue(":u", userId);
    q.bindValue(":b", bookId);

    if (!q.exec() || !q.next())
        return 1;

    return q.value(0).toInt();
}

//آپدیت آخرین صفحه خوانده شده
bool DatabaseManager::updateLastReadPage(int userId, int bookId, int page) {
    QSqlQuery check(db);
    check.prepare("SELECT 1 FROM reading_progress WHERE user_id = :u AND book_id = :b");
    check.bindValue(":u", userId);
    check.bindValue(":b", bookId);

    if (check.exec() && check.next()) {
        // ردیف از قبل وجود دارد، پس آپدیتش می‌کنیم
        QSqlQuery updateQuery(db);
        updateQuery.prepare("UPDATE reading_progress SET last_page = :p WHERE user_id = :u AND book_id = :b");
        updateQuery.bindValue(":p", page);
        updateQuery.bindValue(":u", userId);
        updateQuery.bindValue(":b", bookId);
        return updateQuery.exec();
    } else {
        // ردیفی وجود ندارد، پس ردیف جدید می‌سازیم
        QSqlQuery insertQuery(db);
        insertQuery.prepare("INSERT INTO reading_progress (user_id, book_id, last_page) VALUES (:u, :b, :p)");
        insertQuery.bindValue(":u", userId);
        insertQuery.bindValue(":b", bookId);
        insertQuery.bindValue(":p", page);
        return insertQuery.exec();
    }
}


//************************************************پنل ناشر ( ماژول 1 )*******************************************************

// گرفتن اطلاعات ناشر
QJsonObject DatabaseManager::getPublisherProfile(int publisherId) {
    QJsonObject profile;
    QSqlQuery q(db);

    // انتخاب دقیق فیلدهای موجود و بررسی متنی نقش
    q.prepare("SELECT id, username, name, email FROM users WHERE id = :id");
    q.bindValue(":id", publisherId);

    if (!q.exec() || !q.next())
        return profile; // در صورت عدم یافتن، جیسون خالی برمیگردد

    profile["id"] = q.value("id").toInt();
    profile["username"] = q.value("username").toString();
    profile["name"] = q.value("name").toString();
    profile["email"] = q.value("email").toString();

    return profile;
}

// آپدیت اطلاعات ناشر
bool DatabaseManager::updatePublisherProfile(int publisherId, const QJsonObject &data) {
    //خواندن اطلاعات فعلی ناشر از دیتابیس
    QSqlQuery currentQuery(db);
    currentQuery.prepare("SELECT username, name, email FROM users WHERE id = :id");
    currentQuery.bindValue(":id", publisherId);

    if (!currentQuery.exec() || !currentQuery.next()) {
        qDebug() << "Publisher not found or invalid role!";
        return false;
    }

    QString currentUsername = currentQuery.value("username").toString();
    QString currentName = currentQuery.value("name").toString();
    QString currentEmail = currentQuery.value("email").toString();

    QString newUsername = data.value("username").toString().trimmed();
    QString newName = data.value("name").toString().trimmed();
    QString newEmail = data.value("email").toString().trimmed();

    //اگر فیلدی خالی بود، همان مقدار فعلی دیتابیس حفظ میشود
    if (newUsername.isEmpty()) newUsername = currentUsername;
    if (newName.isEmpty())     newName = currentName;
    if (newEmail.isEmpty())    newEmail = currentEmail;

    //بررسی خطای نهایی خالی بودن
    if (newUsername.isEmpty()) {
        qDebug() << "Publisher username cannot be empty!";
        return false;
    }

    //بررسی تکراری نبودن نام کاربری
    if (newUsername != currentUsername) {
        QSqlQuery checkUsername(db);
        checkUsername.prepare("SELECT 1 FROM users WHERE username = :username AND id != :id LIMIT 1");
        checkUsername.bindValue(":username", newUsername);
        checkUsername.bindValue(":id", publisherId);
        if (checkUsername.exec() && checkUsername.next()) {
            qDebug() << "Publisher username is already taken!";
            return false;
        }
    }

    // بررسی تکراری نبودن نام نمایش
    if (!newName.isEmpty() && newName != currentName) {
        QSqlQuery checkName(db);
        checkName.prepare("SELECT 1 FROM users WHERE name = :name AND id != :id LIMIT 1");
        checkName.bindValue(":name", newName);
        checkName.bindValue(":id", publisherId);
        if (checkName.exec() && checkName.next()) {
            qDebug() << "Publisher name is already taken!";
            return false;
        }
    }

    //بررسی تکراری نبودن ایمیل
    if (!newEmail.isEmpty() && newEmail != currentEmail) {
        QSqlQuery checkEmail(db);
        checkEmail.prepare("SELECT 1 FROM users WHERE email = :email AND id != :id LIMIT 1");
        checkEmail.bindValue(":email", newEmail);
        checkEmail.bindValue(":id", publisherId);
        if (checkEmail.exec() && checkEmail.next()) {
            qDebug() << "Publisher email is already taken!";
            return false;
        }
    }

    //انجام عملیات بروزرسانی در دیتابیس
    QSqlQuery q(db);
    q.prepare("UPDATE users SET "
              "username = :username, "
              "name = :name, "
              "email = :email "
              "WHERE id = :id");

    q.bindValue(":username", newUsername);
    q.bindValue(":name", newName);
    q.bindValue(":email", newEmail);
    q.bindValue(":id", publisherId);

    return q.exec();
}


//************************************************پنل ناشر ( ماژول 2 )*******************************************************

// افزودن کتاب
bool DatabaseManager::addBook(const QJsonObject& bookData)
{
    QSqlQuery q(db);
    q.prepare("INSERT INTO books (title, author, genre, description, price, discountPercent, discountAmount, coverImagePath, pdfPath, publisher_id, isActive, is_deleted) "
              "VALUES (:title, :author, :genre, :desc, :price, :discP, :discA, :cover, :pdf, :pub_id, 1, 0)");

    double price = bookData.value("price").toDouble();
    double discP = bookData.value("discountPercent").toDouble();
    double discA = (price * discP) / 100.0; // محاسبه خودکار مبلغ تخفیف

    q.bindValue(":title", bookData.value("title").toString());
    q.bindValue(":author", bookData.value("author").toString());
    q.bindValue(":genre", bookData.value("genre").toString());
    q.bindValue(":desc", bookData.value("description").toString());
    q.bindValue(":price", price);
    q.bindValue(":discP", discP);
    q.bindValue(":discA", discA);

    q.bindValue(":cover", bookData.value("coverImagePath").toString());
    q.bindValue(":pdf", bookData.value("pdfPath").toString());
    q.bindValue(":pub_id", bookData.value("publisher_id").toInt());

    if (!q.exec()) {
        qDebug() << "Database Error (addBook failed):" << q.lastError().text();
        return false;
    }
    return true;
}

//دیدن اطلاعات یک کتاب
QJsonObject DatabaseManager::getBookDetails(int bookId) {
    QJsonObject book;
    QSqlQuery q(db);
    q.prepare("SELECT title, author, genre, description, price, discountPercent, pdfPath, coverImagePath FROM books WHERE id = :id");
    q.bindValue(":id", bookId);
    if (q.exec() && q.next()) {
        book["title"] = q.value("title").toString();
        book["author"] = q.value("author").toString();
        book["genre"] = q.value("genre").toString();
        book["description"] = q.value("description").toString();
        book["price"] = q.value("price").toDouble();
        book["discountPercent"] = q.value("discountPercent").toDouble();
        book["pdfPath"] = q.value("pdfPath").toString();
        book["coverImagePath"] = q.value("coverImagePath").toString();
    }
    return book;
}

// ویرایش یک کتاب
bool DatabaseManager::updateBook(int bookId, const QJsonObject& bookData) {
    QSqlQuery q(db);

    q.prepare("UPDATE books SET title = :title, author = :author, genre = :genre, "
              "description = :desc, price = :price, discountPercent = :discP, "
              "discountAmount = :discA, coverImagePath = :cover, pdfPath = :pdf "
              "WHERE id = :id AND is_deleted = 0");

    double price = bookData.value("price").toDouble();
    double discP = bookData.value("discountPercent").toDouble();
    double discA = (price * discP) / 100.0; // محاسبه خودکار مبلغ تخفیف جدید

    q.bindValue(":title", bookData.value("title").toString());
    q.bindValue(":author", bookData.value("author").toString());
    q.bindValue(":genre", bookData.value("genre").toString());
    q.bindValue(":desc", bookData.value("description").toString());
    q.bindValue(":price", price);
    q.bindValue(":discP", discP);
    q.bindValue(":discA", discA);
    q.bindValue(":cover", bookData.value("coverImagePath").toString());
    q.bindValue(":pdf", bookData.value("pdfPath").toString());
    q.bindValue(":id", bookId);

    if (!q.exec()) {
        qDebug() << "Database Error (updateBook failed):" << q.lastError().text();
        return false;
    }
    return true;
}

// اعمال تخفیف (با محاسبه خودکار بر اساس قیمت موجود در دیتابیس)
bool DatabaseManager::setBookDiscount(int bookId, int publisherId, double percent) {
    // ابتدا قیمت کتاب را بیرون می‌کشیم تا بر اساس آن تخفیف عددی را حساب کنیم
    QSqlQuery qPrice(db);
    qPrice.prepare("SELECT price FROM books WHERE id = :id AND publisher_id = :publisher AND is_deleted = 0");
    qPrice.bindValue(":id", bookId);
    qPrice.bindValue(":publisher", publisherId);

    if (!qPrice.exec() || !qPrice.next())
        return false;

    double price = qPrice.value("price").toDouble();
    double amount = (price * percent) / 100.0; // محاسبه خودکار مبلغ تخفیف

    QSqlQuery q(db);
    q.prepare("UPDATE books SET discountPercent = :p, discountAmount = :a "
              "WHERE id = :id AND publisher_id = :publisher AND is_deleted = 0");

    q.bindValue(":p", percent);
    q.bindValue(":a", amount);
    q.bindValue(":id", bookId);
    q.bindValue(":publisher", publisherId);

    return q.exec();
}

// فعال/غیر فعال کردن کتاب
bool DatabaseManager::setBookActiveState(int bookId, int publisherId, bool active) {
    QSqlQuery q(db);
    q.prepare("UPDATE books SET isActive = :active "
              "WHERE id = :id AND publisher_id = :publisher AND is_deleted = 0");

    q.bindValue(":active", active ? 1 : 0);
    q.bindValue(":id", bookId);
    q.bindValue(":publisher", publisherId);

    return q.exec();
}

//حذف کتاب
bool DatabaseManager::publisherDeleteBook(int bookId, int publisherId) {
    QSqlQuery q(db);
    // شرطِ اصلی: هم آیدی کتاب درست باشد، هم ناشرش همین کسی باشد که درخواست داده
    q.prepare("UPDATE books SET isActive = 0, is_deleted = 1 "
              "WHERE id = :id AND publisher_id = :pubId");

    q.bindValue(":id", bookId);
    q.bindValue(":pubId", publisherId);

    if (!q.exec()) {
        qDebug() << "Database Error (publisherDeleteBook failed):" << q.lastError().text();
        return false;
    }

    // بررسی اینکه آیا واقعاً تغییری اعمال شده (یعنی کتاب متعلق به این ناشر بوده)
    return q.numRowsAffected() > 0;
}

// گرفتن لیستی از کتاب های یک ناشر
QList<QJsonObject> DatabaseManager::getPublisherBooks(int publisherId) {
    QList<QJsonObject> list;
    QSqlQuery q(db);

    q.prepare("SELECT id, title, author, genre, description, price, "
              "discountPercent, discountAmount, coverImagePath, pdfPath, isActive "
              "FROM books WHERE publisher_id = :publisher AND is_deleted = 0");
    q.bindValue(":publisher", publisherId);

    if (!q.exec())
        return list;

    while (q.next()) {
        QJsonObject b;
        b["id"]              = q.value("id").toInt();
        b["title"]           = q.value("title").toString();
        b["author"]          = q.value("author").toString();
        b["genre"]           = q.value("genre").toString();
        b["description"]     = q.value("description").toString();
        b["price"]           = q.value("price").toDouble();
        b["discountPercent"] = q.value("discountPercent").toDouble();
        b["coverImagePath"]  = q.value("coverImagePath").toString();
        b["pdfPath"]         = q.value("pdfPath").toString();
        b["isActive"]        = q.value("isActive").toInt();
        list.append(b);
    }
    return list;
}


//************************************************پنل ناشر ( ماژول 3 )*******************************************************

//دیدن وضعیت ناشر
QJsonObject DatabaseManager::getPublisherStats(int publisherId) {
    QJsonObject stats;

    //تعداد کل کتاب های منتشرشده (فقط حذف نشده)
    {
        QSqlQuery q(db);
        q.prepare("SELECT COUNT(*) FROM books WHERE publisher_id = :p AND is_deleted = 0");
        q.bindValue(":p", publisherId);
        if (q.exec() && q.next())
            stats["totalBooks"] = q.value(0).toInt();
        else
            stats["totalBooks"] = 0;
    }

    //مجموع درآمد ناشر
    {
        QSqlQuery q(db);
        q.prepare("SELECT IFNULL(SUM(b.price - b.discountAmount), 0.0) "
                  "FROM library l "
                  "JOIN books b ON l.book_id = b.id "
                  "WHERE b.publisher_id = :p");
        q.bindValue(":p", publisherId);

        if (q.exec() && q.next())
            stats["totalRevenue"] = q.value(0).toDouble();
        else
            stats["totalRevenue"] = 0.0;
    }

    //میانگین امتیاز هر کتاب ناشر (فقط کتاب های حذف نشده)
    {
        QSqlQuery q(db);
        q.prepare("SELECT id, title, averageRating FROM books WHERE publisher_id = :p AND is_deleted = 0");
        q.bindValue(":p", publisherId);

        QJsonArray ratingsArr;
        if (q.exec()) {
            while (q.next()) {
                QJsonObject o;
                o["book_id"]   = q.value("id").toInt();
                o["title"]     = q.value("title").toString();
                o["avgRating"] = q.value("averageRating").toDouble();
                ratingsArr.append(o);
            }
        }
        stats["booksRatings"] = ratingsArr;
    }

    //پرفروش ترین کتاب ها (۵ کتاب اول از بین کتاب های حذف نشده)
    {
        QSqlQuery q(db);
        q.prepare("SELECT b.id, b.title, COUNT(l.id) AS salesCount "
                  "FROM books b "
                  "LEFT JOIN library l ON l.book_id = b.id "
                  "WHERE b.publisher_id = :p AND b.is_deleted = 0 "
                  "GROUP BY b.id, b.title "
                  "ORDER BY salesCount DESC "
                  "LIMIT 5");
        q.bindValue(":p", publisherId);

        QJsonArray bestArr;
        if (q.exec()) {
            while (q.next()) {
                QJsonObject o;
                o["book_id"]    = q.value("id").toInt();
                o["title"]      = q.value("title").toString();
                o["salesCount"] = q.value("salesCount").toInt();
                bestArr.append(o);
            }
        }
        stats["bestSellers"] = bestArr;
    }

    //کم فروش ترین کتاب ها (۵ کتاب آخر از بین کتاب های حذف نشده)
    {
        QSqlQuery q(db);
        q.prepare("SELECT b.id, b.title, COUNT(l.id) AS salesCount "
                  "FROM books b "
                  "LEFT JOIN library l ON l.book_id = b.id "
                  "WHERE b.publisher_id = :p AND b.is_deleted = 0 "
                  "GROUP BY b.id, b.title "
                  "ORDER BY salesCount ASC "
                  "LIMIT 5");
        q.bindValue(":p", publisherId);

        QJsonArray worstArr;
        if (q.exec()) {
            while (q.next()) {
                QJsonObject o;
                o["book_id"]    = q.value("id").toInt();
                o["title"]      = q.value("title").toString();
                o["salesCount"] = q.value("salesCount").toInt();
                worstArr.append(o);
            }
        }
        stats["worstSellers"] = worstArr;
    }

    return stats;
}


//*********************************************پنل مدیر سیستم ( ماژول 1 )****************************************************

//لیست همه کاربران
QList<QJsonObject> DatabaseManager::getAllUsers() {
    QList<QJsonObject> list;
    QSqlQuery q(db);
    q.prepare("SELECT id, username, role, is_blocked, registration_date FROM users WHERE is_deleted = 0");

    if (!q.exec()) return list;

    while (q.next()) {
        QJsonObject u;
        u["id"]               = q.value("id").toInt();
        u["username"]         = q.value("username").toString();
        u["role"]             = q.value("role").toString();
        u["is_blocked"]       = q.value("is_blocked").toInt();
        u["registration_date"]= q.value("registration_date").toString();
        list.append(u);
    }
    return list;
}

//دیدن اطلاعات کامل یک کاربر
QJsonObject DatabaseManager::getUserById(int userId) {
    QJsonObject u;
    QSqlQuery q(db);
    q.prepare("SELECT id, username, role, is_blocked, security_question, registration_date "
              "FROM users WHERE id = :id AND is_deleted = 0");
    q.bindValue(":id", userId);

    if (!q.exec() || !q.next()) return u;

    u["id"]               = q.value("id").toInt();
    u["username"]         = q.value("username").toString();
    u["role"]             = q.value("role").toString();
    u["is_blocked"]       = q.value("is_blocked").toInt();
    u["security_question"]= q.value("security_question").toString();
    u["registration_date"]= q.value("registration_date").toString();

    return u;
}

//جست و جو و فیلتر کاربران
QList<QJsonObject> DatabaseManager::searchUsers(const QString& keyword, const QString& roleFilter,
                                                int blockedFilter, const QString& registerDateFilter) {
    QList<QJsonObject> list;
    QString queryStr =
        "SELECT id, username, role, is_blocked, registration_date "
        "FROM users WHERE is_deleted = 0 ";

    //فیلتر کلمه کلیدی (اگر خالی نباشد)
    if (!keyword.isEmpty())
        queryStr += "AND username LIKE :kw ";

    //فیلتر نقش (اگر خالی نباشد)
    if (!roleFilter.isEmpty())
        queryStr += "AND role = :role ";

    //فیلتر وضعیت مسدود بودن (۰ یا ۱ فعال است، ۱- یعنی بی‌اثر)
    if (blockedFilter == 0 || blockedFilter == 1)
        queryStr += "AND is_blocked = :blk ";

    //فیلتر تاریخ ثبت نام (مثلاً کلاینت میفرستد: "2026-17-02")
    if (!registerDateFilter.isEmpty())
        queryStr += "AND registration_date LIKE :regDate ";

    QSqlQuery q(db);
    q.prepare(queryStr);

    // مقداردهی امن پارامترها
    if (!keyword.isEmpty())
        q.bindValue(":kw", "%" + keyword + "%");

    if (!roleFilter.isEmpty())
        q.bindValue(":role", roleFilter);

    if (blockedFilter == 0 || blockedFilter == 1)
        q.bindValue(":blk", blockedFilter);

    if (!registerDateFilter.isEmpty())
        q.bindValue(":regDate", registerDateFilter + "%"); // هر چیزی که با این تاریخ شروع شود

    if (!q.exec()) return list;

    while (q.next()) {
        QJsonObject u;
        u["id"]               = q.value("id").toInt();
        u["username"]         = q.value("username").toString();
        u["role"]             = q.value("role").toString();
        u["is_blocked"]       = q.value("is_blocked").toInt();
        u["registration_date"]= q.value("registration_date").toString();
        list.append(u);
    }
    return list;
}


//*********************************************پنل مدیر سیستم ( ماژول 2 )****************************************************

//حذف منطقی حساب کاربری
bool DatabaseManager::deleteUser(int userId) {
    QSqlQuery q(db);
    // هم پرچم حذف فعال می‌شود و هم برای امنیت دوبل کاربر مسدود میشود
    q.prepare("UPDATE users SET is_deleted = 1, is_blocked = 1 WHERE id = :id");
    q.bindValue(":id", userId);
    return q.exec();
}

//فعال/غیر فعال کردن کاربر
bool DatabaseManager::setUserActiveState(int userId, bool active) {
    QSqlQuery q(db);
    q.prepare("UPDATE users SET is_blocked = :b WHERE id = :id  AND is_deleted = 0");
    q.bindValue(":b", active ? 0 : 1);
    q.bindValue(":id", userId);
    return q.exec();
}


//*********************************************پنل مدیر سیستم ( ماژول 3 )****************************************************

//مشاهده تمامی کتاب های فعال
QList<QJsonObject> DatabaseManager::getAllBooks() {
    QList<QJsonObject> list;

    QSqlQuery q("SELECT id, title, author, genre, description, price, discountPercent, discountAmount, coverImagePath, publisher_id, isActive "
                "FROM books WHERE is_deleted = 0", db);

    if (!q.exec()) {
        qDebug() << "getAllBooks failed:" << q.lastError().text();
        return list;
    }

    while (q.next()) {
        QJsonObject book;
        book["id"]              = q.value("id").toInt();
        book["title"]           = q.value("title").toString();
        book["author"]          = q.value("author").toString();
        book["genre"]           = q.value("genre").toString();
        book["description"]     = q.value("description").toString();
        book["price"]           = q.value("price").toDouble();
        book["discountPercent"] = q.value("discountPercent").toDouble();
        book["discountAmount"]  = q.value("discountAmount").toDouble();
        book["coverImagePath"]  = q.value("coverImagePath").toString(); // مسیر نسبی عکس
        book["publisher_id"]    = q.value("publisher_id").toInt();
        book["isActive"]        = q.value("isActive").toInt();
        list.append(book);
    }
    return list;
}

// مشاهده اطلاعات کامل یک کتاب
QJsonObject DatabaseManager::getadminBookDetails(int bookId) {
    QJsonObject book;
    QSqlQuery q(db);

    q.prepare("SELECT id, title, author, genre, description, price, discountPercent, discountAmount, "
              "coverImagePath, pdfPath, publisher_id, isActive "
              "FROM books WHERE id = :id AND is_deleted = 0");
    q.bindValue(":id", bookId);

    if (!q.exec() || !q.next()) return book;

    book["id"]              = q.value("id").toInt();
    book["title"]           = q.value("title").toString();
    book["author"]          = q.value("author").toString();
    book["genre"]           = q.value("genre").toString();
    book["description"]     = q.value("description").toString();
    book["price"]           = q.value("price").toDouble();
    book["discountPercent"] = q.value("discountPercent").toDouble();
    book["discountAmount"]  = q.value("discountAmount").toDouble();

    // استخراج مسیرها از دیتابیس
    book["coverImagePath"]  = q.value("coverImagePath").toString();
    book["pdfPath"]         = q.value("pdfPath").toString();

    book["publisher_id"]    = q.value("publisher_id").toInt();
    book["isActive"]        = q.value("isActive").toInt();

    return book;
}

//ویرایش اطلاعات کتاب توسط مدیر
bool DatabaseManager::adminUpdateBook(int bookId, const QJsonObject& bookData) {
    QSqlQuery q(db);
    q.prepare("UPDATE books SET "
              "title = :title, author = :author, genre = :genre, description = :description, "
              "price = :price, discountPercent = :dp, discountAmount = :da, "
              "coverImagePath = :cover, pdfPath = :pdf "
              "WHERE id = :id AND is_deleted = 0");

    double price = bookData["price"].toDouble();
    double discP = bookData["discountPercent"].toDouble();
    double discA = (price * discP) / 100.0; // محاسبه خودکار مبلغ تخفیف

    q.bindValue(":title", bookData["title"].toString());
    q.bindValue(":author", bookData["author"].toString());
    q.bindValue(":genre", bookData["genre"].toString());
    q.bindValue(":description", bookData["description"].toString());
    q.bindValue(":price", price);
    q.bindValue(":dp", discP);
    q.bindValue(":da", discA);
    q.bindValue(":cover", bookData["coverImagePath"].toString());
    q.bindValue(":pdf", bookData["pdfPath"].toString());
    q.bindValue(":id", bookId);

    return q.exec();
}

//حذف کتاب نامعتبر
bool DatabaseManager::adminDeleteBook(int bookId) {
    QSqlQuery q(db);
    // همزمان کتاب را غیرفعال (0) و حذف منطقی (1) می‌کنیم
    q.prepare("UPDATE books SET isActive = 0, is_deleted = 1 WHERE id = :id");
    q.bindValue(":id", bookId);
    return q.exec();
}


//*********************************************سیستم اعلان ها****************************************************






