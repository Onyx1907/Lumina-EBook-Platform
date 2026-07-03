#include "userhomewidget.h"
#include "ui_userhomewidget.h"

UserHomeWidget::UserHomeWidget(QString cur_username, QWidget *parent)
    : QWidget(parent), username(cur_username)
    , ui(new Ui::UserHomeWidget)
{
    ui->setupUi(this);
}

UserHomeWidget::~UserHomeWidget()
{
    delete ui;
}
