#ifndef NOTIFICATIONBAR_H
#define NOTIFICATIONBAR_H
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QTimer>
#include <QWidget>
#include <QSoundEffect>

namespace Ui {
class NotificationBar;
}

class NotificationBar : public QWidget
{
    Q_OBJECT

public:
    explicit NotificationBar(QWidget *parent = nullptr);
    ~NotificationBar();

    void setID(int id);
    void sendUnreadNotifsRequest();
    void decreaseUnreadCount();

signals:
    void displayNotifs();

private slots:
    void getNotification(const QString& action, const QJsonObject& data);

    void on_notif_pushButton_clicked();

private:
    Ui::NotificationBar *ui;

    int m_userID;

    QGraphicsOpacityEffect *opacityEffect;
    QPropertyAnimation *fadeInAnimation;
    QPropertyAnimation *fadeOutAnimation;
    QTimer *hideTimer;

    void initializePreview();
    void showPreview(const QString &message);

    void setUnreadCount(int count);
    int unreadCount = 0;

    QSoundEffect m_notificationSound;
};

#endif // NOTIFICATIONBAR_H
