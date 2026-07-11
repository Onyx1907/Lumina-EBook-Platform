#include "server.h"
#include "networkworker.h"

Server::Server(QObject* parent) : QTcpServer(parent) {
    dbManager.setConnectionName(QSqlDatabase::defaultConnection);
    if (!dbManager.initDatabase(QSqlDatabase::defaultConnection)) {
        qCritical() << "Failed to initialize main database in Server!";
    }
}

bool Server::start() {
    if (!listen(QHostAddress(SERVER_IP), SERVER_PORT)) {
        qDebug() << "Server listen failed:" << errorString();
        return false;
    }
    qDebug() << "Server listen on" << SERVER_IP << SERVER_PORT;
    return true;
}

void Server::incomingConnection(qintptr socketDescriptor) {
    QThread* thread = new QThread();
    NetworkWorker* worker = new NetworkWorker(socketDescriptor, &dbManager);
    worker->moveToThread(thread);

    // ۱. مدیریت پخش همگانی مسیج ها / کامنت ها (دقیقاً کد خودت)
    connect(worker, &NetworkWorker::broadcastRequested, this, [this](const QJsonObject& obj){
        QMutexLocker locker(&mutex);
        for (QTcpSocket* socket : onlineUsers.values()) {
            if (socket && socket->isOpen()) {
                QMetaObject::invokeMethod(socket, [socket, obj]() {
                    socket->write(QJsonDocument(obj).toJson(QJsonDocument::Compact));
                    socket->flush();
                }, Qt::QueuedConnection);
            }
        }
    });

    // ۲. پخش بر اساس نقش (دقیقاً کد خودت)
    connect(worker, &NetworkWorker::roleBroadcastRequested, this, [this](const QJsonObject& broadcastObj){
        QMutexLocker locker(&mutex);
        QString targetRoles = broadcastObj.value("role").toString().trimmed();
        QStringList roleList = targetRoles.split(',', Qt::SkipEmptyParts);
        for(QString &r : roleList) r = r.trimmed();

        for (QTcpSocket* socket : onlineUsers.values()) {
            if (socket && socket->isOpen()) {
                int userId = socketToUser.value(socket, -1);
                bool shouldSend = false;

                if (roleList.isEmpty()) {
                    shouldSend = true;
                }
                else if (userId > 0) {
                    QSqlQuery q;
                    q.prepare("SELECT role FROM users WHERE id = :id");
                    q.bindValue(":id", userId);
                    if (q.exec() && q.next()) {
                        QString userRole = q.value(0).toString();
                        if (roleList.contains(userRole)) {
                            shouldSend = true;
                        }
                    }
                }

                if (shouldSend) {
                    QMetaObject::invokeMethod(socket, [socket, broadcastObj]() {
                        socket->write(QJsonDocument(broadcastObj).toJson(QJsonDocument::Compact));
                        socket->flush();
                    }, Qt::QueuedConnection);
                }
            }
        }
    });

    // ۳. مدیریت اعلان‌ها (دقیقاً کد خودت)
    connect(worker, &NetworkWorker::notificationTriggered, this, [this](const QString& username, const QJsonObject& notifObj) {
        QMutexLocker locker(&mutex);
        int targetUserId = -1;
        QMapIterator<QTcpSocket*, QString> i(socketToName);
        while (i.hasNext()) {
            i.next();
            if (i.value() == username) {
                targetUserId = socketToUser.value(i.key(), -1);
                break;
            }
        }
        if (targetUserId != -1) {
            locker.unlock();
            pushNotification(targetUserId, notifObj);
        }
    });

    // ۴. ورود کاربر (دقیقاً کد خودت + فقط اضافه شدن ارسال پاسخ لاگین کلاینت دوم)
    connect(worker, &NetworkWorker::userLoggedIn, this, [this](int userId, const QString& username, QTcpSocket* socket){
        QMutexLocker locker(&mutex);
        onlineUsers[userId] = socket;
        socketToUser[socket] = userId;
        socketToName[socket] = username;

        // --- تنها خطی که اضافه شده تا کلاینت دوم آزاد شود و صفحه‌اش تغییر کند ---
        QMetaObject::invokeMethod(socket, [socket]() {
            if (socket && socket->isOpen()) {
                QJsonObject resp;
                resp["action"] = "LOGIN_RESPONSE";
                resp["status"] = "SUCCESS";
                // اضافه کردن n\ حیاتی است تا readLine کلاینت دوم باز شود
                socket->write(QJsonDocument(resp).toJson(QJsonDocument::Compact) + "\n");
                socket->flush();
            }
        }, Qt::QueuedConnection);
        // -----------------------------------------------------------------

        emit onlineCountChanged(onlineUsers.size());
        emit clientListChanged(socketToName.values());
        emit logGenerated("NETWORK: User ID '" + QString::number(userId) + "' (" + username + ") logged in.");
    });

    // ۵. خروج کاربر (دقیقاً کد خودت)
    connect(worker, &NetworkWorker::userDisconnected, this, [this](QTcpSocket* socket){
        QMutexLocker locker(&mutex);
        if (socket && socketToUser.contains(socket)) {
            int disconnectedUserId = socketToUser.value(socket);
            QString username = socketToName.value(socket, "Unknown");

            socketToUser.remove(socket);
            socketToName.remove(socket);
            if (disconnectedUserId != 0) {
                onlineUsers.remove(disconnectedUserId);
            }

            emit onlineCountChanged(onlineUsers.size());
            emit clientListChanged(socketToName.values());
            emit logGenerated("NETWORK: Client '" + username + "' (ID: " + QString::number(disconnectedUserId) + ") connection closed.");
        }
    });

    // مدیریت طول عمر ترد
    connect(thread, &QThread::started, worker, &NetworkWorker::startProcessing);
    connect(worker, &NetworkWorker::finished, thread, &QThread::quit);
    connect(worker, &NetworkWorker::finished, worker, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    emit logGenerated("NETWORK: Incoming connection detected. Thread context spawned.");
    thread->start();
}

// مدیریت ارسال اعلان (دقیقاً کد خودت)
void Server::pushNotification(int userId, const QJsonObject& notif)
{
    QMutexLocker locker(&mutex);
    QTcpSocket* userSocket = onlineUsers.value(userId, nullptr);
    if (!userSocket || !userSocket->isOpen())
        return;

    QJsonObject obj = notif;
    obj["action"] = "NOTIFICATION_PUSH";

    QMetaObject::invokeMethod(userSocket, [userSocket, obj]() {
        userSocket->write(QJsonDocument(obj).toJson(QJsonDocument::Compact));
        userSocket->flush();
    }, Qt::QueuedConnection);
}
