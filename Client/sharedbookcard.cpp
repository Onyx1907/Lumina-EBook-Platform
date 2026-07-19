#include "sharedbookcard.h"
#include "ui_sharedbookcard.h"
#include <QTimer>
#include "constants.h"
#include "storageutils.h"

SharedBookCard::SharedBookCard(const QJsonObject& obj, QWidget *parent)
    : QWidget(parent), bookObj(obj)
    , ui(new Ui::SharedBookCard)
{
    ui->setupUi(this);

    ui->name_label->setText(bookObj["title"].toString());
    ui->author_label->setText(bookObj["author"].toString());
    ui->genre_label->setText(bookObj["genre"].toString());
    ui->description_label->setText(bookObj["description"].toString());
    StorageUtils::displayBookCover(bookObj["coverImagePath"].toString(),
                                   ui->book_cover);


    if(bookObj["discountPercentage"].toDouble() > 0.0){
        ui->discount_label->show();
        ui->oldPrice_label->show();
        ui->discount_label->setText(QString::number(bookObj["discountPercentage"].toDouble(), 'f', 1) + "%");
        ui->oldPrice_label->setText(QString::number(bookObj["price"].toDouble(), 'f', 0));
        // ui->finalPrice_label->setText(QString::number(book.getFinalPrice(), 'f', 0));
        QFont font = ui->oldPrice_label->font();
        font.setStrikeOut(true);
        ui->oldPrice_label->setFont(font);
    }
    else{
        ui->discount_label->hide();
        ui->oldPrice_label->hide();
        ui->finalPrice_label->setText(QString::number(book.getFinalPrice(), 'f', 0));
    }
}

SharedBookCard::~SharedBookCard()
{
    delete ui;
}

void SharedBookCard::on_delete_pushButton_clicked()
{
    emit deleteRequested(bookObj["id"].toInt());
}


void SharedBookCard::on_edit_pushButton_clicked()
{
    emit editRequested(bookObj["id"].toInt());
}


void SharedBookCard::on_discount_pushButton_clicked()
{
    emit discountRequested(bookObj["id"].toInt());
}


void SharedBookCard::on_active_checkBox_toggled(bool checked)
{
    emit changeActiveRequested(bookObj["id"].toInt(), checked);
    ui->active_checkBox->setEnabled(false);

    QTimer::singleShot(10000, this, [this](){
        ui->active_checkBox->setEnabled(true);
    });
}

