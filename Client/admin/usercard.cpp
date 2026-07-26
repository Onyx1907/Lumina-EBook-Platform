#include "usercard.h"
#include "ui_usercard.h"
#include <QJsonObject>
#include <QDateTime>

UserCard::UserCard(const QJsonObject& user, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UserCard)
{
    ui->setupUi(this);

    ui->id_label->setText("آیدی کاربر: " + QString::number(user.value("id").toInt()));
    ui->username_label->setText("نام کاربری: " + user.value("username").toString());

    ui->block_checkBox->blockSignals(true);
    ui->block_checkBox->setChecked(user.value("is_blocked").toInt() == 1);
    ui->block_checkBox->blockSignals(false);

    QString role = user.value("role").toString();
    if(role == "Publisher"){
        ui->role_label->setText("نقش : ناشر");
    }
    else if(role == "RegularUser"){
        ui->role_label->setText("نقش : کاربر عادی");
    }
    else if(role == "Admin"){
        ui->role_label->setText("نقش : ادمین");
    }

    QDateTime dateTime = QDateTime::fromString(user.value("registration_date").toString(), Qt::ISODate);
    ui->date_label->setText("ثبت‌نام شده در: " + dateTime.toString("yyyy/MM/dd - HH:mm:ss"));
}

UserCard::~UserCard()
{
    delete ui;
}
