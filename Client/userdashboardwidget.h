#ifndef USERDASHBOARDWIDGET_H
#define USERDASHBOARDWIDGET_H

#include <QWidget>
#include <regularuser.h>


namespace Ui {
class UserDashboardWidget;
}

class UserDashboardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UserDashboardWidget(RegularUser* cur_user, QWidget *parent = nullptr);
    ~UserDashboardWidget();

private:
    Ui::UserDashboardWidget *ui;
    RegularUser *user;
};

#endif // USERDASHBOARDWIDGET_H
