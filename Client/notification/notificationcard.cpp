#include "notificationcard.h"
#include "ui_notificationcard.h"

NotificationCard::NotificationCard(Notification &notification, QWidget *parent)
    : QWidget(parent), m_notification(notification)
    , ui(new Ui::NotificationCard)
{
    ui->setupUi(this);

    updateUi();
}

NotificationCard::~NotificationCard()
{
    delete ui;
}

void NotificationCard::updateUi()
{
    ui->message_label->setText(m_notification.message);

    ui->time_label->setText(m_notification.createdAt);

    if(m_notification.isRead)
    {
        ui->read_pushButton->hide();

        ui->line->hide();
    }
    else{
        ui->read_pushButton->show();

        ui->line->show();
    }

    switch (m_notification.type)
    {
    case NotificationType::NewComment:
        ui->title_label->setText("نظر جدید");
        break;

    case NotificationType::BookSold:
        ui->title_label->setText("فروش کتاب");
        break;

    case NotificationType::NewBookGenre:
        ui->title_label->setText("کتاب جدید");
        break;

    case NotificationType::BookDiscount:
        ui->title_label->setText("تخفیف کتاب");
        break;

    default:
        ui->title_label->setText("اعلان");
        break;
    }
}

void NotificationCard::on_read_pushButton_clicked()
{
    emit markRead(m_notification.id);

    ui->line->hide();
    ui->read_pushButton->hide();
}

