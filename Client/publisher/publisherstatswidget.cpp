#include "publisherstatswidget.h"
#include "ui_publisherstatswidget.h"
#include "clientnetworkmanager.h"
#include <QTimer>
#include <QJsonArray>
#include "bookcard.h"
#include "book.h"

PublisherStatsWidget::PublisherStatsWidget(int publisherID, QWidget *parent)
    : QWidget(parent), m_publisherID(publisherID)
    , ui(new Ui::PublisherStatsWidget)
{
    ui->setupUi(this);

    ui->error_label->hide();

    connect(&ClientNetworkManager::instance(), &ClientNetworkManager::responseReceived,
            this, &PublisherStatsWidget::processNetworkData);
}

PublisherStatsWidget::~PublisherStatsWidget()
{
    delete ui;
}


void PublisherStatsWidget::showEvent(QShowEvent *event){
    QWidget::showEvent(event);

    QJsonObject data;
    data["publisher_id"] = m_publisherID;

    if(ClientNetworkManager::instance().connectToServer()){

        ClientNetworkManager::instance().sendRequest("GET_PUBLISHER_STATS", data, true);
    }
    else{
        //نمایش صفحه نمایش خطای برقراری اتصال
    }
}

void PublisherStatsWidget::on_books_pushButton_clicked()
{
    emit goToBooks();
}

void PublisherStatsWidget::processNetworkData(const QString& action, const QJsonObject& data)
{
    if (action != "GET_PUBLISHER_STATS_RESPONSE")
        return;

    if (!data.contains("status") || data["status"].toString() != "SUCCESS")
    {
        ui->error_label->setText("خطا در برقراری ارتباط با سرور");
        ui->error_label->show();

        QTimer::singleShot(3000, this, [this]()
                           {
                               ui->error_label->clear();
                               ui->error_label->hide();
                           });

        return;
    }

    ui->bestSellers_listWidget->clear();
    ui->worstSellers_listWidget->clear();

    QJsonObject stats = data["stats"].toObject();

    ui->total_books_label->setText(QString::number(stats["totalBooks"].toInt()) + " عدد");
    ui->total_revenue_label->setText(QString::number(stats["totalRevenue"].toInt()) + " تومان");

    QJsonArray bestSellers = stats["bestSellers"].toArray();

    for (const QJsonValue &value : bestSellers)
    {
        QJsonObject bookObj = value.toObject();

        Book book(
            bookObj["book_id"].toInt(),
            bookObj["title"].toString(),
            "",
            "",
            BookGenre::Unknown,
            "",
            0.0
            );

        int salesCount = bookObj["salesCount"].toInt();

        QListWidgetItem *item = new QListWidgetItem(ui->bestSellers_listWidget);
        BookCard *card = new BookCard(book, this, salesCount);

        item->setSizeHint(card->sizeHint());
        ui->bestSellers_listWidget->addItem(item);
        ui->bestSellers_listWidget->setItemWidget(item, card);
    }

    QJsonArray worstSellers = stats["worstSellers"].toArray();

    for (const QJsonValue &value : worstSellers)
    {
        QJsonObject bookObj = value.toObject();

        Book book(
            bookObj["book_id"].toInt(),
            bookObj["title"].toString(),
            "",
            "",
            BookGenre::Unknown,
            "",
            0.0
            );

        int salesCount = bookObj["salesCount"].toInt();

        QListWidgetItem *item = new QListWidgetItem(ui->worstSellers_listWidget);
        BookCard *card = new BookCard(book, this, salesCount);

        item->setSizeHint(card->sizeHint());
        ui->worstSellers_listWidget->addItem(item);
        ui->worstSellers_listWidget->setItemWidget(item, card);
    }
}
