#include "comment.h"

Comment::Comment(const QJsonObject& data) : raw_data(data){}

int Comment::getId() const{
    if(raw_data.contains("id"))
        return raw_data["id"].toInt();
    return raw_data["comment_id"].toInt();
}

int Comment::getBookID() const{
    return raw_data["book_id"].toInt();
}

int Comment::getUserID() const{
    return raw_data["user_id"].toInt();
}

int Comment::getRating() const{
    return raw_data["rating"].toInt();
}

QString Comment::getUsername() const{
    return raw_data["username"].toString();
}

QString Comment::getText() const{
    return raw_data["text"].toString();
}

QDateTime Comment::createdAt() const{
    QString dateStr = raw_data["created_at"].toString();

    return QDateTime::fromString(dateStr, Qt::ISODate);
}

QString Comment::formattedData() const{
    return createdAt().toString("yyyy/MM/dd - hh:mm");
}
