#ifndef NETWORKWORKER_H
#define NETWORKWORKER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include  <QThread>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QDateTime>
#include <QRegularExpression>
#include <QByteArray>
#include <QMetaObject>
#include<QCoreApplication>

#include <utility>


#include "database_manager.h"
#include "user.h"
#include "constants.h"



// کلاس کارگر برای پردازش همزمان دستورات شبکه، نظرات و اعلان ها در ترد جداگانه
class NetworkWorker : public QObject {
    Q_OBJECT
public:
    NetworkWorker(qintptr socketDescriptor, DatabaseManager* db);

signals:
    void finished();
    void roleBroadcastRequested(const QJsonObject& broadcastObj);
    void notificationTriggered(const QString& username, const QJsonObject& notifObj);
    void userLoggedIn(int userId, const QString& username, QTcpSocket* socket);
    void userDisconnected(QTcpSocket* socket);
    void broadcastRequested(const QJsonObject& obj);

public slots:
    void startProcessing();
    void onReadyRead();
    void onDisconnected();

private:
    qintptr m_socketDescriptor;
    QTcpSocket* m_socket;
    DatabaseManager* m_dbManager;

    void handleRequest(QTcpSocket* socket, const QJsonObject& obj);
    void sendJson(QTcpSocket* socket, const QJsonObject& obj);


    //**************************************************احراز هویت مرکزی********************************************************

    void handleLogin(QTcpSocket* socket, const QJsonObject& data);
    void handleRegister(QTcpSocket* socket, const QJsonObject& data);
    void handleForgotPassword(QTcpSocket* socket, const QJsonObject& data);


    //*********************************************پنل کاربر عادی ( ماژول 1 )****************************************************

    void handleSetFavoriteGenres(QTcpSocket* socket, const QJsonObject& data);
    void handleGetRecommendedBooks(QTcpSocket* socket, const QJsonObject& data);
    void handleGetBooksByGenre(QTcpSocket* socket, const QJsonObject& data);
    void handleGetPopularBooks(QTcpSocket* socket);
    void handleGetNewBooks(QTcpSocket* socket);
    void handleGetBestsellers(QTcpSocket* socket);
    void handleGetFreeBooks(QTcpSocket* socket);
    void handleGetProfile(QTcpSocket* socket, const QJsonObject& data);
    void handleUpdateProfile(QTcpSocket* socket, const QJsonObject& data);
    void handleChangePassword(QTcpSocket* socket, const QJsonObject& data);
    void handleGetPurchaseHistory(QTcpSocket* socket, const QJsonObject& data);
    void handleCheckBookOwnership(QTcpSocket* socket, const QJsonObject& data);
    void handleGetBookPdfPath(QTcpSocket* socket, const QJsonObject& data);


    //*********************************************پنل کاربر عادی ( ماژول 2 )****************************************************

    void handleSearchBooks(QTcpSocket* socket, const QJsonObject& data);


    //*********************************************پنل کاربر عادی ( ماژول 3 )****************************************************

    void handleAddComment(QTcpSocket* socket, const QJsonObject& data);
    void handleEditComment(QTcpSocket* socket, const QJsonObject& data);
    void handleDeleteComment(QTcpSocket* socket, const QJsonObject& data);
    void handleGetComments(QTcpSocket* socket, const QJsonObject& data);


    //*********************************************پنل کاربر عادی ( ماژول 4 )****************************************************

    void handleAddToCart(QTcpSocket* socket, const QJsonObject& data);
    void handleRemoveFromCart(QTcpSocket* socket, const QJsonObject& data);
    void handleGetCart(QTcpSocket* socket, const QJsonObject& data);
    void handleFinalizePurchase(QTcpSocket* socket, const QJsonObject& data);


    //*********************************************پنل کاربر عادی ( ماژول 5 )****************************************************

    //+++++کتاب های خریداری شده+++++
    void handleGetPurchasedBooks(QTcpSocket* socket, const QJsonObject& data);

    //+++++کتاب های ذخیره شده+++++
    void handleSaveBook(QTcpSocket* socket, const QJsonObject& data);
    void handleRemoveSavedBook(QTcpSocket* socket, const QJsonObject& data);
    void handleGetSavedBooks(QTcpSocket* socket, const QJsonObject& data);

    //+++++قفسه ها+++++
    void handleCreateShelf(QTcpSocket* socket, const QJsonObject& data);
    void handleRenameShelf(QTcpSocket* socket, const QJsonObject& data);
    void handleDeleteShelf(QTcpSocket* socket, const QJsonObject& data);
    void handleAddBookToShelf(QTcpSocket* socket, const QJsonObject& data);
    void handleMoveBookBetweenShelves(QTcpSocket* socket, const QJsonObject& data);
    void handleGetShelves(QTcpSocket* socket, const QJsonObject& data);
    void handleGetShelfBooks(QTcpSocket* socket, const QJsonObject& data);


    //*********************************************پنل کاربر عادی ( ماژول 6 )****************************************************

    void handleGetLastReadPage(QTcpSocket* socket, const QJsonObject& data);
    void handleUpdateLastReadPage(QTcpSocket* socket, const QJsonObject& data);


   //************************************************پنل ناشر ( ماژول 1 )*******************************************************

    void handleGetPublisherProfile(QTcpSocket* socket, const QJsonObject& data);
    void handleUpdatePublisherProfile(QTcpSocket* socket, const QJsonObject& data);


    //************************************************پنل ناشر ( ماژول 2 )*******************************************************

    void handleAddBook(QTcpSocket* socket, const QJsonObject& data);
    void handleUpdateBook(QTcpSocket* socket, const QJsonObject& data);
    void handleSetBookDiscount(QTcpSocket* socket, const QJsonObject& data);
    void handleSetBookActiveState(QTcpSocket* socket, const QJsonObject& data);
    void handleGetPublisherBooks(QTcpSocket* socket, const QJsonObject& data);


    //************************************************پنل ناشر ( ماژول 3 )*******************************************************

    void handleGetPublisherStats(QTcpSocket* socket, const QJsonObject& data);


    //*********************************************پنل مدیر سیستم ( ماژول 1 )****************************************************

    void handleGetAllUsers(QTcpSocket* socket, const QJsonObject& data);
    void handleGetUserDetails(QTcpSocket* socket, const QJsonObject& data);
    void handleSearchUsers(QTcpSocket* socket, const QJsonObject& data);


    //*********************************************پنل مدیر سیستم ( ماژول 2 )****************************************************








};

#endif // NETWORKWORKER_H
