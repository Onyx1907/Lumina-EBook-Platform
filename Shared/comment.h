#ifndef COMMENT_H
#define COMMENT_H

#include <QString>

class Comment
{
private:
    int id;
    int bookID;
    int userID;
    QString username;
    QString text;


public:
    Comment();
};

#endif // COMMENT_H
