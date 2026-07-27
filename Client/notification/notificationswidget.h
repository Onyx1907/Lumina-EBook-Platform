#ifndef NOTIFICATIONSWIDGET_H
#define NOTIFICATIONSWIDGET_H

#include <QWidget>

namespace Ui {
class NotificationsWidget;
}

class NotificationsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NotificationsWidget(int userID, QString role, QWidget *parent = nullptr);
    ~NotificationsWidget();

    void loadNotifs();

private slots:
    void processNetworkData(const QString& action, const QJsonObject& data);

    void on_back_pushButton_clicked();

    void on_back_pushButton_2_clicked();

signals:
    void back();

private:
    Ui::NotificationsWidget *ui;

    int m_userID;
    QString m_role;

    void displayNotifications(const QJsonObject& data);
};

#endif // NOTIFICATIONSWIDGET_H
