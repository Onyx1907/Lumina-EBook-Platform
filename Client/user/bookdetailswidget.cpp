#include "bookdetailswidget.h"
#include "ui_bookdetailswidget.h"
#include "clientnetworkmanager.h"
#include "constants.h"
#include <QString>
#include <QFont>
#include <QTimer>
#include <QScrollBar>

BookDetailsWidget::BookDetailsWidget(int ID, QWidget *parent)
    : QWidget(parent), userID(ID)
    , ui(new Ui::BookDetailsWidget)
{
    ui->setupUi(this);


    connect(&ClientNetworkManager::instance(), &ClientNetworkManager::responseReceived,
            this, &BookDetailsWidget::processNetworkData);
}

void BookDetailsWidget::loadBook(Book *book){

    cur_book = book;

    ui->name_label->setText(book->getTitle());
    ui->author_label->setText(book->getAuthor());
    ui->publisher_label->setText(book->getPublisher());
    QString sv = genreToString(book->getGenre());

    if (sv == "Biography")
        ui->genre_label->setText("زندگینامه");
    else if (sv == "Educational")
        ui->genre_label->setText("علمی و تحصیلی");
    else if (sv == "Psychology")
        ui->genre_label->setText("روانشناسی");
    else if (sv == "Fiction")
        ui->genre_label->setText("ادبیات داستانی");
    else if (sv == "History")
        ui->genre_label->setText("تاریخی");
    else if (sv == "SciFi")
        ui->genre_label->setText("علمی تخیلی");
    else
        ui->genre_label->setText("");

    if(book->getDiscountPercentage()){
        ui->discount_label->show();
        ui->oldPrice_label->show();
        ui->discount_label->setText(QString::number(book->getDiscountPercentage(), 'f', 1) + "%");
        ui->oldPrice_label->setText(QString::number(book->getPrice(), 'f', 0));
        ui->finalPrice_label->setText(QString::number(book->getFinalPrice(), 'f', 0));
        QFont font = ui->oldPrice_label->font();
        font.setStrikeOut(true);
        ui->oldPrice_label->setFont(font);
    }
    else{
        ui->discount_label->hide();
        ui->oldPrice_label->hide();
        ui->finalPrice_label->setText(QString::number(book->getFinalPrice(), 'f', 0));
    }

    ui->addCart_pushButton->hide();
    ui->removeCart_pushButton->hide();
    ui->study_pushButton->hide();
    // ui->comments->hide(); //موقت
    // ui->comments_pushButton->hide(); //موقت
    // ui->saveBook_pushButton->hide();
    ui->savedBook_pushButton->hide();
    ui->disconnected->hide(); // موقت

    if(ClientNetworkManager::instance().connectToServer()){
        QJsonObject data;
        data["book_id"] = book->getId();
        data["user_id"] = userID;

        ClientNetworkManager::instance().sendRequest("CHECK_BOOK_OWNERSHIP", data);
    }
    else{
        ui->disconnected->show();
        ui->error_label->setText("خطا در برقراری اتصال");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
        });
    }

    QTimer::singleShot(0, this, [this]() {
        if (ui->scrollArea && ui->scrollArea->verticalScrollBar()) {
            ui->scrollArea->verticalScrollBar()->setValue(0);
        }
    });
}

BookDetailsWidget::~BookDetailsWidget()
{
    delete ui;
}

