#include "publisherbookwidget.h"
#include "ui_publisherbookwidget.h"
#include "clientnetworkmanager.h"
#include <QJsonArray>
#include <QTimer>
#include <QScrollBar>

PublisherBookWidget::PublisherBookWidget(int publisherID, QWidget *parent)
    : QWidget(parent), m_publisherID(publisherID)
    , ui(new Ui::PublisherBookWidget)
{
    ui->setupUi(this);

    ui->error_label->hide();

    ui->listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->listWidget->verticalScrollBar()->setSingleStep(15);

    connect(&ClientNetworkManager::instance(), &ClientNetworkManager::responseReceived,
            this, &PublisherBookWidget::processNetworkData);

}

void PublisherBookWidget::loadBooks(){
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

void PublisherBookWidget::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);

    loadBooks();
}

PublisherBookWidget::~PublisherBookWidget()
{
    delete ui;
}


void PublisherBookWidget::processNetworkData(const QString& action, const QJsonObject& data){
    if(action != "GET_PUBLISHER_BOOKS_RESPONSE" &&
        action != "SET_BOOK_ACTIVE_STATE_RESPONSE" &&
        action != "PUBLISHER_DELETE_BOOK_RESPONSE" &&
        action != "SET_BOOK_DISCOUNT_RESPONSE"){
        return;
    }

    if (action == "PUBLISHER_DELETE_BOOK_RESPONSE"){
        loadBooks();
    }

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
    else if(action == "SET_BOOK_DISCOUNT_RESPONSE"){

        for (int i = 0; i < ui->listWidget->count(); i++){
            SharedBookCard *card =
                qobject_cast<SharedBookCard*>(
                    ui->listWidget->itemWidget(ui->listWidget->item(i)));
            if(!card) continue;

            if(card->getBookID() == m_bookID){
                card->updatePrice(m_discount);
            }
        }
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

        connect(itemWidget, &SharedBookCard::editRequested, this, [this](int bookID){
            emit editBook(bookID);
        });
        connect(itemWidget, &SharedBookCard::changeActiveRequested, this,
                &PublisherBookWidget::checkIsActiveRequested);

        connect(itemWidget, &SharedBookCard::goToPDF, this, [this](QString path){
            emit PDFreader(path);
        });

        connect(itemWidget, &SharedBookCard::deleteRequested, this,
                &PublisherBookWidget::deleteBookRequested);

        connect(itemWidget, &SharedBookCard::discountRequested, this,
                &PublisherBookWidget::setDiscountRequested);

        connect(itemWidget, &SharedBookCard::goToComments, this, [this](int bookID){
            emit goToCommentsPage(bookID);
        });
    }
}

void PublisherBookWidget::on_back_pushButton_clicked()
{
    emit addBook();
}



void PublisherBookWidget::checkIsActiveRequested(int bookID, bool isActive){
    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["publisher_id"] = m_publisherID;
        data["book_id"] = bookID;
        data["active"] = isActive;

        ClientNetworkManager::instance().sendRequest("SET_BOOK_ACTIVE_STATE", data, true);
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


void PublisherBookWidget::deleteBookRequested(int bookID){
    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["publisher_id"] = m_publisherID;
        data["book_id"] = bookID;

        ClientNetworkManager::instance().sendRequest("PUBLISHER_DELETE_BOOK", data, true);
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


void PublisherBookWidget::setDiscountRequested(int bookID, double percent){
    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["publisher_id"] = m_publisherID;
        data["book_id"] = bookID;
        data["discountPercent"] = percent;

        m_bookID = bookID;
        m_discount = percent;

        ClientNetworkManager::instance().sendRequest("SET_BOOK_DISCOUNT", data, true);
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
