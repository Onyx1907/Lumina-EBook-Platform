#include "purchasehistorywidget.h"
#include "ui_purchasehistorywidget.h"
#include "clientnetworkmanager.h"
#include <QJsonArray>
#include "inovicecard.h"
#include <QTimer>

PurchaseHistoryWidget::PurchaseHistoryWidget(int userID, QWidget *parent)
    : QWidget(parent), m_userID(userID)
    , ui(new Ui::PurchaseHistoryWidget)
{
    ui->setupUi(this);
    ui->error_label->hide();


    connect(&ClientNetworkManager::instance(), &ClientNetworkManager::responseReceived,
            this, &PurchaseHistoryWidget::processNetworkData);
}

PurchaseHistoryWidget::~PurchaseHistoryWidget()
{
    delete ui;
}


void PurchaseHistoryWidget::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);

    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["user_id"] = m_userID;

        ClientNetworkManager::instance().sendRequest("GET_PURCHASE_HISTORY", data);
    }
    else{
        ui->error_label->setText("خطا در برقرار اتصال");
        ui->error_label->show();
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
            ui->error_label->hide();
        });
    }
}

void PurchaseHistoryWidget::processNetworkData(const QString& action, const QJsonObject& data){
    if(action != "GET_PURCHASE_HISTORY_RESPONSE"){
        return;
    }

    if(data.value("status").toString() != "SUCCESS") {
        return;
    }

    ui->total_parchases_label->setText(QString::number(data.value("total_purchases").toInt()) + " عدد");

    ui->listWidget->clear();


    QJsonArray itemsArray = data.value("history").toArray();

    for (const QJsonValue& val : itemsArray) {
        QJsonObject inovice = val.toObject();

        InoviceCard* card = new InoviceCard(inovice.value("book_id").toInt(),
                                            inovice.value("purchase_date").toString(),
                                            inovice.value("title").toString(),
                                            inovice.value("author").toString(),
                                            inovice.value("price").toInt(), this);

        QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
        item->setSizeHint(card->sizeHint());
        ui->listWidget->setItemWidget(item, card);
    }

}


void PurchaseHistoryWidget::on_back_pushButton_clicked()
{
    emit back();
}

