#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>

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
    DatabaseManager dbManager;

    void handleRequest(QTcpSocket* socket, const QJsonObject& obj);
    void handleLogin(QTcpSocket* socket, const QJsonObject& data);
    void handleRegister(QTcpSocket* socket, const QJsonObject& data);
    void handleForgotPassword(QTcpSocket* socket, const QJsonObject& data);
    void sendJson(QTcpSocket* socket, const QJsonObject& obj);
};

#endif // SERVER_H
