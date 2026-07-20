#include "publisherbookwidget.h"
#include "ui_publisherbookwidget.h"
#include "clientnetworkmanager.h"
#include <QJsonArray>
#include <QTimer>

PublisherBookWidget::PublisherBookWidget(int publisherID, QWidget *parent)
    : QWidget(parent), m_publisherID(publisherID)
    , ui(new Ui::PublisherBookWidget)
{
    ui->setupUi(this);

    ui->error_label->hide();


    connect(&ClientNetworkManager::instance(), &ClientNetworkManager::responseReceived,
            this, &PublisherBookWidget::processNetworkData);
}

void PublisherBookWidget::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);

    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["publisher_id"] = m_publisherID;

        ClientNetworkManager::instance().sendRequest("GET_PUBLISHER_BOOKS", data, true);
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

PublisherBookWidget::~PublisherBookWidget()
{
    delete ui;
}


void PublisherBookWidget::processNetworkData(const QString& action, const QJsonObject& data){
    if(data.value("status").toString() != "SUCCESS"){
        ui->error_label->show();
        ui->error_label->setText(data["message"].toString());
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
            ui->error_label->hide();
        });
        return;
    }

    if(action == "GET_PUBLISHER_BOOKS_RESPONSE"){
        handleGetBooks(data);
    }


}


void PublisherBookWidget::handleGetBooks(const QJsonObject& data){
    ui->listWidget->clear();

    QJsonArray booksArray = data["books"].toArray();

    for (const QJsonValue& value : booksArray) {
        QJsonObject bookObj = value.toObject();

        SharedBookCard* itemWidget = new SharedBookCard(bookObj, this);

        QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
        item->setSizeHint(itemWidget->sizeHint());
        ui->listWidget->setItemWidget(item, itemWidget);
    }
}

void PublisherBookWidget::on_back_pushButton_clicked()
{
    emit addBook();
}

