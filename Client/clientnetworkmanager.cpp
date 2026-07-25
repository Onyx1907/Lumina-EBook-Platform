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

void ClientNetworkManager::sendRequest(const QString& action,
                                       const QJsonObject& data,
                                       bool isFlat) {
    if (socket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "cannot send request";
        return;
    }

    QJsonObject packet;
    packet["action"] = action;

    if (isFlat) {
        for (auto it = data.begin(); it != data.end(); ++it)
            packet.insert(it.key(), it.value());
    } else {
        packet["data"] = data;
    }

    qDebug() << packet;

    QJsonDocument doc(packet);
    QByteArray bytes = doc.toJson(QJsonDocument::Compact) + "\n";

    // qDebug().noquote() << bytes;

    socket->write(bytes);
    socket->flush();
}

void ClientNetworkManager::onReadyRead() {
    m_buffer.append(socket->readAll());

    while (!m_buffer.isEmpty()) {
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(m_buffer, &parseError);

        // حالت اول: پکت کاملاً معتبر و تکی است
        if (parseError.error == QJsonParseError::NoError) {
            QJsonObject response = doc.object();
            QString action = response.value("action").toString();

            //  اول بافر را خالی و حلقه را آزاد می‌کنیم، بعد سیگنال می‌دهیم
            m_buffer.clear();

            emit responseReceived(action, response);
            break; // چون بافر خالی شد خارج می‌شویم
        }

        // حالت دوم: چند پکت به هم چسبیده‌اند
        else if (parseError.error == QJsonParseError::GarbageAtEnd) {
            int endIndex = parseError.offset;

            QByteArray firstPacket = m_buffer.left(endIndex);
            QJsonDocument firstDoc = QJsonDocument::fromJson(firstPacket);

            //  اول پکت پردازش شده را از بافر حذف می‌کنیم تا حلقه در امان باشد
            m_buffer.remove(0, endIndex);

            if (!firstDoc.isNull() && firstDoc.isObject()) {
                QJsonObject response = firstDoc.object();
                QString action = response.value("action").toString();
                emit responseReceived(action, response);
            }
            // حلقه ادامه پیدا می‌کند تا پکت‌های بعدیِ چسبیده را بخواند
        }

        // حالت سوم: پکت ناقص است (Fragmented TCP)
        else if (parseError.error == QJsonParseError::UnterminatedObject ||
                 parseError.error == QJsonParseError::IllegalValue) {
            break;
        }

        // حالت چهارم: خطای ساختاری
        else {
            qDebug() << "Real JSON conversion error:" << parseError.errorString();
            m_buffer.clear();
            break;
        }
    }
}
