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
    explicit NotificationsWidget(QWidget *parent = nullptr);
    ~NotificationsWidget();

private:
    Ui::NotificationsWidget *ui;
};

#endif // NOTIFICATIONSWIDGET_H
