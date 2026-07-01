#include "userdashboardwidget.h"
#include "ui_userdashboardwidget.h"

UserDashboardWidget::UserDashboardWidget(RegularUser* cur_user, QWidget *parent)
    : QWidget(parent) , user(cur_user)
    , ui(new Ui::UserDashboardWidget)
{
    ui->setupUi(this);
}

UserDashboardWidget::~UserDashboardWidget()
{
    delete ui;
}
