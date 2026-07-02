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
    explicit UserDashboardWidget(RegularUser* cur_user, bool is_first_login, QWidget *parent = nullptr);
    ~UserDashboardWidget();

protected:
    //void resizeEvent(QResizeEvent *event) override;

private:
    Ui::UserDashboardWidget *ui;
    RegularUser *user;
};

#endif // USERDASHBOARDWIDGET_H
