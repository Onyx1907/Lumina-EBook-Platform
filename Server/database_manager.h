#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

#include "constants.h"
#include "cryptohelper.h"
#include "user.h"

class DatabaseManager
{
public:
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

private:
    QSqlDatabase db;
    bool createTables();

};

#endif // DATABASE_MANAGER_H
