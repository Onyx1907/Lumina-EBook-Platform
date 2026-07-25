#include "adminbookswidget.h"
#include "ui_adminbookswidget.h"
#include "clientnetworkmanager.h"
#include "sharedbookcard.h"
#include <QJsonArray>
#include <QTimer>
#include <QScrollBar>

AdminBooksWidget::AdminBooksWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdminBooksWidget)
{
    ui->setupUi(this);


    ui->error_label->hide();

    ui->listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->listWidget->verticalScrollBar()->setSingleStep(15);

    connect(&ClientNetworkManager::instance(), &ClientNetworkManager::responseReceived,
            this, &AdminBooksWidget::processNetworkData);
}

AdminBooksWidget::~AdminBooksWidget()
{
    delete ui;
}

void AdminBooksWidget::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);

    loadBooks();
}

void AdminBooksWidget::loadBooks(){
    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;

        ClientNetworkManager::instance().sendRequest("GET_ALL_BOOKS", data, true);
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


void AdminBooksWidget::processNetworkData(const QString& action, const QJsonObject& data){
    qDebug() <<data;

    if(action != "GET_ALL_BOOKS_RESPONSE" &&
        action != "ADMIN_DELETE_BOOK_RESPONSE"){
        return;
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

    if(action == "GET_ALL_BOOKS_RESPONSE"){
        handleGetBooks(data);
    }
    else if(action == "ADMIN_DELETE_BOOK_RESPONSE"){
        loadBooks();
    }
}


void AdminBooksWidget::handleGetBooks(const QJsonObject& data){
    ui->listWidget->clear();

    QJsonArray booksArray = data["books"].toArray();

    for (const QJsonValue& value : booksArray) {
        QJsonObject bookObj = value.toObject();

        SharedBookCard* itemWidget = new SharedBookCard(bookObj, this, true);

        QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
        item->setSizeHint(itemWidget->sizeHint());
        ui->listWidget->setItemWidget(item, itemWidget);

        connect(itemWidget, &SharedBookCard::deleteRequested, this,
                &AdminBooksWidget::deleteBookRequested);

        connect(itemWidget, &SharedBookCard::editRequested, this, [this](int bookID){
            emit editBook(bookID);
        });
    }
}


void AdminBooksWidget::deleteBookRequested(int bookID){
    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["book_id"] = bookID;

        ClientNetworkManager::instance().sendRequest("ADMIN_DELETE_BOOK", data, true);
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
