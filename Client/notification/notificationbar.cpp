#include "notificationbar.h"
#include "ui_notificationbar.h"
#include "clientnetworkmanager.h"

NotificationBar::NotificationBar(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::NotificationBar)
{
    ui->setupUi(this);

    ui->previewLabel->hide();

    initializePreview();

    ui->unread_label->setText(QString::number(unreadCount));

    connect(&ClientNetworkManager::instance(), &ClientNetworkManager::responseReceived,
            this, &NotificationBar::getNotification);
}

NotificationBar::~NotificationBar()
{
    delete ui;
}

void NotificationBar::initializePreview()
{
    opacityEffect = new QGraphicsOpacityEffect(ui->previewLabel);
    ui->previewLabel->setGraphicsEffect(opacityEffect);

    opacityEffect->setOpacity(0);

    fadeInAnimation = new QPropertyAnimation(opacityEffect, "opacity", this);
    fadeInAnimation->setDuration(220);
    fadeInAnimation->setStartValue(0.0);
    fadeInAnimation->setEndValue(1.0);

    fadeOutAnimation = new QPropertyAnimation(opacityEffect, "opacity", this);
    fadeOutAnimation->setDuration(350);
    fadeOutAnimation->setStartValue(1.0);
    fadeOutAnimation->setEndValue(0.0);

    hideTimer = new QTimer(this);
    hideTimer->setSingleShot(true);

    connect(hideTimer, &QTimer::timeout, this, [this](){
                fadeOutAnimation->start();
            });

    connect(fadeOutAnimation, &QPropertyAnimation::finished,
            ui->previewLabel, &QLabel::hide);

    ui->previewLabel->hide();
}

void NotificationBar::showPreview(const QString &message)
{
    hideTimer->stop();

    fadeInAnimation->stop();
    fadeOutAnimation->stop();

    ui->previewLabel->setText(message);
    ui->previewLabel->show();

    opacityEffect->setOpacity(0);

    fadeInAnimation->start();

    hideTimer->start(3000);   // سه ثانیه نمایش
}


void NotificationBar::getNotification(const QString& action, const QJsonObject& data){
    if(action != "NEW_NOTIFICATION_RECEIVED"){
        return;
    }

    showPreview(data.value("message").toString());

    unreadCount++;
    ui->unread_label->setText(QString::number(unreadCount));
}
