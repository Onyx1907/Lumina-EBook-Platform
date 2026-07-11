#include "server.h"
#include "networkWorker.h"
#include <QSqlQuery>
#include <QSqlDatabase>

Server::Server(QObject* parent)
    : QTcpServer(parent),
    dbManager("main_connection")
{
    // دیتابیس اصلی سرور (فقط برای ساخت جدول‌ها و کارهای عمومی)
    if (!dbManager.initDatabase()) {
        qDebug() << "Main DB init failed";
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

    // دیتابیس اختصاصی برای این کلاینت (در ترد خودش)
    auto* workerDb = new DatabaseManager(QString("conn_%1").arg(socketDescriptor));
    if (!workerDb->initDatabase()) {
        qDebug() << "Worker DB init failed for socket" << socketDescriptor;
        delete workerDb;
        delete thread;
        return;
    }

    NetworkWorker* worker = new NetworkWorker(socketDescriptor, workerDb);
    worker->moveToThread(thread);

    // پاک کردن دیتابیس بعد از پایان کار ترد
    connect(worker, &NetworkWorker::finished, thread, [workerDb]() {
        delete workerDb;
    });

    // پخش همگانی (کامنت‌ها و …) به همه‌ی کاربران آنلاین
    connect(worker, &NetworkWorker::broadcastRequested, this,
            [this](const QJsonObject& obj){
                QMutexLocker locker(&mutex);
                for (QTcpSocket* socket : onlineUsers.values()) {
                    if (socket && socket->isOpen()) {
                        QMetaObject::invokeMethod(socket, [socket, obj]() {
                            QJsonDocument doc(obj);
                            QByteArray bytes = doc.toJson(QJsonDocument::Compact);
                            bytes.append('\n');          // مهم برای کلاینتی که line-based می‌خواند
                            socket->write(bytes);
                            socket->flush();
                        }, Qt::QueuedConnection);
                    }
                }
            });

    // پخش بر اساس نقش (Publisher / Admin / …)
    connect(worker, &NetworkWorker::roleBroadcastRequested, this,
            [this](const QJsonObject& broadcastObj){
                QMutexLocker locker(&mutex);

                QString targetRoles = broadcastObj.value("role").toString().trimmed();
                QStringList roleList = targetRoles.split(',', Qt::SkipEmptyParts);
                for (QString &r : roleList) r = r.trimmed();

                for (QTcpSocket* socket : onlineUsers.values()) {
                    if (!socket || !socket->isOpen())
                        continue;

                    int userId = socketToUser.value(socket, -1);
                    bool shouldSend = false;

                    if (roleList.isEmpty()) {
                        shouldSend = true;
                    }
                    else if (userId > 0) {
                        // استفاده از کانکشن اصلی سرور (main_connection)
                        QSqlDatabase db = QSqlDatabase::database("main_connection");
                        if (db.isValid() && db.isOpen()) {
                            QSqlQuery q(db);
                            q.prepare("SELECT role FROM users WHERE id = :id");
                            q.bindValue(":id", userId);
                            if (q.exec() && q.next()) {
                                QString userRole = q.value(0).toString();
                                if (roleList.contains(userRole)) {
                                    shouldSend = true;
                                }
                            }
                        }
                    }

                    if (shouldSend) {
                        QMetaObject::invokeMethod(socket, [socket, broadcastObj]() {
                            QJsonDocument doc(broadcastObj);
                            QByteArray bytes = doc.toJson(QJsonDocument::Compact);
                            bytes.append('\n');
                            socket->write(bytes);
                            socket->flush();
                        }, Qt::QueuedConnection);
                    }
                }
            });

    // نوتیفیکیشن به یک کاربر خاص (با username)
    connect(worker, &NetworkWorker::notificationTriggered, this,
            [this](const QString& username, const QJsonObject& notifObj) {
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

    // ورود کاربر
    connect(worker, &NetworkWorker::userLoggedIn, this,
            [this](int userId, const QString& username, QTcpSocket* socket){
                QMutexLocker locker(&mutex);
                onlineUsers[userId] = socket;
                socketToUser[socket] = userId;
                socketToName[socket] = username;

                emit onlineCountChanged(onlineUsers.size());
                emit clientListChanged(socketToName.values());
                emit logGenerated("NETWORK: User ID '" + QString::number(userId) +
                                  "' (" + username + ") logged in.");
            });

    // خروج کاربر
    connect(worker, &NetworkWorker::userDisconnected, this,
            [this](QTcpSocket* socket){
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

                    emit logGenerated("NETWORK: Client '" + username +
                                      "' (ID: " + QString::number(disconnectedUserId) +
                                      ") connection closed.");
                }
            });

    // مدیریت طول عمر ترد و Worker
    connect(thread, &QThread::started, worker, &NetworkWorker::startProcessing);
    connect(worker, &NetworkWorker::finished, thread, &QThread::quit);
    connect(worker, &NetworkWorker::finished, worker, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    emit logGenerated("NETWORK: Incoming connection detected. Thread context spawned.");
    thread->start();
}

void Server::pushNotification(int userId, const QJsonObject& notif)
{
    QMutexLocker locker(&mutex);
    QTcpSocket* userSocket = onlineUsers.value(userId, nullptr);
    if (!userSocket || !userSocket->isOpen())
        return;

    QJsonObject obj = notif;
    obj["action"] = "NOTIFICATION_PUSH";

    QMetaObject::invokeMethod(userSocket, [userSocket, obj]() {
        QJsonDocument doc(obj);
        QByteArray bytes = doc.toJson(QJsonDocument::Compact);
        bytes.append('\n');
        userSocket->write(bytes);
        userSocket->flush();
    }, Qt::QueuedConnection);
}
