#include "database_manager.h"

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
    QString createUsers =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"                // شناسه یکتا و خودکار
        "username TEXT UNIQUE NOT NULL,"                       // نام کاربری (یکتا و اجباری)
        "password_hash TEXT NOT NULL,"                         // هش رمز عبور (عدم ذخیره متن خام)
        "role TEXT NOT NULL,"                                  // نقش کاربر در سیستم
        "is_blocked INTEGER NOT NULL DEFAULT 0,"               // وضعیت مسدود بودن (پیش فرض: فعال)
        "security_question TEXT,"                              // سوال امنیتی برای بازیابی رمز
        "security_answer_encrypted BLOB"                       // پاسخ امنیتی رمزنگاری شده به صورت باینری
        ");";
    if(!q.exec(createUsers)){
        qDebug() << "Create users failed: " << q.lastError().text();
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

    QSqlQuery q;
    q.prepare("INSERT INTO users "
              "(username, password_hash, role, is_blocked, security_question, security_answer_encrypted) "
              "VALUES (:u, :ph, :r, 0, :sq, :sa)");

    q.bindValue(":u", username);
    q.bindValue(":ph", passwordHash);
    q.bindValue(":r", QString::number(static_cast<int>(role)));
    q.bindValue(":sq", securityQuestion);
    q.bindValue(":sa", encryptedAnswer);

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



