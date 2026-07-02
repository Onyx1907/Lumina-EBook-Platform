#include "userdashboardwidget.h"
#include "ui_userdashboardwidget.h"
#include <QGridLayout>

UserDashboardWidget::UserDashboardWidget(RegularUser* cur_user, bool is_first_login, QWidget *parent)
    : QWidget(parent) , user(cur_user)
    , ui(new Ui::UserDashboardWidget)
{
    ui->setupUi(this);

    //setAttribute(Qt::WA_TranslucentBackground);
    if (this->layout()) {
        delete this->layout();
    }

    ui->containerWidget->setParent(this);
    ui->containerWidget->setFixedSize(900, 650); // یا هر سایزی که در دیزاینر مد نظرت هست
    QGridLayout* finalLayout = new QGridLayout(this);
    finalLayout->addWidget(ui->containerWidget, 0, 0, Qt::AlignCenter);
    this->setLayout(finalLayout);
}


UserDashboardWidget::~UserDashboardWidget()
{
    delete ui;
}
