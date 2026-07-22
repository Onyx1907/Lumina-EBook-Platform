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
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>

LibraryWidget::LibraryWidget(int userID, QWidget *parent)
    : QWidget(parent), m_userID(userID)
    , ui(new Ui::LibraryWidget)
{
    ui->setupUi(this);

    ui->error_label->hide();

    ui->shelves_listWidget->setIconSize(QSize(50, 50));

    //راست کلیک روی قفسه ها
    ui->shelves_listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->shelves_listWidget, &QListWidget::customContextMenuRequested,
            this, &LibraryWidget::onShelfContextMenuRequested);


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

    int currentIndex = ui->tabWidget->currentIndex();
    on_tabWidget_currentChanged(currentIndex);
}

void LibraryWidget::on_tabWidget_currentChanged(int index)
{
    switch (index) {
    case 0:
        requestPurchasedBooks();
        requestShelves();
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
        ui->error_label->show();
        ui->error_label->setText("خطا در برقراری اتصال");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
            ui->error_label->hide();
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
        ui->error_label->show();
        ui->error_label->setText("خطا در برقراری اتصال");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
            ui->error_label->hide();
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
        ui->error_label->show();
        ui->error_label->setText("خطا در برقراری اتصال");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
            ui->error_label->hide();
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
        ui->error_label->show();
        ui->error_label->setText("خطا در برقراری اتصال");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
            ui->error_label->hide();
        });
    }
}


void LibraryWidget::processNetworkData(const QString& action, const QJsonObject& data){
    if(action != "GET_PURCHASED_BOOKS_RESPONSE" &&
        action != "GET_SAVED_BOOKS_RESPONSE" &&
        action != "GET_SHELVES_RESPONSE" &&
        action != "GET_SHELF_BOOKS_RESPONSE" &&
        action != "CREATE_SHELF_RESPONSE" &&
        action != "RENAME_SHELF_RESPONSE" &&
        action != "DELETE_SHELF_RESPONSE" &&
        action != "ADD_BOOK_TO_SHELF_RESPONSE" &&
        action != "MOVE_BOOK_BETWEEN_SHELVES_RESPONSE"){
        return;
    }

    if(data["status"].toString() != "SUCCESS"){
        ui->error_label->show();
        if(data.contains("message")){
            QString message = data["message"].toString();
            ui->error_label->setText(message);
            requestShelfBooks(m_active_shelfID);
        }
        else{
            ui->error_label->setText("خطای ناشناخته");
        }
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
            ui->error_label->hide();
        });
        return;
    }

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
    else if(action == "CREATE_SHELF_RESPONSE" ||
               action == "RENAME_SHELF_RESPONSE" ||
               action == "DELETE_SHELF_RESPONSE" ||
               action == "ADD_BOOK_TO_SHELF_RESPONSE" ||
               action == "MOVE_BOOK_BETWEEN_SHELVES_RESPONSE"){
        requestShelves();
        ui->shelfBooks_listWidget->clear();
    }
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
            bookObj["publisher_name"].toString(),
            stringToGenre(bookObj["genre"].toString()),
            bookObj["coverImagePath"].toString(),
            bookObj["price"].toDouble(),
            0.0);

        BookInfoCard* card = new BookInfoCard(book, this);
        card->setCardMode();

        connect(card, &BookInfoCard::clicked, this, [this](Book* clickedBookptr){
            emit bookSelected(clickedBookptr);
        });

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

        item->setIcon(QIcon(":/resources/user/close_shelf.png"));

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
        ui->shelves_listWidget->item(i)->setIcon(QIcon(":/resources/user/close_shelf.png"));
    }

    item->setIcon(QIcon(":/resources/user/open_shelf.png"));
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
        emit goToPDF(bookPtr->getId());
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
                    sendAddBookToShelfRequest(bookPtr->getId(), shelf.id);
                }
                else{
                    sendMoveBookRequest(bookPtr->getId(), m_active_shelfID, shelf.id);
                }
            });
        }
    }
    menu->exec(QCursor::pos());

    menu->deleteLater();
}


