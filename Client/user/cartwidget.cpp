#include "cartwidget.h"
#include "ui_cartwidget.h"
#include <QJsonObject>
#include <QTimer>
#include <QJsonArray>
#include <QMessageBox>
#include <QScrollBar>
#include "clientnetworkmanager.h"
#include "constants.h"
#include "bookinfocard.h"

CartWidget::CartWidget(int userID, QWidget *parent)
    : QWidget(parent), m_userID(userID)
    , ui(new Ui::CartWidget)
{
    ui->setupUi(this);

    ui->error_label->hide();
    ui->empty_cart_label->hide();


    ui->listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->listWidget->verticalScrollBar()->setSingleStep(15);


    connect(&ClientNetworkManager::instance(), &ClientNetworkManager::responseReceived,
            this, &CartWidget::processNetworkData);
}

CartWidget::~CartWidget()
{
    delete ui;
}


void CartWidget::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);

    loadCartFromServer();
}


void CartWidget::loadCartFromServer(){

    QJsonObject data;
    data["user_id"] = m_userID;


    if(ClientNetworkManager::instance().connectToServer()){

        ClientNetworkManager::instance().sendRequest("GET_CART", data);
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

void CartWidget::processNetworkData(const QString& action, const QJsonObject& data) {
    if (action != "GET_CART_RESPONSE" &&
        action != "REMOVE_FROM_CART_RESPONSE" &&
        action != "FINALIZE_PURCHASE_RESPONSE") {
        return;
    }

    qDebug() << data;

    if (action == "GET_CART_RESPONSE") {
        handleGetCartResponse(data);
    }

    else if (action == "FINALIZE_PURCHASE_RESPONSE" ||
               action == "REMOVE_FROM_CART_RESPONSE") {

        if(data["status"].toString() != "SUCCESS"){
            if(data.contains("message")){
                ui->error_label->setText(data["message"].toString());
                ui->error_label->show();
                QTimer::singleShot(3000, this, [this](){
                    ui->error_label->setText("");
                    ui->error_label->hide();
                });
            }
            else{
                ui->error_label->setText("خطای ناشناخته");
                ui->error_label->show();
                QTimer::singleShot(3000, this, [this](){
                    ui->error_label->setText("");
                    ui->error_label->hide();
                });
            }
        }

            loadCartFromServer();

    }
}


void CartWidget::handleGetCartResponse(const QJsonObject& response) {
    QString status = response.value("status").toString();

    if (status != "SUCCESS") {
        ui->error_label->setText("خطای ناشناخته");
        ui->error_label->show();
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
            ui->error_label->hide();
        });
        return;
    }

    ui->empty_cart_label->hide();
    ui->listWidget->show();
    ui->pay_pushButton->setEnabled(true);
    ui->listWidget->clear();

    QJsonArray itemsArray = response.value("items").toArray();

    if(itemsArray.isEmpty()){
        ui->listWidget->hide();
        ui->empty_cart_label->show();
        ui->pay_pushButton->setEnabled(false);

    }

    for (const QJsonValue& val : itemsArray) {
        QJsonObject bookObj = val.toObject();


        Book book(bookObj["id"].toInt(),
                  bookObj["title"].toString(),
                  bookObj["author"].toString(),
                  bookObj["publisher_name"].toString(),
                  BookGenre::Unknown,
                  bookObj["coverImagePath"].toString(),
                  bookObj["price"].toDouble(),
                  bookObj["discount"].toDouble());


        BookInfoCard* card = new BookInfoCard(book, this);

        card->setCardMode(BookInfoCard::Cart);

        connect(card, &BookInfoCard::removeRequested, this, &CartWidget::onBookRemoveRequested);

        QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
        item->setSizeHint(card->sizeHint());
        ui->listWidget->setItemWidget(item, card);

        connect(card, &BookInfoCard::clicked, this, [this](Book* clickedBookptr){
            emit bookSelected(clickedBookptr);
        });
    }

    ui->count_label->setText(QString::number(response.value("count").toInt()) + " عدد");
    ui->total_price_label->setText(QString::number(response.value("total_price").toDouble(), 'f', 0) + " تومان");
    ui->discount_label->setText(QString::number(response.value("discount").toDouble(), 'f', 0) + " تومان");
    ui->final_price_label->setText(QString::number(response.value("final_price").toDouble(), 'f', 0) + " تومان");

    m_finalPrice = response.value("final_price").toDouble();
}


void CartWidget::onBookRemoveRequested(int bookId) {

    if(ClientNetworkManager::instance().connectToServer()){
        QJsonObject data;
        data["book_id"] = bookId;
        data["user_id"] = m_userID;

        ClientNetworkManager::instance().sendRequest("REMOVE_FROM_CART", data);
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

void CartWidget::on_pay_pushButton_clicked()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("پرداخت نهایی");
    msgBox.setText("آیا مایل به پرداخت نهایی صورت‌حساب خود هستید؟");
    msgBox.setIcon(QMessageBox::Question);


    QPushButton *yesButton = msgBox.addButton("بله، پرداخت ", QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton("خیر", QMessageBox::NoRole);
    msgBox.setDefaultButton(noButton);

    msgBox.setStyleSheet(
        "QMessageBox {"
        "   background-color: #1A1512;" // پس‌زمینه تیره نسکافه‌ای
        "   border: 1px solid #C19A6B;" // مرز طلایی/نسکافه‌ای ملایم
        "   border-radius: 12px;"
        "}"
        "QLabel {"
        "   color: #E6D7C3;" // رنگ متن کرم روشن
        "   font-family: 'Segoe UI', 'Tahoma';"
        "   font-size: 13px;"
        "   padding: 10px;"
        "}"
        "QPushButton {"
        "   background-color: rgba(193, 154, 107, 0.15);"
        "   border: 1px solid rgba(193, 154, 107, 0.4);"
        "   border-radius: 6px;"
        "   color: #E6D7C3;"
        "   font-family: 'Segoe UI', 'Tahoma';"
        "   font-size: 11px;"
        "   min-width: 80px;"
        "   padding: 6px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #C19A6B;"
        "   color: #1E1914;" // متن تیره موقع هاور برای خوانایی
        "   font-weight: bold;"
        "}"
        );

    msgBox.exec();

    if (msgBox.clickedButton() == yesButton) {
        if(ClientNetworkManager::instance().connectToServer()){
            QJsonObject data;
            data["user_id"] = m_userID;
            data["client_final_price"] = m_finalPrice;

            ClientNetworkManager::instance().sendRequest("FINALIZE_PURCHASE", data);
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
}


void CartWidget::on_back_pushButton_clicked()
{
    emit goToHistory();
}

