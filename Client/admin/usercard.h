#ifndef USERCARD_H
#define USERCARD_H

#include <QWidget>
#include "user.h"

namespace Ui {
class UserCard;
}

class UserCard : public QWidget
{
    Q_OBJECT

public:
    explicit UserCard(const QJsonObject& user, QWidget *parent = nullptr);
    ~UserCard();

private:
    Ui::UserCard *ui;

    User *m_user;
};

#endif // USERCARD_H
