#include "librarywidget.h"
#include "ui_librarywidget.h"
#include <QTabWidget>
#include <QTimer>
#include <QJsonArray>
#include <QScrollBar>
#include "clientnetworkmanager.h"
#include "bookcard.h"
#include "constants.h"
#include "bookinfocard.h"
#include <QMenu>

LibraryWidget::LibraryWidget(int userID, QWidget *parent)
    : QWidget(parent), m_userID(userID)
    , ui(new Ui::LibraryWidget)
{
    ui->setupUi(this);

    ui->error_label->hide();


    connect(&ClientNetworkManager::instance(), &ClientNetworkManager::responseReceived,
            this, &LibraryWidget::processNetworkData);

    connect(ui->shelves_listWidget, &QListWidget::itemClicked, this, &LibraryWidget::on_shelf_clicked);


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

    requestShelves();
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


void LibraryWidget::requestShelfBooks(int shelfID){
    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["shelf_id"] = shelfID;

        ClientNetworkManager::instance().sendRequest("GET_SHELF_BOOKS", data, true);
    }
    else{
        ui->error_label->setText("خطا در برقراری اتصال");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
        });
    }
}


void LibraryWidget::processNetworkData(const QString& action, const QJsonObject& data){
    if (action == "GET_PURCHASED_BOOKS_RESPONSE"){
        handleGetPurchasedBooks(data);
    }
    else if(action == "GET_SAVED_BOOKS_RESPONSE"){
        handleGetSavedBooks(data);
    }
    else if(action == "GET_SHELVES_RESPONSE"){
        handleGetShelves(data);
    }
    else if(action == "GET_SHELF_BOOKS_RESPONSE"){
        handleGetShelfBooks(data);
    }
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

        connect(card, &BookCard::clicked, this, &LibraryWidget::on_bookCard_clicked);

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

void LibraryWidget::handleGetShelves(const QJsonObject& response) {
    ui->shelves_listWidget->clear();
    m_shelves.clear();

    if (response["status"].toString() != "SUCCESS") return;


    QJsonArray shelvesArray = response["shelves"].toArray();

    for (const QJsonValue& val : shelvesArray) {
        QJsonObject shelfObj = val.toObject();

        Shelf shelf;
        shelf.id = shelfObj["id"].toInt();
        shelf.name = shelfObj["name"].toString();

        m_shelves.append(shelf);

        QListWidgetItem* item = new QListWidgetItem(ui->shelves_listWidget);
        item->setText(shelf.name);
        item->setIcon(QIcon("qrc:/resources/user/close_shelf.png"));

        item->setData(Qt::UserRole, shelf.id);

        ui->shelves_listWidget->addItem(item);
    }
}


void LibraryWidget::handleGetShelfBooks(const QJsonObject& response){
    ui->shelfBooks_listWidget->clear();

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
            0.0,
            0.0);

        BookCard* card = new BookCard(book, this);

        connect(card, &BookCard::clicked, this, &LibraryWidget::on_bookCard_clicked);

        QListWidgetItem* item = new QListWidgetItem(ui->shelfBooks_listWidget);
        item->setSizeHint(card->sizeHint());
        ui->shelfBooks_listWidget->setItemWidget(item, card);
    }
}

void LibraryWidget::on_shelf_clicked(QListWidgetItem* item){
    if(!item) return;

    for (int i = 0; i < ui->shelves_listWidget->count() ; i++){
        ui->shelves_listWidget->item(i)->setIcon(QIcon("qrc:/resources/user/close_shelf.png"));
    }

    item->setIcon(QIcon("qrc:/resources/user/open_shelf.png"));
    int selectedShelfID = item->data(Qt::UserRole).toInt();

    m_active_shelfID = selectedShelfID;

    requestShelfBooks(selectedShelfID);
}

//هندل کردن کلیک روی کتاب های داخل کتاب های من و کتاب های قفسه ها
void LibraryWidget::on_bookCard_clicked(Book* bookPtr){
    if(!bookPtr) return;

    //handle shelves

    QMenu* menu = new QMenu(this);

    menu->setStyleSheet(
        "QMenu {"
        "   background-color: #F4EBE1;"
        "   border: 1px solid #D4B28C;"
        "   border-radius: 8px;"
        "   padding: 5px 0px;"
        "}"
        "QMenu::item {"
        "   background-color: transparent;"
        "   color: #4A3525;"
        "   padding: 8px 25px 8px 20px;"
        "}"
        "QMenu::item:selected {"
        "   background-color: #4A3525;"
        "   color: #F7DC6F;"
        "}"
        "QMenu::separator {"
        "   height: 1px;"
        "   background-color: #D4B28C;"
        "   margin: 4px 10px;"
        "}"
        );

    menu->addAction("مطالعه کتاب", [this, bookPtr](){
        //open book
    });

    int currentIndex = ui->tabWidget->currentIndex();

    bool isMyBooksTab = (currentIndex == 0);
    bool isShelvesTab = (currentIndex == 2);

    if(isMyBooksTab || isShelvesTab){
        QString menuTitle = isMyBooksTab ? "افزودن به قفسه" : "انتقال به قفسه";

        QMenu* shelvesSubMenu = menu->addMenu(menuTitle);

        for(const Shelf& shelf : m_shelves){
            if(isShelvesTab && m_active_shelfID == shelf.id){
                continue;
            }

            shelvesSubMenu->addAction(shelf.name, [this, bookPtr, shelf, isMyBooksTab](){
                if(isMyBooksTab){
                    //ارسال درخواست اضافه کردن کتاب به قفسه
                }
                else{
                    //درخواست انتقال کتاب از مبدا به مقصد
                }
            });
        }
    }
    menu->exec(QCursor::pos());

    menu->deleteLater();
}



