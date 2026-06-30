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
        "password_hash TEXT NOT NULL,"                        // هش رمز عبور (عدم ذخیره متن خام)
        "role TEXT NOT NULL,"                                // نقش کاربر در سیستم
        "is_blocked INTEGER NOT NULL DEFAULT 0,"            // وضعیت مسدود بودن (پیش فرض: فعال)
        "security_question TEXT,"                          // سوال امنیتی برای بازیابی رمز
        "security_answer_encrypted BLOB"                  // پاسخ امنیتی رمزنگاری شده به صورت باینری
        "registration_date TEXT NOT NULL"                // تاریخ ثبت نام
        ");";
    if(!q.exec(createUsers)){
        qDebug() << "Create users failed: " << q.lastError().text();
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
        "FOREIGN KEY (publisher_id) REFERENCES users(id)"                     //تعریف کلید خارجی برای اتصال شناسه ناشر به شناسه کاربر در جدول کاربران
        ")";
    if (!q.exec(createBooks)) {
        qDebug() << "Create books failed:" << q.lastError().text();
        return false;
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

    QSqlQuery q;
    q.prepare("INSERT INTO users "
              "(username, password_hash, role, is_blocked, security_question, security_answer_encrypted,  registration_date) "
              "VALUES (:u, :ph, :r, 0, :sq, :sa, :rd)");

    q.bindValue(":u", username);
    q.bindValue(":ph", passwordHash);
    q.bindValue(":r", QString::number(static_cast<int>(role)));
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
bool DatabaseManager::verifyUser(const QString& username,const QString& plainPassword,UserRole& outRole,bool& outIsBlocked,int& outUserId){
    QSqlQuery q;
    q.prepare("SELECT id, password_hash, role, is_blocked FROM users WHERE username = :u");
    q.bindValue(":u", username);
    if (!q.exec()) return false;
    if (!q.next()) return false;
    outUserId = q.value("id").toInt();
    QString storedHash = q.value("password_hash").toString();
    int roleInt = q.value("role").toInt();
    outRole = static_cast<UserRole>(roleInt);
    outIsBlocked = q.value("is_blocked").toInt() != 0;

    if(outIsBlocked) return false;

    QString inputHash = CryptoHelper::hashPassword(plainPassword);
    return (inputHash == storedHash);
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
bool DatabaseManager::verifySecurityAnswerAndResetPassword(const QString& username,const QString& answerPlain,const QString& newPlainPassword){
    QSqlQuery q;
    q.prepare("SELECT security_answer_encrypted FROM users WHERE username = :u");
    q.bindValue(":u", username);
    if (!q.exec()) return false;
    if (!q.next()) return false;
    QByteArray encrypted = q.value(0).toByteArray();
    QString storedAnswerPlain = CryptoHelper::decryptData(encrypted, NETWORK_SECRET_KEY);
    if (storedAnswerPlain != answerPlain) return false;

    QString newHash = CryptoHelper::hashPassword(newPlainPassword);

    QSqlQuery q2;
    q2.prepare("UPDATE users SET password_hash = :ph WHERE username = :u");
    q2.bindValue(":ph", newHash);
    q2.bindValue(":u", username);
    return q2.exec();
}

//*********************************************پنل کاربر عادی ( ماژول 1 )****************************************************


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
    obj["publisher_id"] = q.value("publisherId").toInt();
    obj["genre"] = q.value("genre").toString();
    obj["price"] = q.value("price").toDouble();
    obj["discount_percentage"] = q.value("discountPercentage").toDouble();
    obj["pdf_path"] = q.value("pdfPath").toString();
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

    QString sql = QString("SELECT * FROM books WHERE genre IN (%1)").arg(placeholders.join(","));
    QSqlQuery q;
    q.prepare(sql);
    for (int i = 0; i < genres.size(); ++i)
        q.bindValue(QString(":g%1").arg(i), genres[i]);
    if(!q.exec())
        return list;
    while(q.next())
        list.append(bookFromQuery(q));
    return list;
}
// فیلتراسیون و دریافت کتاب ها بر اساس یک ژانر مشخص شده
QList<QJsonObject> DatabaseManager::getBooksByGenre(const QString& genre){
    QList<QJsonObject> list;
    QSqlQuery q;
    q.prepare("SELECT * FROM books WHERE genre = :g");
    q.bindValue(":g", genre);
    if(!q.exec())
        return list;
    while(q.next())
        list.append(bookFromQuery(q));
    return list;
}
// بازیابی لیست تمام کتاب های نشانه گذاری شده به عنوان محبوب
QList<QJsonObject> DatabaseManager::getPopularBooks(){
    QList<QJsonObject> list;
    QSqlQuery q;
    q.prepare("SELECT * FROM books WHERE is_popular = 1");
    if(!q.exec())
        return list;
    while(q.next())
        list.append(bookFromQuery(q));
    return list;
}
// بازیابی لیست تمام کتاب های تازه اضافه شده به سیستم
QList<QJsonObject> DatabaseManager::getNewBooks(){
    QList<QJsonObject> list;
    QSqlQuery q;
    q.prepare("SELECT * FROM books WHERE is_new = 1");
    if(!q.exec())
        return list;
    while(q.next())
        list.append(bookFromQuery(q));
    return list;
}
// بازیابی لیست پرفروش ترین کتاب های موجود در پایگاه داده
QList<QJsonObject> DatabaseManager::getBestsellers(){
    QList<QJsonObject> list;
    QSqlQuery q;
    q.prepare("SELECT * FROM books WHERE is_bestseller = 1");
    if(!q.exec())
        return list;
    while(q.next())
        list.append(bookFromQuery(q));
    return list;
}
// دریافت لیست کتاب هایی که به صورت رایگان در اختیار کاربران قرار دارند
QList<QJsonObject> DatabaseManager::getFreeBooks(){
    QList<QJsonObject> list;
    QSqlQuery q;
    q.prepare("SELECT * FROM books WHERE is_free = 1");
    if(!q.exec())
        return list;
    while(q.next())
        list.append(bookFromQuery(q));
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
    q.prepare("SELECT passwordHash FROM users WHERE username = :u");
    q.bindValue(":u", username);
    if (!q.exec() || !q.next())
        return false;

    const QString currentHash = q.value(0).toString();
    if (currentHash != CryptoHelper::hashPassword(oldPasswordPlain))
        return false;

    const QString newHash = CryptoHelper::hashPassword(newPasswordPlain);
    QSqlQuery q2;
    q2.prepare("UPDATE users SET passwordHash = :p WHERE username = :u");
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
QList<QJsonObject> DatabaseManager::searchBooks(const QString& title,const QString& author,const QString& publisherName) {
    QList<QJsonObject> list;

// تعریف کوئری پایه برای دریافت اطلاعات کتاب ها به همراه نام ناشر
       QString sql = "SELECT b.*, p.companyName AS publisherName "      // انتخاب تمام ستون های کتاب و تغییر نام ستون شرکت ناشر
                  "FROM books b "                                       // b انتخاب جدول اصلی کتاب ها با نام مستعار
                  "LEFT JOIN publishers p ON b.publisherId = p.id "     //اتصال به جدول ناشران بر اساس شناسه ناشر (حتی اگر کتاب ناشر نداشته باشد)
                  "WHERE 1=1 ";                                         // ANDیک شرط همیشه درست برای تسهیل در چسباندن دینامیک شرط های بعدی با



// افزودن دینامیک(پویا)شرط عنوان کتاب به کوئری در صورت وجود ورودی
    if (!title.isEmpty())                                         //اگر کاربر در بخش عنوان کلمه "برنامه" را سرچ کند
        sql += "AND b.title LIKE :title ";                       //مقدار نهایی تبدیل به "%برنامه%" میشود
                                                                //اگر علامت % اول را حذف کنید (یعنی title + "%")
                                                               // فقط کتاب هایی پیدا میشوند که با کلمه "برنامه" شروع میشوند
                                                              //اگر علامت % آخر را حذف کنید (یعنی "%" + title)
                                                             //فقط کتاب هایی پیدا میشوند که به کلمه "برنامه" ختم میشوند


// افزودن دینامیک(پویا)شرط نویسنده به کوئری در صورت وجود ورودی
    if (!author.isEmpty())
        sql += "AND b.author LIKE :author ";

// افزودن دینامیک(پویا)شرط نام ناشر به کوئری در صورت وجود ورودی
    if (!publisherName.isEmpty())
        sql += "AND p.companyName LIKE :publisherName ";

    QSqlQuery q;
    q.prepare(sql);

    if (!title.isEmpty())
        q.bindValue(":title", "%" + title + "%");
    if (!author.isEmpty())
        q.bindValue(":author", "%" + author + "%");
    if (!publisherName.isEmpty())
        q.bindValue(":publisherName", "%" + publisherName + "%");

    if (!q.exec())
        return list;

    while (q.next()) {
        QJsonObject obj = bookFromQuery(q);
        obj["publisher_name"] = q.value("publisherName").toString();
        list.append(obj);
    }

    return list;
}


