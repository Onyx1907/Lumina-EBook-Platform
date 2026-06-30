#include "clientnetworkmanager.h"
#include <QDebug>

ClientNetworkManager& ClientNetworkManager::instance(){
    static ClientNetworkManager m_instance;
    return m_instance;
}

ClientNetworkManager::ClientNetworkManager(QObject* parent) : QObject(parent) {
    socket = new QTcpSocket(this);

    // گوش دادن به دیتای دریافتی
    connect(socket, &QTcpSocket::readyRead, this, &ClientNetworkManager::onReadyRead);
}

bool ClientNetworkManager::connectToServer() {
    // اگر از قبل وصل بودیم، نیازی به اتصال مجدد نیست
    if (socket && socket->state() == QAbstractSocket::ConnectedState) {
        return true;
    }

    // تلاش برای اتصال به سرور
    socket->connectToHost(SERVER_IP, SERVER_PORT);

    // حداکثر ۳ ثانیه منتظر می‌ماند تا سوکت وصل شود (UI را قفل نمی‌کند چون زمانش کم است)
    if (socket->waitForConnected(3000)) {
        qDebug() << "connected to server successfully";
        return true; // اتصال موفقیت‌آمیز بود
    }

    qDebug() << "failed to connect to server";
    return false; // اتصال ناموفق بود
}

void ClientNetworkManager::sendRequest(const QString& action, const QJsonObject& data) {
    if (socket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "cannot send request";
        return;
    }
    QJsonObject packet;
    packet["action"] = action;
    packet["data"] = data;

    QJsonDocument doc(packet);
    socket->write(doc.toJson(QJsonDocument::Compact));
    //socket->flush();
}

void ClientNetworkManager::onReadyRead() {
    QByteArray raw = socket->readAll();

    QJsonDocument doc = QJsonDocument::fromJson(raw);
    if (!doc.isObject() || doc.isNull()){
        qDebug() << "invalid json data from server";
        return;
    }

    QJsonObject response = doc.object();
    QString action = response.value("action").toString();
    QJsonObject data = response.value("data").toObject();


    emit responseReceived(action, data);
}

