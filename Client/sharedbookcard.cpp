#include "sharedbookcard.h"
#include "ui_sharedbookcard.h"
#include <QTimer>
#include "constants.h"
#include "storageutils.h"
#include <QFont>
#include "clientnetworkmanager.h"

SharedBookCard::SharedBookCard(const QJsonObject& obj, QWidget *parent)
    : QWidget(parent), book(obj["id"].toInt(),
                        obj["title"].toString(),
                        obj["author"].toString(),
                        "", stringToGenre(obj["genre"].toString()),
                        obj["coverImagePath"].toString(),
                        obj["price"].toDouble(),
                        obj["discountPercent"].toDouble()),
    isActive((obj["isActive"].toInt()) == 1),
    PDFpath(obj.value("pdfPath").toString())
    , ui(new Ui::SharedBookCard)
{
    ui->setupUi(this);

    ui->tooman_label->show();

    ui->name_label->setText(book.getTitle());
    ui->author_label->setText(book.getAuthor());
    ui->description_label->setText(obj["description"].toString());
    StorageUtils::displayBookCover(book.getCoverImagePath(),
                                   ui->book_cover);


    QString sv = genreToString(book.getGenre());

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


    if(book.getDiscountPercentage() && book.getPrice()){
        ui->discount_label->show();
        ui->oldPrice_label->show();
        ui->discount_label->setText(QString::number(book.getDiscountPercentage(), 'f', 1) + "%");
        ui->oldPrice_label->setText(QString::number(book.getPrice(), 'f', 0));
        ui->finalPrice_label->setText(QString::number(book.getFinalPrice(), 'f', 0));
        QFont font = ui->oldPrice_label->font();
        font.setStrikeOut(true);
        ui->oldPrice_label->setFont(font);
    }
    else{
        ui->discount_label->hide();
        ui->oldPrice_label->hide();
        ui->finalPrice_label->setText(QString::number(book.getFinalPrice(), 'f', 0));
    }
    if(book.getPrice() == 0){
        ui->finalPrice_label->setText("رایگان");
        ui->tooman_label->hide();
    }

    ui->active_checkBox->setChecked(isActive);


    connect(&ClientNetworkManager::instance(), &ClientNetworkManager::responseReceived,
            this, &SharedBookCard::handleCheckIsActive);
}

SharedBookCard::~SharedBookCard()
{
    delete ui;
}

void SharedBookCard::on_delete_pushButton_clicked()
{
    emit deleteRequested(book.getId());
}


void SharedBookCard::on_edit_pushButton_clicked()
{
    emit editRequested(book.getId());
}


void SharedBookCard::on_discount_pushButton_clicked()
{
    emit discountRequested(book.getId());
}


void SharedBookCard::on_active_checkBox_toggled(bool checked)
{
    old_active_state = !checked;

    emit changeActiveRequested(book.getId(), checked);
    ui->active_checkBox->setEnabled(false);

    QTimer::singleShot(10000, this, [this](){
        ui->active_checkBox->setEnabled(true);
    });
}


void SharedBookCard::handleCheckIsActive(const QString& action, const QJsonObject& data){
    if(action != "SET_BOOK_ACTIVE_STATE_RESPONSE"){
        return;
    }

    if(data.value("status").toString() != "SUCCESS"){
        ui->active_checkBox->setChecked(old_active_state);
    }
    else{
        old_active_state = ui->active_checkBox->isChecked();
    }
}
