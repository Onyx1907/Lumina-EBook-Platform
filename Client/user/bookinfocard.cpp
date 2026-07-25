#include "bookinfocard.h"
#include "ui_bookinfocard.h"
#include "storageutils.h"

BookInfoCard::BookInfoCard(Book b, QWidget *parent)
    : QWidget(parent), book(b)
    , ui(new Ui::BookInfoCard)
{
    ui->setupUi(this);
    ui->author_label->setText(book.getAuthor());
    ui->name_label->setText(book.getTitle());
    ui->publisher_label->setText(book.getPublisher());

    ui->tooman_label->show();

    StorageUtils::displayBookCover(book.getCoverImagePath(), ui->book_cover);

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

    book_pushbutton = new QPushButton(this);

    book_pushbutton->setStyleSheet(
        /* ۱. حالت عادی: دکمه کاملاً شفاف با گوشه‌های گرد */
        "QPushButton {"
        "   background-color: transparent;"
        "   border: 2px solid transparent;"
        "   border-radius: 10px;"
        "}"

        /* ۲. حالت Hover: هاله طلایی ملایم و لوکس دور کارت */
        "QPushButton:hover {"
        "   background-color: rgba(218, 165, 32, 0.07);" /* ۷ درصد طلایی متالیک (Goldenrod) روی عکس کتاب */
        "   border: 2px solid rgba(184, 134, 11, 0.6);" /* حاشیه طلایی تیره نیمه‌شفاف دور کارت */
        "}"

        /* ۳. حالت Pressed: وقتی کاربر کلیک می‌کند (کمی تیره و فشرده‌تر) */
        "QPushButton:pressed {"
        "   background-color: rgba(139, 69, 19, 0.15);" /* هاله قهوه‌ای شکلاتی ملایم برای حس کلیک */
        "   border: 2px solid #B8860B;" /* حاشیه طلایی صلب و مشخص */
        "}"
        );

    connect(book_pushbutton, &QPushButton::clicked, this, [this](){
        emit clicked(&book);
    });
}

void BookInfoCard::resizeEvent(QResizeEvent *event){
    QWidget::resizeEvent(event);

    if(book_pushbutton){
        book_pushbutton->setGeometry(0, 0, this->width(), this->height());
        book_pushbutton->raise();

        ui->delete_pushButton->raise();
    }
}

void BookInfoCard::setCardMode(CardMode mode){
    ui->delete_pushButton->hide();

    if(mode == CardMode::Cart){
        ui->delete_pushButton->show();
    }

    ui->delete_pushButton->raise();
}

BookInfoCard::~BookInfoCard()
{
    delete ui;
}

void BookInfoCard::on_delete_pushButton_clicked()
{
    emit removeRequested(book.getId());
}


void BookInfoCard::updatePrice(double price, double discount){
    book.setPrice(price, discount);

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

}

int BookInfoCard::getBookID(){
    return book.getId();
}
