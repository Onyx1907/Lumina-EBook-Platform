#include "searchwidget.h"
#include <QTimer>
#include "ui_searchwidget.h"
#include "clientnetworkmanager.h"
#include <QJsonArray>


SearchWidget::SearchWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SearchWidget)
{
    ui->setupUi(this);


    connect(&ClientNetworkManager::instance(), &ClientNetworkManager::responseReceived,
            this, &SearchWidget::processNetworkData);
}

SearchWidget::~SearchWidget()
{
    delete ui;
}

void SearchWidget::on_educational_pushButton_clicked()
{

    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["genre"] = "Educational";

        ClientNetworkManager::instance().sendRequest("GET_BOOKS_BY_GENRE", data);
    }
    else{
        ui->error_label->setText("خطا در برقراری اتصال");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
        });
    }
}


void SearchWidget::on_biography_pushButton_clicked()
{
    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["genre"] = "Biography";

        ClientNetworkManager::instance().sendRequest("GET_BOOKS_BY_GENRE", data);
    }
    else{
        ui->error_label->setText("خطا در برقراری اتصال");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
        });
    }
}


void SearchWidget::on_scifi_pushButton_clicked()
{
    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["genre"] = "SciFi";

        ClientNetworkManager::instance().sendRequest("GET_BOOKS_BY_GENRE", data);
    }
    else{
        ui->error_label->setText("خطا در برقراری اتصال");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
        });
    }
}


void SearchWidget::on_psychology_pushButton_clicked()
{
    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["genre"] = "Psychology";

        ClientNetworkManager::instance().sendRequest("GET_BOOKS_BY_GENRE", data);
    }
    else{
        ui->error_label->setText("خطا در برقراری اتصال");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
        });
    }
}


void SearchWidget::on_history_pushButton_clicked()
{
    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["genre"] = "History";

        ClientNetworkManager::instance().sendRequest("GET_BOOKS_BY_GENRE", data);
    }
    else{
        ui->error_label->setText("خطا در برقراری اتصال");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
        });
    }
}


void SearchWidget::on_ficition_pushButton_clicked()
{
    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["genre"] = "Fiction";

        ClientNetworkManager::instance().sendRequest("GET_BOOKS_BY_GENRE", data);
    }
    else{
        ui->error_label->setText("خطا در برقراری اتصال");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
        });
    }
}


void SearchWidget::on_search_pushButton_clicked()
{
    QString title = ui->title_lineEdit->text().trimmed();
    QString author = ui->author_lineEdit->text().trimmed();
    QString publisher = ui->publisher_lineEdit->text().trimmed();

    if(title.isEmpty() && author.isEmpty() && publisher.isEmpty()){
        ui->error_label->setText("برای جستجو باید حداقل یک فیلد را پر کنید");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
        });
        return;
    }

    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["title"] = title;
        data["author"] = author;
        data["publisher_name"] = publisher;

        ClientNetworkManager::instance().sendRequest("SEARCH_BOOKS", data);
    }
    else{
        ui->error_label->setText("خطا در برقراری اتصال");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
        });
    }

}



void SearchWidget::processNetworkData(const QString& action, const QJsonObject& data){
    if(action != "SEARCH_BOOKS_RESPONSE" &&
        action != "GET_BOOKS_BY_GENRE_RESPONSE"){
        return;
    }

    if(data.value("status").toString() != "SUCCESS"){
        ui->error_label->setText("خطای ناشناخته");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
        });
        return;
    }

    //چک کردن دریافت داده
    qDebug() << data;

    QJsonArray booksArray = data["books"].toArray();

    QVector<Book> results;

    for (int i = 0; i < booksArray.size(); ++i) {
        QJsonObject bookObj = booksArray[i].toObject();

        Book book(bookObj["id"].toInt(),
                  bookObj["title"].toString(),
                  bookObj["author"].toString(),
                  bookObj["publisher_name"].toString(),
                  stringToGenre(bookObj["genre"].toString()),
                  bookObj["cover_image_path"].toString(),
                  bookObj["price"].toDouble(),
                  bookObj["discount_percentage"].toDouble());

        results.append(book);
    }

    emit searchCompleted(results);
}


