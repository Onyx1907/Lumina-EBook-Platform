#include "clientnetworkmanager.h"
#include <QDebug>

ClientNetworkManager::ClientNetworkManager(QObject* parent) : QObject(parent) {
    socket = new QTcpSocket(this);

    // ۱. گوش دادن به دیتای دریافتی
    connect(socket, &QTcpSocket::readyRead, this, &ClientNetworkManager::onReadyRead);

    // ۲. گوش دادن به قطع اتصال
    connect(socket, &QTcpSocket::disconnected, this, &ClientNetworkManager::onDisconnected);

    // ۳. مدیریت خطا (پاسخ به سوال قبلی‌ت برای جلوگیری از کرش)
    connect(socket, &QAbstractSocket::errorOccurred, this, [this](QAbstractSocket::SocketError socketError) {
        qDebug() << "Socket Error: " << socket->errorString();
        emit connectionErrorOccurred("خطا در ارتباط با سرور! لطفاً بررسی کنید که سرور روشن باشد.");
    });
}

void ClientNetworkManager::connectToServer() {
    if (socket->state() == QAbstractSocket::UnconnectedState) {
        // !!! اینجا از همان متغیرهای کانست کتابخانه مشترکتون استفاده می‌کنی
        // فرض می‌کنم اسمشون SERVER_IP و SERVER_PORT باشه (مثل کد هم‌تیمی‌ات)
        socket->connectToHost(SERVER_IP, SERVER_PORT);
    }
}

void ClientNetworkManager::sendRequest(const QString& action, const QJsonObject& data) {
    if (socket->state() != QAbstractSocket::ConnectedState) {
        emit connectionErrorOccurred("شما به سرور متصل نیستید!");
        return;
    }
    QJsonObject request;
    request["action"] = action;
    request["data"] = data;

    QJsonDocument doc(request);
    socket->write(doc.toJson(QJsonDocument::Compact));
    socket->flush();
}

void ClientNetworkManager::onReadyRead() {
    QByteArray raw = socket->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(raw);
    if (!doc.isObject()) return;

    QJsonObject obj = doc.object();
    QString action = obj.value("action").toString();

    if (action == "LOGIN_RESPONSE") {
        bool success = (obj.value("status").toString() == "SUCCESS");
        QString message = obj.value("message").toString();
        QString role = obj.value("user_role").toString();
        emit loginResponseReceived(success, message, role);
    }
    else if (action == "REGISTER_RESPONSE") {
        bool success = (obj.value("status").toString() == "SUCCESS");
        QString message = obj.value("message").toString();
        emit registerResponseReceived(success, message);
    }
    else if (action == "FORGOT_PASSWORD_RESPONSE") {
        emit forgotPasswordResponseReceived(obj);
    }
}

void ClientNetworkManager::onDisconnected() {
    qDebug() << "Disconnected from server.";
}
