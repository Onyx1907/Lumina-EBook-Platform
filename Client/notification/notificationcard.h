#ifndef NOTIFICATIONCARD_H
#define NOTIFICATIONCARD_H

#include <QWidget>
#include "notification.h"

namespace Ui {
class NotificationCard;
}

class NotificationCard : public QWidget
{
    Q_OBJECT

public:
    explicit NotificationCard(Notification &notification, QWidget *parent = nullptr);
    ~NotificationCard();

signals:
    void markRead(int id);

private slots:
    void on_read_pushButton_clicked();

private:
    Ui::NotificationCard *ui;
    Notification m_notification;

    void updateUi();
};

#endif // NOTIFICATIONCARD_H
