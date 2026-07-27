#include "notificationswidget.h"
#include "ui_notificationswidget.h"
#include "clientnetworkmanager.h"
#include <QTimer>
#include "notification.h"
#include "notificationcard.h"
#include <QJsonArray>


NotificationsWidget::NotificationsWidget(int userID, QString role, QWidget *parent)
    : QWidget(parent), m_userID(userID), m_role(role)
    , ui(new Ui::NotificationsWidget)
{
    ui->setupUi(this);

    ui->error_label->hide();

    connect(&ClientNetworkManager::instance(), &ClientNetworkManager::responseReceived,
            this, &NotificationsWidget::processNetworkData);
}

NotificationsWidget::~NotificationsWidget()
{
    delete ui;
}


void NotificationsWidget::loadNotifs(){

    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["user_id"] = m_userID;
        data["role"] = m_role;

        ClientNetworkManager::instance().sendRequest("GET_NOTIFICATIONS", data, true);
    }
    else{
        ui->error_label->show();
        ui->error_label->setText("خطا در برقراری اتصال");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
            ui->error_label->hide();
        });
    }
}


void NotificationsWidget::processNetworkData(const QString& action, const QJsonObject& data){
    if(action != "GET_NOTIFICATIONS_RESPONSE"){
        return;
    }

    if(action == "GET_NOTIFICATIONS_RESPONSE"){
        displayNotifications(data);
    }
}

void NotificationsWidget::displayNotifications(const QJsonObject& data){
    QJsonArray notifications = data["notifications"].toArray();

    ui->listWidget->clear();

    for (const QJsonValue &value : notifications)
    {
        Notification notification(value.toObject());

        auto *card = new NotificationCard(notification);

        auto *item = new QListWidgetItem(ui->listWidget);

        item->setSizeHint(card->sizeHint());

        ui->listWidget->addItem(item);
        ui->listWidget->setItemWidget(item, card);
    }
}

void NotificationsWidget::on_back_pushButton_clicked()
{
    emit back();
}


void NotificationsWidget::on_back_pushButton_2_clicked()
{
    loadNotifs();
}

