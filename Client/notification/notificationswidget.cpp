#include "notificationswidget.h"
#include "ui_notificationswidget.h"

NotificationsWidget::NotificationsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::NotificationsWidget)
{
    ui->setupUi(this);
}

NotificationsWidget::~NotificationsWidget()
{
    delete ui;
}
