#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QDateTime>
#include <QJsonObject>
#include <QString>

enum class NotificationType
{
    Unknown,

    NewComment,
    BookSold,
    NewBookGenre,
    BookDiscount,
};

class Notification
{
public:
    Notification() = default;
    explicit Notification(const QJsonObject &json);

    int id = -1;
    int userId = -1;

    QString username;

    NotificationType type = NotificationType::Unknown;

    QString message;

    bool isRead = false;

    QString createdAt;

private:
    static NotificationType stringToType(const QString &type);
};

#endif // NOTIFICATION_H
