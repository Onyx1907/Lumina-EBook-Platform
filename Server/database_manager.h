#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QList>


#include "constants.h"
#include "cryptohelper.h"
#include "user.h"

class DatabaseManager
{
public:
//**************************************************احراز هویت مرکزی********************************************************
    DatabaseManager();
    bool initDatabase();

//Registration(ثبت نام)
    bool isUsernameTaken(const QString& username);
    bool registerUser(const QString& username,const QString& plainPassword,UserRole role,
                      const QString& securityQuestion,const QString& securityAnswerPlain);

//login(ورود به سیستم)
    bool verifyUser(const QString& username,const QString& plainPassword,
                    UserRole& outRole,bool& outIsBlocked,int& outUserId,int& outFirstLogin);

    bool setFirstLoginFalse(int userId);


//forget password(فراموشی رمز عبور)
    bool getSecurityQuestion(const QString& username,QString& outQuestion);
    bool verifySecurityAnswerAndResetPassword(const QString& username,const QString& answerPlain,const QString& newPlainPassword);

//*********************************************پنل کاربر عادی ( ماژول 1 )****************************************************
    bool setFavoriteGenres(const QString& username, const QStringList& genres);
    QStringList getFavoriteGenres(const QString& username);

    QList<QJsonObject> getRecommendedBooks(const QStringList& genres);
    QList<QJsonObject> getBooksByGenre(const QString& genre);

    QList<QJsonObject> getPopularBooks();
    QList<QJsonObject> getNewBooks();
    QList<QJsonObject> getBestsellers();
    QList<QJsonObject> getFreeBooks();

    QJsonObject getUserProfile(const QString& username);
    bool updateUserProfile(const QString& username, const QString& name, const QString& email);

    bool changePassword(const QString& username,const QString& oldPasswordPlain,const QString& newPasswordPlain);

    QList<QJsonObject> getPurchaseHistory(const QString& username);
    int getTotalPurchases(const QString& username);


//*********************************************پنل کاربر عادی ( ماژول 2 )****************************************************


    QList<QJsonObject> searchBooks(const QString& title,const QString& author,const QString& publisherName);










private:
//**************************************************احراز هویت مرکزی*********************************************************
    QSqlDatabase db;
    bool createTables();

};

#endif // DATABASE_MANAGER_H
