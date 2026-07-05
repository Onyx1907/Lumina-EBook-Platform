#include "bookcard.h"
#include "ui_bookcard.h"

BookCard::BookCard(Book book, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BookCard), book(book)
{
    ui->setupUi(this);
}


void BookCard::resizeEvent(QResizeEvent *event){
    QWidget::resizeEvent(event);


}

BookCard::~BookCard()
{
    delete ui;
}
