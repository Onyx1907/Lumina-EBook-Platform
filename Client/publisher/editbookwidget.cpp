#include "editbookwidget.h"
#include "ui_editbookwidget.h"
#include <QValidator>
#include <QFileDialog>
#include <QPixmap>
#include "clientnetworkmanager.h"
#include <QTimer>
#include "constants.h"

EditBookWidget::EditBookWidget(QWidget *parent, int publisherID)
    : QWidget(parent), m_publisherID(publisherID)
    , ui(new Ui::EditBookWidget)
{
    ui->setupUi(this);

    ui->price_lineEdit->setValidator(new QIntValidator(this));


    auto validator = new QDoubleValidator(0.0, 100.0, 2, this);
    validator->setNotation(QDoubleValidator::StandardNotation);

    ui->percent_lineEdit->setValidator(validator);

    ui->error_label->hide();

    connect(&ClientNetworkManager::instance(), &ClientNetworkManager::responseReceived,
            this, &EditBookWidget::processNetworkData);

}

EditBookWidget::~EditBookWidget()
{
    delete ui;
}


void EditBookWidget::loadEditBook(int bookID){
    m_bookID = bookID;

    ui->author_lineEdit->setText("");
    ui->description_textEdit->setText("");
    ui->percent_lineEdit->setText("");
    ui->price_lineEdit->setText("");
    ui->title_lineEdit->setText("");

    ui->cover_pix->clear();

    ui->chooseCover_pushButton->setText("انتخاب عکس...");
    ui->choosePDF_pushButton->setText("انتخاب فایل...");

    filePath = "";
    coverPath = "";

    if(bookID != -1){
        ui->error_label->show();
        ui->error_label->setText("تنها فیلد هایی که قصد تغییر دارید را پر کنید");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("توجه کنید که فیلد ژانر نمی‌تواند خالی باشد و تغییر خواهد کرد");
            QTimer::singleShot(3000, this, [this](){
                ui->error_label->setText("");
                ui->error_label->hide();
            });
        });
    }
}

void EditBookWidget::on_chooseCover_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Select Image",
        "",
        "Images (*.png *.jpg *.jpeg *.bmp)"
        );

    ui->chooseCover_pushButton->setText(fileName);

    QPixmap pix(fileName);
    ui->cover_pix->setPixmap(
        pix.scaled(
            ui->cover_pix->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            ));

    coverPath = fileName;
}


void EditBookWidget::on_choosePDF_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Select PDF",
        "",
        "PDF Files (*.pdf)"
        );

    ui->choosePDF_pushButton->setText(fileName);

    filePath = fileName;
}


void EditBookWidget::on_submit_pushButton_clicked()
{
    QString title = ui->title_lineEdit->text().trimmed();
    QString author = ui->author_lineEdit->text().trimmed();
    QString description = ui->description_textEdit->toPlainText().trimmed();
    double price = !ui->price_lineEdit->text().trimmed().isEmpty()
        ? ui->price_lineEdit->text().toDouble() : -1;
    double discount = !ui->percent_lineEdit->text().trimmed().isEmpty()
                          ? ui->percent_lineEdit->text().toDouble() : -1;

    BookGenre genre;
    switch (ui->genre_comboBox->currentIndex()) {
    case 0:
        genre = BookGenre::Fiction;
        break;
    case 1:
        genre = BookGenre::SciFi;
        break;
    case 2:
        genre = BookGenre::Psychology;
        break;
    case 3:
        genre = BookGenre::History;
        break;
    case 4:
        genre = BookGenre::Educational;
        break;
    case 5:
        genre = BookGenre::Biography;
        break;
    case 6:
        genre = BookGenre::Unknown;
        break;
    default:
        genre = BookGenre::Unknown;
        break;
    }

    if(ClientNetworkManager::instance().connectToServer()){

        if(m_bookID == -1){
            if(title.isEmpty() || author.isEmpty() || price == -1 || discount == -1 ||
                filePath.isEmpty() || coverPath.isEmpty()){
                ui->error_label->show();
                ui->error_label->setText("لطفا فیلد ها را تکمیل کنید");
                QTimer::singleShot(3000, this, [this](){
                    ui->error_label->setText("");
                    ui->error_label->hide();
                });
                return;
            }

            QJsonObject data;
            data["publisher_id"] = m_publisherID;
            data["title"] = title;
            data["author"] = author;
            data["description"] = description;
            data["price"] = price;
            data["discountPercent"] = discount;
            data["publisher_pdf_path"] = filePath;
            data["publisher_cover_path"] = coverPath;
            data["genre"] = genreToString(genre);

            ClientNetworkManager::instance().sendRequest("ADD_BOOK", data, true);
        }

        else{
            QJsonObject data;
            data["book_id"] = m_bookID;
            data["publisher_id"] = m_publisherID;
            if(!title.isEmpty()){
                data["title"] = title;
            }
            if(!author.isEmpty()){
                data["author"] = author;
            }
            if(!description.isEmpty()){
                data["description"] = description;
            }
            if(price != -1){
                data["price"] = price;
            }
            if(discount != -1){
                data["discountPercent"] = discount;
            }
            if(!filePath.isEmpty()){
                data["publisher_pdf_path"] = filePath;
            }
            if(!coverPath.isEmpty()){
                data["publisher_cover_path"] = coverPath;
            }
            data["genre"] = genreToString(genre);

            ClientNetworkManager::instance().sendRequest("UPDATE_BOOK", data, true);
        }
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


void EditBookWidget::processNetworkData(const QString& action, const QJsonObject& data){
    if(action != "ADD_BOOK_RESPONSE" &&
        action != "UPDATE_BOOK_RESPONSE"){
        return;
    }

    if(data.value("status").toString() == "SUCCESS"){
        emit back();
        return;
    }
    else{
        ui->error_label->show();
        ui->error_label->setText(data.value("message").toString());
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
            ui->error_label->hide();
        });
    }
}

void EditBookWidget::on_back_pushButton_clicked()
{
    emit back();
}

