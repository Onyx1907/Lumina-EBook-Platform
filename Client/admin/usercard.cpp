#include "usercard.h"
#include "ui_usercard.h"
#include <QJsonObject>
#include <QDateTime>
#include"QMessageBox"
#include <QTimer>


UserCard::UserCard(const QJsonObject& user, QWidget *parent)
    : QWidget(parent), m_userID(user.value("id").toInt())
    , ui(new Ui::UserCard)
{
    ui->setupUi(this);

    ui->id_label->setText("آیدی کاربر: " + QString::number(user.value("id").toInt()));
    ui->username_label->setText("نام کاربری: " + user.value("username").toString());

    ui->block_checkBox->blockSignals(true);
    ui->block_checkBox->setChecked(user.value("is_blocked").toInt() == 0);
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

void UserCard::on_delete_pushButton_clicked()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("حذف حساب کاربری");
    msgBox.setText("آیا از حذف حساب این کاربر مطمئن هستید؟ امکان بازگردانی وجود ندارد");
    msgBox.setIcon(QMessageBox::Question);


    QPushButton *yesButton = msgBox.addButton("بله، حذف شود", QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton("خیر", QMessageBox::NoRole);
    msgBox.setDefaultButton(noButton);

    // 🎨 جادوی QSS برای شیک کردن کل بدنه و دکمه‌های QMessageBox
    msgBox.setStyleSheet(
        "QMessageBox {"
        "   background-color: #1A1512;" // پس‌زمینه تیره نسکافه‌ای
        "   border: 1px solid #C19A6B;" // مرز طلایی/نسکافه‌ای ملایم
        "   border-radius: 12px;"
        "}"
        "QLabel {"
        "   color: #E6D7C3;" // رنگ متن کرم روشن
        "   font-family: 'Segoe UI', 'Tahoma';"
        "   font-size: 13px;"
        "   padding: 10px;"
        "}"
        "QPushButton {"
        "   background-color: rgba(193, 154, 107, 0.15);"
        "   border: 1px solid rgba(193, 154, 107, 0.4);"
        "   border-radius: 6px;"
        "   color: #E6D7C3;"
        "   font-family: 'Segoe UI', 'Tahoma';"
        "   font-size: 11px;"
        "   min-width: 80px;"
        "   padding: 6px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #C19A6B;"
        "   color: #1E1914;" // متن تیره موقع هاور برای خوانایی
        "   font-weight: bold;"
        "}"
        );

    msgBox.exec();

    if (msgBox.clickedButton() == yesButton) {
        emit deleteRequested(m_userID);
    }
}


void UserCard::on_block_checkBox_toggled(bool checked)
{
    ui->block_checkBox->setEnabled(false);

    QTimer::singleShot(10000, this, [this](){
        ui->block_checkBox->setEnabled(true);
    });
    emit changeBlockedState(m_userID, checked);
}




void UserCard::on_back_pushButton_clicked()
{
    emit detailsRequested(m_userID);
}

