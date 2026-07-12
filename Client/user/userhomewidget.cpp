#include "userhomewidget.h"
#include "ui_userhomewidget.h"
#include "clientnetworkmanager.h"
#include "book.h"
#include "bookcard.h"
#include "constants.h"
#include <QListWidgetItem>
#include <QJsonArray>

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
    //بخش اصلی که برای تست موقتا کامنت کردم
    QJsonObject emptyData;

    // if(ClientNetworkManager::instance().connectToServer()){

    //     ClientNetworkManager::instance().sendRequest("GET_RECOMMENDED_BOOKS", data);

    //     ClientNetworkManager::instance().sendRequest("GET_POPULAR_BOOKS", emptyData);
    //     ClientNetworkManager::instance().sendRequest("GET_NEW_BOOKS", emptyData);
    //     ClientNetworkManager::instance().sendRequest("GET_BESTSELLERS", emptyData);
    //     ClientNetworkManager::instance().sendRequest("GET_FREE_BOOKS", emptyData);
    // }
    // else{
    //     //نمایش صفحه نمایش خطای برقراری اتصال
    // }

    //*************تست خارج از سرور*************
    qDebug() << "--- حالت تست گرافیک با دیتای فیک ---";

    // ۱. ساختن یک آرایه جیسون از کتاب‌های فیک
    QJsonArray mockBooks;

    // کتاب اول
    QJsonObject book1;
    book1["id"] = 1;
    book1["title"] = "شاهنامه فردوسی";
    book1["author"] = "ابوالقاسم فردوسی";
    book1["genre"] = "Biography"; // مطمئن شو تابع stringToGenre این را می‌شناسد
    book1["price"] = 120000;
    book1["discount_percentage"] = 10.0;
    // یک متن بیس۶۴ فیک و بسیار کوتاه برای تست (پیکسل سفید یا خالی)
    book1["cover_base64"] = "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAQAAAC1HAwCAAAAC0lEQVR42mNkYAAAAAYAAjCB0C8AAAAASUVORK5CYII=";
    mockBooks.append(book1);

    // کتاب دوم
    QJsonObject book2;
    book2["id"] = 2;
    book2["title"] = "دیوان حافظ";
    book2["author"] = "شمس‌الدین حافظ";
    book2["genre"] = "SciFi";
    book2["price"] = 85000;
    book2["discount_percentage"] = 0.0;
    book2["cover_base64"] = "";
    mockBooks.append(book2);

    // ۲. بسته‌بندی نهایی دیتا درون آبجکت اصلی
    QJsonObject mockData;
    mockData["books"] = mockBooks;

    // ۳. صدا زدن مستقیم تابع رندر برای لیست محبوب‌ها و جدیدها
    parseAndFillList(mockData, ui->popular_listWidget);
    parseAndFillList(mockData, ui->new_listWidget);
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

    qDebug() << "here : " << booksArray.size();

    for (int i = 0; i < booksArray.size(); ++i) {
        QJsonObject bookObj = booksArray[i].toObject();

        Book book(bookObj["id"].toInt(), bookObj["title"].toString(),
                bookObj["author"].toString(), "",
                stringToGenre(bookObj["genre"].toString()), bookObj["cover_base64"].toString(),
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

