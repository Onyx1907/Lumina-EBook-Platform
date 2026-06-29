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
    static ClientNetworkManager& instance() {
        static ClientNetworkManager _instance;
        return _instance;
    }

    void connectToServer(); // دیگر نیازی به پاس دادن IP و پورت نیست
    void sendRequest(const QString& action, const QJsonObject& data);

signals:
    // سیگنال‌های مربوط به پاسخ سرور
    void loginResponseReceived(bool success, const QString& message, const QString& role);
    void registerResponseReceived(bool success, const QString& message);
    void forgotPasswordResponseReceived(const QJsonObject& response);

    // سیگنال اعلام خطا به UI برای جلوگیری از کرش
    void connectionErrorOccurred(const QString& errorMessage);

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    ClientNetworkManager(QObject* parent = nullptr);
    ~ClientNetworkManager() = default;
    ClientNetworkManager(const ClientNetworkManager&) = delete;
    ClientNetworkManager& operator=(const ClientNetworkManager&) = delete;

    QTcpSocket* socket;
};

#endif // CLIENTNETWORKMANAGER_H
