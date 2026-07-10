#include "bookdetailswidget.h"
#include "ui_bookdetailswidget.h"

BookDetailsWidget::BookDetailsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BookDetailsWidget)
{
    ui->setupUi(this);
}

void BookDetailsWidget::loadBook(Book *book){

}

BookDetailsWidget::~BookDetailsWidget()
{
    delete ui;
}



void BookDetailsWidget::on_back_pushButton_clicked()
{
    emit backPrevious();
}

