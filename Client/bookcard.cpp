#include "bookcard.h"
#include "ui_bookcard.h"
#include <QPixmap>

BookCard::BookCard(Book b, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BookCard), book(b)
{
    ui->setupUi(this);

    ui->title_label->setText(this->book.getTitle());

    QPixmap pix;

    if(!(this->book.getCoverImagePath().isEmpty())){
        QByteArray byteArray =
            QByteArray::fromBase64(this->book.getCoverImagePath().toUtf8());

        pix.loadFromData(byteArray);

        qDebug() << "شرط اولی";
    }


    if(!pix.isNull()){
        QSize targetSize(110, 130);

        QPixmap scaledPix = pix.scaled(
            targetSize,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            );
        QIcon icon(scaledPix);
        ui->book_cover->setIcon(icon);
        ui->book_cover->setIconSize(targetSize);

        qDebug() << "شرط دومی";
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
    //اتصال کتاب به صفحه جزییات کتاب

    connect(book_pushbutton, &QPushButton::clicked, this, [this](){
        emit clicked(&book);
    });
}


void BookCard::resizeEvent(QResizeEvent *event){
    QWidget::resizeEvent(event);

    if(book_pushbutton){
        book_pushbutton->setGeometry(0, 0, this->width(), this->height());
        book_pushbutton->raise();
    }
}


BookCard::~BookCard()
{
    delete ui;
}
