#ifndef COMMENT_H
#define COMMENT_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

class Comment
{
private:
    QJsonObject raw_data;

public:
    Comment(const QJsonObject& data);

    int getId() const;
    int getBookID() const;
    int getUserID() const;
    int getRating() const;
    QString getUsername() const;
    QString getText() const;
    QDateTime createdAt() const;
    QString formattedDate() const;
};

#endif // COMMENT_H
