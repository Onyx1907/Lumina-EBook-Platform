#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtPreprocessorSupport>


#include "database_manager.h"
#include "constants.h"


class Server:public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QObject* parent = nullptr);
    bool start();

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void onReadyRead();
    void onDisconnected();

private:

    QMap<QString, QTcpSocket*> onlineUsers;      // username → socket
    QMap<QTcpSocket*, QString> socketToUser;    // socket → username


    //***************************************************احراز هویت مرکزی******************************************************
    DatabaseManager dbManager;

    void handleRequest(QTcpSocket* socket, const QJsonObject& obj);
    void handleLogin(QTcpSocket* socket, const QJsonObject& data);
    void handleRegister(QTcpSocket* socket, const QJsonObject& data);
    void handleForgotPassword(QTcpSocket* socket, const QJsonObject& data);
    void sendJson(QTcpSocket* socket, const QJsonObject& obj);


    //*********************************************پنل کاربر عادی ( ماژول 1 ) *************************************************

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



    //*********************************************پنل کاربر عادی ( ماژول 2 )***************************************************


    void handleSearchBooks(QTcpSocket* socket, const QJsonObject& data);



    //*********************************************پنل کاربر عادی ( ماژول 3 )***************************************************


    QList<QTcpSocket*> clients;

    void handleAddComment(QTcpSocket* socket, const QJsonObject& data);
    void handleEditComment(QTcpSocket* socket, const QJsonObject& data);
    void handleDeleteComment(QTcpSocket* socket, const QJsonObject& data);
    void handleGetComments(QTcpSocket* socket, const QJsonObject& data);

    void broadcastToAll(const QJsonObject& obj);



    //*********************************************پنل کاربر عادی ( ماژول 4 )****************************************************


    void handleAddToCart(QTcpSocket* socket, const QJsonObject& data);
    void handleRemoveFromCart(QTcpSocket* socket, const QJsonObject& data);
    void handleGetCart(QTcpSocket* socket, const QJsonObject& data);
    void handleFinalizePurchase(QTcpSocket* socket, const QJsonObject& data);




};

#endif // SERVER_H
