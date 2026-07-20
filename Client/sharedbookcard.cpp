#include "sharedbookcard.h"
#include "ui_sharedbookcard.h"
#include <QTimer>
#include "constants.h"
#include "storageutils.h"
#include <QFont>

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


    ui->name_label->setText(book.getTitle());
    ui->author_label->setText(book.getAuthor());
    ui->genre_label->setText(genreToString(book.getGenre()));
    ui->description_label->setText(obj["description"].toString());
    StorageUtils::displayBookCover(book.getCoverImagePath(),
                                   ui->book_cover);


    if(book.getDiscountPercentage()){
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

    ui->active_checkBox->setChecked(isActive);
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
    emit changeActiveRequested(book.getId(), checked);
    ui->active_checkBox->setEnabled(false);

    QTimer::singleShot(10000, this, [this](){
        ui->active_checkBox->setEnabled(true);
    });
}

