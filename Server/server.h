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
    QMap<int, QTcpSocket*> onlineUsers;       // کلید: userId -> مقدار: سوکت
    QMap<QTcpSocket*, int> socketToUser;      // کلید: سوکت -> مقدار: userId
    QMap<QTcpSocket*, QString> socketToName;  // کلید: سوکت -> مقدار: username

    DatabaseManager dbManager;
    QMutex mutex; // قفل برای امنیت مپ‌ها در محیط مالتی‌تردینگ

signals:
    void onlineCountChanged(int count);
    void logGenerated(const QString &logMessage);
    void systemNotificationGenerated(const QString &message);
    void clientListChanged(const QStringList &usernames);
};

#endif // SERVER_H