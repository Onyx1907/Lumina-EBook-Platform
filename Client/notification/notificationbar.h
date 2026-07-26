#ifndef NOTIFICATIONBAR_H
#define NOTIFICATIONBAR_H
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QTimer>
#include <QWidget>

namespace Ui {
class NotificationBar;
}

class NotificationBar : public QWidget
{
    Q_OBJECT

public:
    explicit NotificationBar(QWidget *parent = nullptr);
    ~NotificationBar();

    int unreadCount = 0;

private slots:
    void getNotification(const QString& action, const QJsonObject& data);

private:
    Ui::NotificationBar *ui;

    QGraphicsOpacityEffect *opacityEffect;
    QPropertyAnimation *fadeInAnimation;
    QPropertyAnimation *fadeOutAnimation;
    QTimer *hideTimer;

    void initializePreview();
    void showPreview(const QString &message);
};

#endif // NOTIFICATIONBAR_H
