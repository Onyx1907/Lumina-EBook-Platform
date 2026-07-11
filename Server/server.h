#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>
#include <QMap>
#include <QMutex>
#include <QDebug>

#include "database_manager.h"
#include "networkworker.h"
#include "constants.h"

class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QObject* parent = nullptr);
    bool start();

    void pushNotification(int userId, const QJsonObject& notif);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private:
    QMap<int, QTcpSocket*> onlineUsers;
    QMap<QTcpSocket*, int> socketToUser;
    QMap<QTcpSocket*, QString> socketToName;

    DatabaseManager dbManager;
    QMutex mutex; // قفل برای امنیت مپ ها در محیط مالتی تردینگ

signals:
    void onlineCountChanged(int count);
    void logGenerated(const QString &logMessage);
    void systemNotificationGenerated(const QString &message);
    void clientListChanged(const QStringList &usernames);
};

#endif // SERVER_H
