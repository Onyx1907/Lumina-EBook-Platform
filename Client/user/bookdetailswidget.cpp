#include "bookdetailswidget.h"
#include "ui_bookdetailswidget.h"
#include "constants.h"
#include <QString>

BookDetailsWidget::BookDetailsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BookDetailsWidget)
{
    ui->setupUi(this);
}

void BookDetailsWidget::loadBook(Book *book){

    ui->name_label->setText(book->getTitle());
    ui->author_label->setText(book->getAuthor());
    ui->publisher_label->setText(book->getPublisher());
    ui->genre_label->setText(genreToString(book->getGenre()));
}

BookDetailsWidget::~BookDetailsWidget()
{
    delete ui;
}



void BookDetailsWidget::on_back_pushButton_clicked()
{
    emit backPrevious();
}