void BookDetailsWidget::processNetworkData(const QString& action, const QJsonObject& data){

    qDebug() << data;

    if(action == "CHECK_BOOK_OWNERSHIP_RESPONSE"){
        if(data.value("status").toString() == "SUCCESS"){
            ui->disconnected->hide();

            ui->comments->show();
            ui->comments_pushButton->show();

            if(data.value("is_purchased").toBool()){
                ui->study_pushButton->show();
            }
            else if(data.value("is_in_cart").toBool()){
                ui->removeCart_pushButton->show();
            }
            else{
                ui->addCart_pushButton->show();
            }
            if(data.value("is_saved").toBool()){
                ui->savedBook_pushButton->show();
            }
            else{
                ui->saveBook_pushButton->show();
            }

            ui->rating_label->setText(QString::number(data.value("rating").toDouble(), 'f', 1));
            ui->publisher_label->setText(data.value("publisher").toString());
        }
        else if(data.value("status").toString() == "FAILED"){
            ui->disconnected->show();
            ui->error_label->setText(data.value("message").toString());
            QTimer::singleShot(3000, this, [this](){
                ui->error_label->setText("");
            });
        }
    }

    else if(action == "ADD_TO_CART_RESPONSE"){
        if(data.value("status").toString() == "SUCCESS"){
            ui->error_label->setText(data.value("message").toString());
            QTimer::singleShot(3000, this, [this](){
                ui->error_label->setText("");
            });
            ui->addCart_pushButton->hide();
            ui->removeCart_pushButton->show();
        }
        else if(data.value("status").toString() == "ERROR"){
            ui->error_label->setText(data.value("message").toString());
            QTimer::singleShot(3000, this, [this](){
                ui->error_label->setText("");
            });
        }
    }

    else if(action == "REMOVE_FROM_CART_RESPONSE"){
        if(data.value("status").toString() == "SUCCESS"){
            ui->error_label->setText(data.value("message").toString());
            QTimer::singleShot(3000, this, [this](){
                ui->error_label->setText("");
            });
            ui->addCart_pushButton->show();
            ui->removeCart_pushButton->hide();
        }
        else if(data.value("status").toString() == "ERROR"){
            ui->error_label->setText(data.value("message").toString());
            QTimer::singleShot(3000, this, [this](){
                ui->error_label->setText("");
            });
        }
    }

    else if(action == "SAVE_BOOK_RESPONSE"){
        if(data.value("status").toString() == "SUCCESS"){
            ui->error_label->setText(data.value("message").toString());
            QTimer::singleShot(3000, this, [this](){
                ui->error_label->setText("");
            });
            loadBook(cur_book);
        }
        else if(data.value("status").toString() == "ERROR"){
            ui->error_label->setText(data.value("message").toString());
            QTimer::singleShot(3000, this, [this](){
                ui->error_label->setText("");
            });
        }
    }
    else if(action == "REMOVE_SAVED_BOOK_RESPONSE"){
        if(data.value("status").toString() == "SUCCESS"){
            ui->error_label->setText(data.value("message").toString());
            QTimer::singleShot(3000, this, [this](){
                ui->error_label->setText("");
            });
            loadBook(cur_book);
        }
        else if(data.value("status").toString() == "ERROR"){
            ui->error_label->setText(data.value("message").toString());
            QTimer::singleShot(3000, this, [this](){
                ui->error_label->setText("");
            });
        }
    }
}

void BookDetailsWidget::on_back_pushButton_clicked()
{
    emit backPrevious();
}


void BookDetailsWidget::on_addCart_pushButton_clicked()
{
    if(ClientNetworkManager::instance().connectToServer()){
        QJsonObject data;
        data["book_id"] = cur_book->getId();
        data["user_id"] = userID;

        ClientNetworkManager::instance().sendRequest("ADD_TO_CART", data);
    }
    else{
        ui->error_label->setText("خطا در برقراری اتصال");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
        });
    }
}


void BookDetailsWidget::on_removeCart_pushButton_clicked()
{
    if(ClientNetworkManager::instance().connectToServer()){
        QJsonObject data;
        data["book_id"] = cur_book->getId();
        data["user_id"] = userID;

        ClientNetworkManager::instance().sendRequest("REMOVE_FROM_CART", data);
    }
    else{
        ui->error_label->setText("خطا در برقراری اتصال");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
        });
    }
}


void BookDetailsWidget::on_comments_pushButton_clicked()
{
    emit goToComments(cur_book->getId());
}


void BookDetailsWidget::on_saveBook_pushButton_clicked()
{
    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["user_id"] = userID;
        data["book_id"] = cur_book->getId();

        ClientNetworkManager::instance().sendRequest("SAVE_BOOK", data, true);
    }
    else{
        ui->error_label->setText("خطا در برقراری اتصال");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
        });
    }
}


void BookDetailsWidget::on_savedBook_pushButton_clicked()
{
    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["user_id"] = userID;
        data["book_id"] = cur_book->getId();

        ClientNetworkManager::instance().sendRequest("REMOVE_SAVED_BOOK", data, true);
    }
    else{
        ui->error_label->setText("خطا در برقراری اتصال");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
        });
    }
}

