#include "database_manager.h"

//*********************************************احراز هویت مرکزی***************************************************


// سازنده کلاس: تنظیمات اولیه پایگاه داده را انجام میدهد
DatabaseManager::DatabaseManager() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("bookclub.db");
}
// راه اندازی پایگاه داده: اتصال را برقرار کرده و جدول ها را می سازد
bool DatabaseManager::initDatabase(){
    if(!db.open()){
        qDebug() << "db open failed: " << db.lastError().text();
        return false;
    }
    return createTables();
}
// ایجاد جدول های مورد نیاز در پایگاه داده (در صورت عدم وجود)
bool DatabaseManager::createTables(){
    QSqlQuery q;

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
        "first_login INTEGER DEFAULT 1,"                       // وضعیت اولین ورود کاربر (پیش‌فرض ۱ برای اولین ورود)
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
        // اگر ستون از قبل وجود داشته باشد دیتابیس خطا میدهد که طبیعی است، پس برنامه را متوقف نمیکنیم
        qDebug() << "Note: is_deleted column might already exist.";
    }

    //--------------جدول اعلان ها--------------
    QString createNotifications =
        "CREATE TABLE IF NOT EXISTS notifications ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"                                       //شناسه یکتا، عددی و خودکارافزایش (کلید اصلی)
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




    return true;
}
// بررسی تکراری نبودن نام کاربری هنگام ثبت نام
bool DatabaseManager::isUsernameTaken(const QString& username){
    QSqlQuery q;
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

    QSqlQuery q;
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
    QSqlQuery q;
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
    QSqlQuery q;
    q.prepare("UPDATE users SET first_login = 0 WHERE id = :id");
    q.bindValue(":id", userId);
    return q.exec();
}

// دریافت سوال امنیتی کاربر برای فرآیند بازیابی رمز عبور
bool DatabaseManager::getSecurityQuestion(const QString& username,QString& outQuestion){
    QSqlQuery q;
    q.prepare("SELECT security_question FROM users WHERE username = :u");
    q.bindValue(":u", username);
    if (!q.exec()) return false;
    if (!q.next()) return false;
    outQuestion = q.value(0).toString();
    return true;
}
// تایید پاسخ امنیتی و تغییر رمز عبور در صورت صحت اطلاعات
bool DatabaseManager::verifySecurityAnswerAndResetPassword(const QString& username, const QString& answerPlain, const QString& newPlainPassword){
    QSqlQuery q;
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

    QSqlQuery q2;
    q2.prepare("UPDATE users SET password_hash = :ph WHERE username = :u");
    q2.bindValue(":ph", newHash);
    q2.bindValue(":u", username);
    return q2.exec();

}



//*********************************************پنل کاربر عادی ( ماژول 1 )****************************************************


