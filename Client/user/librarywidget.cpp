#include "librarywidget.h"
#include "ui_librarywidget.h"
#include <QTabWidget>
#include <QTimer>
#include <QJsonArray>
#include <QScrollBar>
#include "clientnetworkmanager.h"
#include "bookcard.h"
#include "book.h"
#include "constants.h"
#include "bookinfocard.h"

LibraryWidget::LibraryWidget(int userID, QWidget *parent)
    : QWidget(parent), m_userID(userID)
    , ui(new Ui::LibraryWidget)
{
    ui->setupUi(this);

    ui->error_label->hide();


    connect(&ClientNetworkManager::instance(), &ClientNetworkManager::responseReceived,
            this, &LibraryWidget::processNetworkData);


    ui->purchased_listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->purchased_listWidget->verticalScrollBar()->setSingleStep(15);

    ui->saved_listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->saved_listWidget->verticalScrollBar()->setSingleStep(15);

    ui->shelfBooks_listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->shelfBooks_listWidget->verticalScrollBar()->setSingleStep(15);

    ui->shelves_listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->shelves_listWidget->verticalScrollBar()->setSingleStep(15);
}

LibraryWidget::~LibraryWidget()
{
    delete ui;
}

void LibraryWidget::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);

    int currentIndex = ui->tabWidget->currentIndex();
    on_tabWidget_currentChanged(currentIndex);

    //تست موقت
    QJsonObject fakeResponse;
    fakeResponse["action"] = "GET_PURCHASED_BOOKS_RESPONSE";
    fakeResponse["status"] = "SUCCESS";

    QJsonArray fakeBooks;

    for(int i = 0; i < 17; i++){
        QJsonObject book2;
        book2["id"] = 11;
        book2["title"] = "دیوان حافظ";
        book2["author"] = "حافظ شیرازی";
        book2["genre"] = "Poetry";
        book2["price"] = 180000.0;
        book2["pdfPath"] = "/var/www/uploads/pdfs/hafez.pdf";
        book2["coverImagePath"] = ":/images/hafez.jpg";     // به مسیر عکس خودت تغییر بده
        fakeBooks.append(book2);
    }


    fakeResponse["books"] = fakeBooks;

    handleGetPurchasedBooks(fakeResponse);

    //تست موقت
    QJsonObject fakeResponse2;
    fakeResponse2["action"] = "GET_SAVED_BOOKS_RESPONSE";
    fakeResponse2["status"] = "SUCCESS";

    QJsonArray fakeBooks2;

    for (int i = 0 ; i < 10 ; i++){
        QJsonObject book4;
        book4["id"] = 13;
        book4["title"] = "مثنوی معنوی";
        book4["author"] = "مولانا";
        book4["genre"] = "Mysticism";
        book4["coverImagePath"] = ":/images/masnavi.jpg";  // به مسیر عکس خودت تغییر بده
        fakeBooks2.append(book4);
    }


    fakeResponse2["books"] = fakeBooks2;

    handleGetSavedBooks(fakeResponse2);
}

void LibraryWidget::on_tabWidget_currentChanged(int index)
{
    switch (index) {
    case 0:
        requestPurchasedBooks();
        break;
    case 1:
        requestSavedBooks();
        break;
    case 2:
        requestShelves();
        break;
    }
}


void LibraryWidget::requestPurchasedBooks(){
    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["user_id"] = m_userID;

        ClientNetworkManager::instance().sendRequest("GET_PURCHASED_BOOKS", data);
    }
    else{
        ui->error_label->setText("خطا در برقراری اتصال");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
        });
    }
}

void LibraryWidget::requestSavedBooks(){
    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["user_id"] = m_userID;

        ClientNetworkManager::instance().sendRequest("GET_SAVED_BOOKS", data);
    }
    else{
        ui->error_label->setText("خطا در برقراری اتصال");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
        });
    }
}

void LibraryWidget::requestShelves(){
    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["user_id"] = m_userID;

        ClientNetworkManager::instance().sendRequest("GET_SHELVES", data);
    }
    else{
        ui->error_label->setText("خطا در برقراری اتصال");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
        });
    }
}


void LibraryWidget::processNetworkData(const QString& action, const QJsonObject& data){
    // if (action == "GET_PURCHASED_BOOKS_RESPONSE"){
    //     handleGetPurchasedBooks(data);
    // }
    // else if(action == "GET_SAVED_BOOKS_RESPONSE"){
    //     handleGetSavedBooks(data);
    // }
    //موقت
}

void LibraryWidget::handleGetPurchasedBooks(const QJsonObject& response) {
    ui->purchased_listWidget->clear();

    if (response["status"].toString() != "SUCCESS") return;

    QJsonArray booksArray = response["books"].toArray();
    for (const QJsonValue& val : booksArray) {
        QJsonObject bookObj = val.toObject();

        Book book(
            bookObj["id"].toInt(),
            bookObj["title"].toString(),
            bookObj["author"].toString(),
            "", // ناشر
            stringToGenre(bookObj["genre"].toString()),
            bookObj["coverImagePath"].toString(),
            bookObj["price"].toDouble(),
            0.0);

        BookCard* card = new BookCard(book, this);

        QListWidgetItem* item = new QListWidgetItem(ui->purchased_listWidget);
        item->setSizeHint(card->sizeHint());
        ui->purchased_listWidget->setItemWidget(item, card);
    }
}

void LibraryWidget::handleGetSavedBooks(const QJsonObject& response) {
    ui->saved_listWidget->clear();

    if (response["status"].toString() != "SUCCESS") return;

    QJsonArray booksArray = response["books"].toArray();
    for (const QJsonValue& val : booksArray) {
        QJsonObject bookObj = val.toObject();

        Book book(
            bookObj["id"].toInt(),
            bookObj["title"].toString(),
            bookObj["author"].toString(),
            "", // ناشر
            stringToGenre(bookObj["genre"].toString()),
            bookObj["coverImagePath"].toString(),
            bookObj["price"].toDouble(),
            0.0);

        BookInfoCard* card = new BookInfoCard(book, this);
        card->setCardMode();

        QListWidgetItem* item = new QListWidgetItem(ui->saved_listWidget);
        item->setSizeHint(card->sizeHint());
        ui->saved_listWidget->setItemWidget(item, card);
    }
}


