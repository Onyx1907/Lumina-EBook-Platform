#include "userhomewidget.h"
#include "ui_userhomewidget.h"
#include "clientnetworkmanager.h"
#include "book.h"
#include "bookcard.h"
#include "constants.h"
#include <QListWidgetItem>
#include <QJsonArray>
#include <QScrollBar>

UserHomeWidget::UserHomeWidget(int ID, QWidget *parent)
    : QWidget(parent), userID(ID)
    , ui(new Ui::UserHomeWidget)
{
    ui->setupUi(this);

    connect(&ClientNetworkManager::instance(), &ClientNetworkManager::responseReceived,
            this, &UserHomeWidget::processNetworkData);
}


void UserHomeWidget::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);

    QJsonObject data;
    data["user_id"] = userID;

    QJsonObject emptyData;

    if(ClientNetworkManager::instance().connectToServer()){

        ClientNetworkManager::instance().sendRequest("GET_RECOMMENDED_BOOKS", data);

        ClientNetworkManager::instance().sendRequest("GET_POPULAR_BOOKS", emptyData);
        ClientNetworkManager::instance().sendRequest("GET_NEW_BOOKS", emptyData);
        ClientNetworkManager::instance().sendRequest("GET_BESTSELLERS", emptyData);
        ClientNetworkManager::instance().sendRequest("GET_FREE_BOOKS", emptyData);
    }
    else{
        emit disconnected();
    }
}

void UserHomeWidget::processNetworkData(const QString& action, const QJsonObject& data){    
    if (action == "GET_RECOMMENDED_BOOKS_RESPONSE") {
        parseAndFillList(data, ui->recommended_listWidget);
    }
    else if (action == "GET_POPULAR_BOOKS_RESPONSE") {
        parseAndFillList(data, ui->popular_listWidget);
    }
    else if (action == "GET_NEW_BOOKS_RESPONSE") {
        parseAndFillList(data, ui->new_listWidget);
    }
    else if (action == "GET_BESTSELLERS_RESPONSE") {
        parseAndFillList(data, ui->bestSellers_listWidget);
    }
    else if (action == "GET_FREE_BOOKS_RESPONSE") {
        parseAndFillList(data, ui->free_listWidget);
    }
}


void UserHomeWidget::parseAndFillList(const QJsonObject &data, QListWidget *targetList){
    targetList->clear();

    // مستقیم می‌رویم سراغ آرایه کتاب‌ها (چون دیتا از قبل آبجکت شده)
    QJsonArray booksArray = data["books"].toArray();

    for (int i = 0; i < booksArray.size(); ++i) {
        QJsonObject bookObj = booksArray[i].toObject();

        Book book(bookObj["id"].toInt(), bookObj["title"].toString(),
                bookObj["author"].toString(), "",
                stringToGenre(bookObj["genre"].toString()), bookObj["cover_image_path"].toString(),
                bookObj["price"].toDouble(), bookObj["discount_percentage"].toDouble());

        // ساخت کارت و تزریق به لیست
        QListWidgetItem *item = new QListWidgetItem(targetList);
        BookCard *card = new BookCard(book, this);

        item->setSizeHint(card->sizeHint());
        targetList->setItemWidget(item, card);
        //کانکت به کلیک روی کتاب
        connect(card, &BookCard::clicked, this, [this](Book* bookptr){
           emit bookSelected(bookptr);
        });
    }
//تست و عیب یابی
    // QListWidgetItem *testItem = new QListWidgetItem(targetList);
    // testItem->setText("سلام من اینجام");
    // targetList->addItem(testItem);
}

UserHomeWidget::~UserHomeWidget()
{
    delete ui;
}

