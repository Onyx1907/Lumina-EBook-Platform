#include "server.h"
#include "networkWorker.h"

Server::Server(QObject* parent) : QTcpServer(parent) {
    dbManager.initDatabase();
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
    NetworkWorker* worker = new NetworkWorker(socketDescriptor, &this->dbManager);
    worker->moveToThread(thread);

    // مدیریت پخش همگانی مسیج ها / کامنت ها به صورت کاملاً ترد-امن
    connect(worker, &NetworkWorker::broadcastRequested, this, [this](const QJsonObject& obj){
        QMutexLocker locker(&mutex); // قفل مپ در طول کپی گرفتن
        for (QTcpSocket* socket : onlineUsers.values()) {
            if (socket && socket->isOpen()) {
                QMetaObject::invokeMethod(socket, [socket, obj]() {
                    socket->write(QJsonDocument(obj).toJson(QJsonDocument::Compact));
                    socket->flush();
                }, Qt::QueuedConnection);
            }
        }
    });

    //اتصال سیگنال پخش بر اساس نقش به منطق فیلترینگ سرور
    connect(worker, &NetworkWorker::roleBroadcastRequested, this, [this](const QJsonObject& broadcastObj){
        QMutexLocker locker(&mutex); // امنیت تردها

        // استخراج نقش های هدف (مثلاً "Publisher" یا "Publisher,Admin")
        QString targetRoles = broadcastObj.value("role").toString().trimmed();
        QStringList roleList = targetRoles.split(',', Qt::SkipEmptyParts);
        for(QString &r : roleList) r = r.trimmed();

        // چرخیدن روی تمام سوکت‌های آنلاین
        for (QTcpSocket* socket : onlineUsers.values()) {
            if (socket && socket->isOpen()) {

                int userId = socketToUser.value(socket, -1);
                bool shouldSend = false;

                // اگر نقشی تعیین نشده بود، برای همه فرستاده میشود
                if (roleList.isEmpty()) {
                    shouldSend = true;
                }
                else if (userId > 0) {
                    // استعلام سریع نقش این کاربر آنلاین از دیتابیس
                    QSqlQuery q;
                    q.prepare("SELECT role FROM users WHERE id = :id");
                    q.bindValue(":id", userId);
                    if (q.exec() && q.next()) {
                        QString userRole = q.value(0).toString();
                        // اگر نقش کاربر متصل شده در لیست نقش‌های هدف بود
                        if (roleList.contains(userRole)) {
                            shouldSend = true;
                        }
                    }
                }

                // ارسال پکت فقط برای کاربران واجد شرایط
                if (shouldSend) {
                    QMetaObject::invokeMethod(socket, [socket, broadcastObj]() {
                        socket->write(QJsonDocument(broadcastObj).toJson(QJsonDocument::Compact));
                        socket->flush();
                    }, Qt::QueuedConnection);
                }
            }
        }
    });


    connect(worker, &NetworkWorker::notificationTriggered, this, [this](const QString& username, const QJsonObject& notifObj) {
        QMutexLocker locker(&mutex);

        int targetUserId = -1;
        // چرخ زدن در لیست کاربران آنلاین برای پیدا کردن آیدی از روی نام کاربری
        QMapIterator<QTcpSocket*, QString> i(socketToName);
        while (i.hasNext()) {
            i.next();
            if (i.value() == username) {
                targetUserId = socketToUser.value(i.key(), -1);
                break;
            }
        }

        // اگر کاربر هدف آنلاین بود، قفل را باز کن و تابع پوش را صدا بزن
        if (targetUserId != -1) {
            locker.unlock();
            pushNotification(targetUserId, notifObj); //خودش بدون تغییر کارش را انجام میدهد
        }
    });

    // ورود امن کاربر با قفل Mutex
    connect(worker, &NetworkWorker::userLoggedIn, this, [this](int userId, const QString& username, QTcpSocket* socket){
        QMutexLocker locker(&mutex);
        onlineUsers[userId] = socket;
        socketToUser[socket] = userId;
        socketToName[socket] = username;

        emit onlineCountChanged(onlineUsers.size());
        emit clientListChanged(socketToName.values());
        emit logGenerated("NETWORK: User ID '" + QString::number(userId) + "' (" + username + ") logged in.");
    });

    // خروج امن کاربر با قفل Mutex
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

    // مدیریت طول عمر ترد، کارگر و سوکت درون آن برای جلوگیری از Memory Leak
    connect(thread, &QThread::started, worker, &NetworkWorker::startProcessing);
    connect(worker, &NetworkWorker::finished, thread, &QThread::quit);
    connect(worker, &NetworkWorker::finished, worker, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    emit logGenerated("NETWORK: Incoming connection detected. Thread context spawned.");
    thread->start();
}

// مدیریت ارسال امن اعلان به تردِ اختصاصیِ سوکت کلاینت
void Server::pushNotification(int userId, const QJsonObject& notif)
{
    QMutexLocker locker(&mutex);
    QTcpSocket* userSocket = onlineUsers.value(userId, nullptr);
    if (!userSocket || !userSocket->isOpen())
        return;

    QJsonObject obj = notif;
    obj["action"] = "NOTIFICATION_PUSH";

    // ارسال به ترد فرعی سوکت با invokeMethod جهت رعایت قانون نخ امنی
    QMetaObject::invokeMethod(userSocket, [userSocket, obj]() {
        userSocket->write(QJsonDocument(obj).toJson(QJsonDocument::Compact));
        userSocket->flush();
    }, Qt::QueuedConnection);
}