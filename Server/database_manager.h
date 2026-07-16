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
#include <QFile>
#include <QDir>
#include <QByteArray>


#include "constants.h"
#include "cryptohelper.h"
#include "user.h"

class DatabaseManager
{
public:
    //**************************************************احراز هویت مرکزی********************************************************
    DatabaseManager(const QString& connectionName = QStringLiteral("main_connection"));
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

    bool setFirstLoginFalseByUsername(const QString& username);
    bool setFavoriteGenres(const QString& username, const QStringList& genres);
    QStringList getFavoriteGenres(int userId);

    QList<QJsonObject> getRecommendedBooks(const QStringList& genres);
    QList<QJsonObject> getBooksByGenre(const QString& genre);

    QList<QJsonObject> getPopularBooks();
    QList<QJsonObject> getNewBooks();
    QList<QJsonObject> getBestsellers();
    QList<QJsonObject> getFreeBooks();

    QJsonObject getUserProfile(int userId);
    bool updateUserProfile(int userId, const QString& newUsername, const QString& name, const QString& email);

    bool changePassword(int userId,const QString& oldPasswordPlain,const QString& newPasswordPlain);

    QList<QJsonObject> getPurchaseHistory(int userId);
    int getTotalPurchases(int userId);

    bool isBookPurchased(int userId, int bookId);
    bool isBookInCart(int userId, int bookId);
    bool isBookSaved(int userId, int bookId);
    bool getActiveBookDetails(int bookId, QString &publisherName, double &rating, QString &coverPath);
    QString getBookPdfPath(int bookId);

    //*********************************************پنل کاربر عادی ( ماژول 2 )****************************************************


    QList<QJsonObject> searchBooks(const QString& title,const QString& author,const QString& publisherName);


    //*********************************************پنل کاربر عادی ( ماژول 3 )****************************************************


    bool addComment(int bookId, int userId, const QString& text, int rating);
    bool editComment(int commentId, const QString& newText, int newRating);
    bool deleteComment(int commentId);
    QList<QJsonObject> getCommentsForBook(int bookId);
    bool recalculateBookRating(int bookId);


    //*********************************************پنل کاربر عادی ( ماژول 4 )****************************************************


    bool addToCart(int userId, int bookId);
    bool removeFromCart(int userId, int bookId);
    QList<QJsonObject> getCartItems(int userId);
    bool clearCart(int userId);

    bool finalizePurchase(int userId, double clientFinalPrice);


    //*********************************************پنل کاربر عادی ( ماژول 5 )****************************************************


    //+++++کتاب های خریداری شده+++++
    QList<QJsonObject> getPurchasedBooks(int userId);

    //+++++کتاب های ذخیره شده+++++
    bool saveBook(int userId, int bookId);
    bool removeSavedBook(int userId, int bookId);
    QList<QJsonObject> getSavedBooks(int userId);

    //+++++قفسه ها+++++
    bool createShelf(int userId, const QString& name);
    bool renameShelf(int shelfId, const QString& newName);
    bool deleteShelf(int shelfId);

    bool addBookToShelf(int shelfId, int bookId);
    bool removeBookFromShelf(int shelfId, int bookId);
    bool moveBookBetweenShelves(int fromShelfId, int toShelfId, int bookId);

    QList<QJsonObject> getShelves(int userId);
    QList<QJsonObject> getBooksInShelf(int shelfId);


    //*********************************************پنل کاربر عادی ( ماژول 6 )****************************************************


    int getLastReadPage(int userId, int bookId);
    bool updateLastReadPage(int userId, int bookId, int page);


   //************************************************پنل ناشر ( ماژول 1 )*******************************************************


    QJsonObject getPublisherProfile(int publisherId);
    bool updatePublisherProfile(int publisherId, const QJsonObject& info);


    //************************************************پنل ناشر ( ماژول 2 )*******************************************************


    bool addBook(const QJsonObject& bookData);
    bool updateBook(int bookId, const QJsonObject& bookData);
    bool setBookDiscount(int bookId, int publisherId, double percent);
    bool setBookActiveState(int bookId, int publisherId, bool active);
    QList<QJsonObject> getPublisherBooks(int publisherId);


    //************************************************پنل ناشر ( ماژول 3 )*******************************************************


    QJsonObject getPublisherStats(int publisherId);








private:
    QSqlDatabase db;
    bool createTables();

};

#endif // DATABASE_MANAGER_H
