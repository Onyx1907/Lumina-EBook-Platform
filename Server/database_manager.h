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


struct InterestedUser {
    int id;
    QString username;
};

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
    bool getActiveBookDetails(int bookId, QString &publisherName, double &rating, QString &coverPath, QString &description);
    QString getBookPdfPath(int bookId);

    //*********************************************پنل کاربر عادی ( ماژول 2 )****************************************************


    QList<QJsonObject> searchBooks(const QString& title,const QString& author,const QString& publisherName);


    //*********************************************پنل کاربر عادی ( ماژول 3 )****************************************************


    int getBookIdByCommentId(int commentId);
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
    bool updatePublisherProfile(int publisherId, const QJsonObject &data);


    //************************************************پنل ناشر ( ماژول 2 )*******************************************************


    bool addBook(const QJsonObject& bookData);
    QJsonObject getBookDetails(int bookId);
    bool updateBook(int bookId, const QJsonObject& bookData);
    bool setBookDiscount(int bookId, int publisherId, double percent);
    QJsonObject getBookFinancialDetails(int bookId);
    bool setBookActiveState(int bookId, int publisherId, bool active);
    bool publisherDeleteBook(int bookId, int publisherId);
    QList<QJsonObject> getPublisherBooks(int publisherId);


    //************************************************پنل ناشر ( ماژول 3 )*******************************************************


    QJsonObject getPublisherStats(int publisherId);


    //*********************************************پنل مدیر سیستم ( ماژول 1 )****************************************************


    QList<QJsonObject> getAllUsers();
    QJsonObject getUserById(int userId);
    QList<QJsonObject> searchUsers(const QString& keyword, const QString& roleFilter,
                                   int blockedFilter, const QString& registerDateFilter);


    //*********************************************پنل مدیر سیستم ( ماژول 2 )****************************************************


    bool deleteUser(int userId);
    bool setUserActiveState(int userId, bool active);


    //*********************************************پنل مدیر سیستم ( ماژول 3 )****************************************************


    QList<QJsonObject> getAllBooks();
    bool adminUpdateBook(int bookId, const QJsonObject& bookData);
    bool adminDeleteBook(int bookId);


    //****************************************************سیستم اعلان ها**********************************************************


    QList<QJsonObject> getNotifications(int userId, const QString& role);
    bool markNotificationRead(int notificationId);
    int createNotification(int userId, const QString& role, const QString& type, const QString& message);
    int getUnreadNotificationsCount(int userId);

    //***************************************************توابع کمکی برای اعلان*****************************************************


    QString getUsernameById(int userId);
    int getPublisherIdForBook(int bookId);
    QString getBookTitle(int bookId);
    QList<QPair<int, QString>> getPublisherAndBooksForCart(int userId);

    int getLastInsertedBookId();
    QList<InterestedUser> getUsersInterestedInGenre(const QString& genre);

    QList<InterestedUser> getUsersWhoSavedBook(int bookId);




private:
    QSqlDatabase db;
    bool createTables();

};

#endif // DATABASE_MANAGER_H
