#ifndef CLIENTNETWORKMANAGER_H
#define CLIENTNETWORKMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QAbstractSocket>
#include "constants.h"


class ClientNetworkManager : public QObject {
    Q_OBJECT
public:
    // الگوی سینگلتون برای دسترسی یکپارچه در سراسر برنامه
    static ClientNetworkManager& instance();

    bool connectToServer(); // دیگر نیازی به پاس دادن IP و پورت نیست
    void sendRequest(const QString& action, const QJsonObject& data);

signals:
    void responseReceived(const QString& action, const QJsonObject& data);

private slots:
    void onReadyRead();

private:
    ClientNetworkManager(QObject* parent = nullptr);
    ~ClientNetworkManager() = default;
    ClientNetworkManager(const ClientNetworkManager&) = delete;
    ClientNetworkManager& operator=(const ClientNetworkManager&) = delete;

    QTcpSocket* socket;

    QByteArray m_buffer;
};

#endif // CLIENTNETWORKMANAGER_H