bool DatabaseManager::setFirstLoginFalseByUsername(const QString& username)
{
    QSqlQuery q;
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

    QSqlQuery q;
    q.prepare("UPDATE users SET favorite_genres = :g WHERE username = :u");
    q.bindValue(":g", json);
    q.bindValue(":u", username);
    return q.exec();
}
// بازیابی و استخراج ژانرهای مورد علاقه کاربر به صورت لیست متنی در سی پلاس پلاس
QStringList DatabaseManager::getFavoriteGenres(const QString& username){
    QStringList result;
    QSqlQuery q;
    q.prepare("SELECT favorite_genres FROM users WHERE username = :u");
    q.bindValue(":u", username);
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
    obj["publisher_id"] = q.value("publisher_id").toInt();
    obj["genre"] = q.value("genre").toString();
    obj["price"] = q.value("price").toDouble();
    obj["discount_percentage"] = q.value("discountPercent").toDouble();
    obj["discount_amount"] = q.value("discountAmount").toDouble();
    obj["cover_image_path"] = q.value("coverImagePath").toString();
    obj["pdf_path"] = q.value("pdfPath").toString();
    obj["averageRating"] = q.value("averageRating").toDouble();
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
    QSqlQuery q;
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
QList<QJsonObject> DatabaseManager::getBooksByGenre(const QString& genre){
    QList<QJsonObject> list;
    QSqlQuery q;
    q.prepare("SELECT * FROM books WHERE genre = :g AND isActive = 1 LIMIT 20");
    q.bindValue(":g", genre);
    if(!q.exec())
        return list;
    while(q.next())
        list.append(bookFromQueryWithoutPdf(q));
    return list;
}
// بازیابی لیست تمام کتاب های نشانه گذاری شده به عنوان محبوب
QList<QJsonObject> DatabaseManager::getPopularBooks(){
    QList<QJsonObject> list;
    QSqlQuery q;
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
    QSqlQuery q;
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
    QSqlQuery q;
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
    QSqlQuery q;
    q.prepare("SELECT * FROM books WHERE price = 0 AND isActive = 1 LIMIT 20");
    if(!q.exec())
        return list;
    while(q.next())
        list.append(bookFromQueryWithoutPdf(q));
    return list;
}
//JSON دریافت و تجمیع اطلاعات پروفایل شخصی کاربر در قالب یک شیء خلاصه شده
QJsonObject DatabaseManager::getUserProfile(const QString& username){
    QJsonObject obj;
    QSqlQuery q;
    q.prepare("SELECT name, email, favorite_genres FROM users WHERE username = :u");
    q.bindValue(":u", username);
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

    obj["total_purchases"] = getTotalPurchases(username);
    return obj;

}
// به روزرسانی اطلاعات هویتی پایه (نام و ایمیل) کاربر بر اساس نام کاربری
bool DatabaseManager::updateUserProfile(const QString& username, const QString& name, const QString& email){
    QSqlQuery q;
    q.prepare("UPDATE users SET name = :n, email = :e WHERE username = :u");
    q.bindValue(":n", name);
    q.bindValue(":e", email);
    q.bindValue(":u", username);
    return q.exec();
}
// فرآیند احراز هویت رمز عبور فعلی و ثبت رمز عبور جدید به صورت هش شده
bool DatabaseManager::changePassword(const QString& username,const QString& oldPasswordPlain,const QString& newPasswordPlain){
    QSqlQuery q;
    q.prepare("SELECT password_hash FROM users WHERE username = :u");
    q.bindValue(":u", username);
    if (!q.exec() || !q.next())
        return false;

    const QString currentHash = q.value(0).toString();
    if (currentHash != CryptoHelper::hashPassword(oldPasswordPlain))
        return false;

    const QString newHash = CryptoHelper::hashPassword(newPasswordPlain);
    QSqlQuery q2;
    q2.prepare("UPDATE users SET password_hash = :p WHERE username = :u");
    q2.bindValue(":p", newHash);
    q2.bindValue(":u", username);
    return q2.exec();
}
// استخراج تاریخچه کامل کتاب های خریداری شده توسط کاربر به همراه جزئیات فاکتور
QList<QJsonObject> DatabaseManager::getPurchaseHistory(const QString& username){
    QList<QJsonObject> list;
    QSqlQuery q;
    q.prepare("SELECT p.book_id, p.purchase_date, b.title, b.author, b.price "
              "FROM purchases p "
              "JOIN users u ON p.user_id = u.id "
              "JOIN books b ON p.book_id = b.id "
              "WHERE u.username = :u "
              "ORDER BY p.purchase_date DESC");
    q.bindValue(":u", username);
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
int DatabaseManager::getTotalPurchases(const QString& username){
    QSqlQuery q;
    q.prepare("SELECT COUNT(*) "
              "FROM purchases p "
              "JOIN users u ON p.user_id = u.id "
              "WHERE u.username = :u");
    q.bindValue(":u", username);
    if (!q.exec() || !q.next())
        return 0;
    return q.value(0).toInt();
}

//*********************************************پنل کاربر عادی ( ماژول 2 )****************************************************


//تابع جستوجوی کتاب
QList<QJsonObject> DatabaseManager::searchBooks(const QString& title, const QString& author, const QString& publisherName) {
    QList<QJsonObject> list;
    QSqlQuery q;

    QString queryStr = "SELECT b.id, b.title, b.author, b.genre, b.description, b.price, "
                       "b.discountPercent, b.discountAmount, b.coverImagePath, b.pdfPath, b.publisher_id, b.isActive "
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
        // استفاده از همان تابع مپینگ استاندارد شده
        list.append(bookFromQueryWithoutPdf(q));
    }
    return list;
}


//*********************************************پنل کاربر عادی ( ماژول 3 )****************************************************


//اضافه کردن نظر
bool DatabaseManager::addComment(int bookId, int userId,
                                 const QString& text, int rating) {
    const QString now = QDateTime::currentDateTime().toString(Qt::ISODate);

    QSqlQuery q;
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
    const QString now = QDateTime::currentDateTime().toString(Qt::ISODate);  //این خط کد، زمان و تاریخ فعلی سیستم را میگیرد
    //و آن را به یک متن (رشته) استاندارد و قابل فهم برای کامپیوتر تبدیل میکند

    QSqlQuery qGet;
    qGet.prepare("SELECT book_id FROM comments WHERE id = :id");
    qGet.bindValue(":id", commentId);
    if (!qGet.exec() || !qGet.next())
        return false;

    int bookId = qGet.value(0).toInt();

    QSqlQuery q;
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
    QSqlQuery qGet;
    qGet.prepare("SELECT book_id FROM comments WHERE id = :id");
    qGet.bindValue(":id", commentId);
    if (!qGet.exec() || !qGet.next())
        return false;

    int bookId = qGet.value(0).toInt();

    QSqlQuery q;
    q.prepare("DELETE FROM comments WHERE id = :id");
    q.bindValue(":id", commentId);

    if (!q.exec())
        return false;

    return recalculateBookRating(bookId);
}

//دیدن نظرات یک کتاب
QList<QJsonObject> DatabaseManager::getCommentsForBook(int bookId) {
    QList<QJsonObject> list;

    QSqlQuery q;
    q.prepare("SELECT c.id, c.comment_text, c.rating, c.created_at, c.updated_at, "
              "u.username "
              "FROM comments c "
              "JOIN users u ON c.user_id = u.id "
              "WHERE c.book_id = :b "
              "ORDER BY c.created_at DESC");
    q.bindValue(":b", bookId);

    if (!q.exec())
        return list;

    while (q.next()) {
        QJsonObject obj;
        obj["id"] = q.value("id").toInt();
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
    QSqlQuery q;
    q.prepare("SELECT rating FROM comments WHERE book_id = :b");
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

    QSqlQuery qUpdate;
    qUpdate.prepare("UPDATE books SET averageRating = :a, ratingCount = :c WHERE id = :b");
    qUpdate.bindValue(":a", avg);
    qUpdate.bindValue(":c", count);
    qUpdate.bindValue(":b", bookId);

    return qUpdate.exec();
}

//*********************************************پنل کاربر عادی ( ماژول 4 )****************************************************


//افزودن به سبد خرید
bool DatabaseManager::addToCart(int userId, int bookId) {
    QSqlQuery q;
    q.prepare("INSERT INTO cart (user_id, book_id) "
              "SELECT :u, id FROM books "
              "WHERE id = :b AND isActive = 1");
    q.bindValue(":u", userId);
    q.bindValue(":b", bookId);
    return q.exec();
}

//حذف از سبد خرید
bool DatabaseManager::removeFromCart(int userId, int bookId) {
    QSqlQuery q;
    q.prepare("DELETE FROM cart WHERE user_id = :u AND book_id = :b");
    q.bindValue(":u", userId);
    q.bindValue(":b", bookId);
    return q.exec();
}

//گرفتن یک لیست از سبد خرید
QList<QJsonObject> DatabaseManager::getCartItems(int userId) {
    QList<QJsonObject> list;

    QSqlQuery q;
    q.prepare("SELECT b.id, b.title, b.author, b.price, b.discountPercent, b.isActive "
              "FROM cart c "
              "JOIN books b ON c.book_id = b.id "
              "WHERE c.user_id = :u");
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

        //فرستادن وضعیت دسترسی به کلاینت (۱ یعنی موجود، ۰ یعنی مسدود/حذف شده)
        obj["isActive"] = (q.value("isActive").toInt() == 1);

        list.append(obj);
    }

    return list;
}

//پاک کردن سبد خرید
bool DatabaseManager::clearCart(int userId) {
    QSqlQuery q;
    q.prepare("DELETE FROM cart WHERE user_id = :u");
    q.bindValue(":u", userId);
    return q.exec();
}

//نهایی کردن خرید
bool DatabaseManager::finalizePurchase(int userId) {
    //به دیتابیس میگوییم: یک تراکنش امن باز کن (تغییرات را موقتی نگه دار)
    QSqlDatabase::database().transaction();

    //بررسی وجود کتاب مسدود/غیرفعال در سبد کاربر
    QSqlQuery qCheck;
    qCheck.prepare("SELECT COUNT(*) FROM cart c "
                   "JOIN books b ON c.book_id = b.id "
                   "WHERE c.user_id = :u AND b.isActive = 0");
    qCheck.bindValue(":u", userId);

    if (!qCheck.exec() || !qCheck.next()) {
        QSqlDatabase::database().rollback(); // خطای ناگهانی دیتابیس -> همه‌چیز لغو
        return false;
    }

    //اگر شمارش کتاب های مسدود بیشتر از 0 بود
    if (qCheck.value(0).toInt() > 0) {
        qDebug() << "Purchase failed: Inactive books detected.";
        QSqlDatabase::database().rollback(); //همه چیز را به حالت قبل برگردان و لغو کن
        return false;
    }

    //خواندن کتاب های داخل سبد خرید
    QSqlQuery q;
    q.prepare("SELECT book_id FROM cart WHERE user_id = :u");
    q.bindValue(":u", userId);

    if (!q.exec()) {
        QSqlDatabase::database().rollback(); // خطا در خواندن -> لغو
        return false;
    }

    const QString now = QDateTime::currentDateTime().toString(Qt::ISODate);

    // شروع حلقه روی کتاب های سبد خرید
    while (q.next()) {
        int bookId = q.value(0).toInt();

        //بررسی اینکه کاربر قبلاً این کتاب را نخریده باشد
        QSqlQuery check;
        check.prepare("SELECT 1 FROM library WHERE user_id = :u AND book_id = :b");
        check.bindValue(":u", userId);
        check.bindValue(":b", bookId);

        if (check.exec() && check.next()) {
            // کاربر قبلاً این کتاب را خریده، پس بدون خطا رفتن به کتاب بعدی سبد خرید
            continue;
        }

        //درج کتاب در کتابخانه کاربر
        QSqlQuery insert;
        insert.prepare("INSERT INTO library (user_id, book_id, purchase_date) "
                       "VALUES (:u, :b, :d)");
        insert.bindValue(":u", userId);
        insert.bindValue(":b", bookId);
        insert.bindValue(":d", now);

        if (!insert.exec()) {
            QSqlDatabase::database().rollback(); // اگر درج این کتاب ارور داد -> کل خرید لغو
            return false;
        }
    }

    //پاک کردن سبد خرید بعد از اتمام حلقه
    if (!clearCart(userId)) {
        QSqlDatabase::database().rollback(); // اگر سبد پاک نشد -> کل خرید لغو
        return false;
    }

    //همه چیز عالی بود! حالا تغییرات را در دیتابیس قطعی و ماندگار کن
    return QSqlDatabase::database().commit();
}


