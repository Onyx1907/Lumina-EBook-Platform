#include "notificationswidget.h"
#include "ui_notificationswidget.h"
#include "clientnetworkmanager.h"
#include <QTimer>
#include "notification.h"
#include "notificationcard.h"
#include <QJsonArray>
#include <QScrollBar>


NotificationsWidget::NotificationsWidget(int userID, QString role, QWidget *parent)
    : QWidget(parent), m_userID(userID), m_role(role)
    , ui(new Ui::NotificationsWidget)
{
    ui->setupUi(this);

    ui->error_label->hide();

    ui->listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->listWidget->verticalScrollBar()->setSingleStep(15);

    connect(&ClientNetworkManager::instance(), &ClientNetworkManager::responseReceived,
            this, &NotificationsWidget::processNetworkData);
}

NotificationsWidget::~NotificationsWidget()
{
    delete ui;
}


void NotificationsWidget::loadNotifs(){
    pendingScrollValue = ui->listWidget->verticalScrollBar()->value();

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
    if(action != "GET_NOTIFICATIONS_RESPONSE" &&
        action != "MARK_NOTIFICATION_READ_RESPONSE"){
        return;
    }

    if(data.value("status").toString() != "SUCCESS"){
        ui->error_label->show();
        ui->error_label->setText("مشکلی پیش آمده لطفا دوباره امتحان کنید");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
            ui->error_label->hide();
        });

        return;
    }

    if(action == "GET_NOTIFICATIONS_RESPONSE"){
        displayNotifications(data);
    }
    else if(action == "MARK_NOTIFICATION_READ_RESPONSE"){
        emit decreaseCount();
    }
}

void NotificationsWidget::displayNotifications(const QJsonObject& data){
    QJsonArray notifications = data["notifications"].toArray();

    ui->listWidget->clear();

    for (const QJsonValue &value : notifications)
    {
        Notification notification(value.toObject());

        NotificationCard *card = new NotificationCard(notification);

        QListWidgetItem *item = new QListWidgetItem(ui->listWidget);

        item->setSizeHint(card->sizeHint());

        ui->listWidget->setItemWidget(item, card);

        connect(card, &NotificationCard::markRead, this,
                &NotificationsWidget::sendMarkReadRequest);
    }

    QTimer::singleShot(0, this, [this]() {
        auto *bar = ui->listWidget->verticalScrollBar();

        bar->setValue(qMin(pendingScrollValue, bar->maximum()));
    });
}

void NotificationsWidget::on_back_pushButton_clicked()
{
    emit back();
}


void NotificationsWidget::on_refresh_pushButton_clicked()
{
    loadNotifs();
}


void NotificationsWidget::sendMarkReadRequest(int id){

    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["notification_id"] = id;

        ClientNetworkManager::instance().sendRequest("MARK_NOTIFICATION_READ", data, true);
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
