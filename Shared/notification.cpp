#include "notification.h"

Notification::Notification(const QJsonObject &json)
{
    id = json["id"].toInt();

    userId = json["user_id"].toInt();

    username = json["username"].toString();

    type = stringToType(json["type"].toString());

    message = json["message"].toString();

    isRead = (json["is_read"].toInt() == 1);

    createdAt = QDateTime::fromString(
        json["created_at"].toString(),
        Qt::ISODate
        );
}

NotificationType Notification::stringToType(const QString &type)
{
    if(type == "NEW_COMMENT")
        return NotificationType::NewComment;

    if(type == "BOOK_SOLD")
        return NotificationType::BookSold;

    if(type == "NEW_BOOK_GENRE")
        return NotificationType::NewBookGenre;

    if(type == "BOOK_DISCOUNT")
        return NotificationType::BookDiscount;

    return NotificationType::Unknown;
}