void LibraryWidget::onShelfContextMenuRequested(const QPoint &pos) {
    QListWidgetItem* item = ui->shelves_listWidget->itemAt(pos);
    if (!item) return;

    int shelfId = item->data(Qt::UserRole).toInt();
    QString oldName = item->text();

    QMenu* menu = new QMenu(this);
    menu->setStyleSheet(
        "QMenu { background-color: #1A1512; border: 1px solid #C19A6B; border-radius: 8px; padding: 5px 0px; }"
        "QMenu::item { background-color: transparent; color: #E6D7C3; padding: 8px 25px; font-family: 'Segoe UI', 'Tahoma'; }"
        "QMenu::item:selected { background-color: #C19A6B; color: #1A1512; font-weight: bold; }"
        );

    menu->addAction("تغییر نام قفسه", [this, shelfId, oldName]() {
        QInputDialog inputDialog(this);
        inputDialog.setWindowTitle("تغییر نام قفسه");
        inputDialog.setLabelText("نام جدید قفسه را وارد کنید:");
        inputDialog.setTextValue(oldName);

        inputDialog.setStyleSheet(
            "QInputDialog { background-color: #1A1512; border: 1px solid #C19A6B; border-radius: 12px; }"
            "QLabel { color: #E6D7C3; font-family: 'Segoe UI', 'Tahoma'; font-size: 13px;  }"
            "QLineEdit { background-color: #261F1A; border: 1px solid rgba(193, 154, 107, 0.4); border-radius: 6px; color: #FFF; padding: 6px; font-family: 'Segoe UI', 'Tahoma'; selection-background-color: #C19A6B; }"
            "QPushButton { background-color: rgba(193, 154, 107, 0.15); border: 1px solid rgba(193, 154, 107, 0.4); border-radius: 6px; color: #E6D7C3; font-family: 'Segoe UI', 'Tahoma'; font-size: 11px; min-width: 80px; padding: 6px; }"
            "QPushButton:hover { background-color: #C19A6B; color: #1E1914; font-weight: bold; }"
            );

        inputDialog.setOkButtonText("تغییر نام");
        inputDialog.setCancelButtonText("انصراف");

        if (inputDialog.exec() == QDialog::Accepted) {
            QString newName = inputDialog.textValue();
            if (!newName.isEmpty() && newName != oldName) {
                this->sendRenameShelfRequest(shelfId, newName);
            }
        }
    });

    menu->addAction("حذف قفسه", [this, shelfId, oldName]() {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("حذف قفسه");
        msgBox.setText(QString("آیا از حذف قفسه \"%1\" مطمئن هستید؟").arg(oldName));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        // پیدا کردن دکمه‌های Yes و No و تغییر متن آن‌ها به سبک Qt6
        if (auto yesButton = msgBox.button(QMessageBox::Yes)) yesButton->setText("بله، حذف شود");
        if (auto noButton = msgBox.button(QMessageBox::No)) noButton->setText("خیر");

        msgBox.setStyleSheet(
            "QMessageBox { background-color: #1A1512; border: 1px solid #C19A6B; border-radius: 12px; }"
            "QLabel { color: #E6D7C3; font-family: 'Segoe UI', 'Tahoma'; font-size: 13px; padding: 10px; qproperty-alignment: 'AlignRight'; }"
            "QPushButton { background-color: rgba(193, 154, 107, 0.15); border: 1px solid rgba(193, 154, 107, 0.4); border-radius: 6px; color: #E6D7C3; font-family: 'Segoe UI', 'Tahoma'; font-size: 11px; min-width: 80px; padding: 6px; }"
            "QPushButton:hover { background-color: #C19A6B; color: #1E1914; font-weight: bold; }"
            );

        if (msgBox.exec() == QMessageBox::Yes) {
            this->sendDeleteShelfRequest(shelfId);
        }
    });

    menu->exec(QCursor::pos());
    menu->deleteLater();
}

void LibraryWidget::on_pushButton_clicked()
{
    QInputDialog inputDialog(this);
    inputDialog.setWindowTitle("قفسه جدید");
    inputDialog.setLabelText("نام قفسه جدید را وارد کنید:");
    inputDialog.setTextValue("");

    inputDialog.setStyleSheet(
        "QInputDialog { background-color: #1A1512; border: 1px solid #C19A6B; border-radius: 12px; }"
        "QLabel { color: #E6D7C3; font-family: 'Segoe UI', 'Tahoma'; font-size: 13px; }"
        "QLineEdit { background-color: #261F1A; border: 1px solid rgba(193, 154, 107, 0.4); border-radius: 6px; color: #FFF; padding: 6px; font-family: 'Segoe UI', 'Tahoma'; selection-background-color: #C19A6B; }"
        "QPushButton { background-color: rgba(193, 154, 107, 0.15); border: 1px solid rgba(193, 154, 107, 0.4); border-radius: 6px; color: #E6D7C3; font-family: 'Segoe UI', 'Tahoma'; font-size: 11px; min-width: 80px; padding: 6px; }"
        "QPushButton:hover { background-color: #C19A6B; color: #1E1914; font-weight: bold; }"
        );

    inputDialog.setOkButtonText("ساخت قفسه");
    inputDialog.setCancelButtonText("انصراف");

    if (inputDialog.exec() == QDialog::Accepted) {
        QString shelfName = inputDialog.textValue().trimmed();

        if (!shelfName.isEmpty()) {
            this->sendCreateShelfRequest(shelfName);
        }
    }
}


void LibraryWidget::sendCreateShelfRequest(QString shelfName){
    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["user_id"] = m_userID;
        data["name"] = shelfName;

        ClientNetworkManager::instance().sendRequest("CREATE_SHELF", data, true);
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

void LibraryWidget::sendRenameShelfRequest(int shelfId, QString newName){
    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["shelf_id"] = shelfId;
        data["new_name"] = newName;

        ClientNetworkManager::instance().sendRequest("RENAME_SHELF", data, true);
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


void LibraryWidget::sendDeleteShelfRequest(int shelfId){
    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["shelf_id"] = shelfId;

        ClientNetworkManager::instance().sendRequest("DELETE_SHELF", data, true);
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


void LibraryWidget::sendAddBookToShelfRequest(int bookId, int shelfId){
    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["book_id"] = bookId;
        data["shelf_id"] = shelfId;

        ClientNetworkManager::instance().sendRequest("ADD_BOOK_TO_SHELF", data, true);
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

void LibraryWidget::sendMoveBookRequest(int bookId, int fromShelfId, int toShelfId){
    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["book_id"] = bookId;
        data["from_shelf"] = fromShelfId;
        data["to_shelf"] = toShelfId;

        ClientNetworkManager::instance().sendRequest("MOVE_BOOK_BETWEEN_SHELVES", data, true);
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




