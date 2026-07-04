#include "bookcard.h"
#include "ui_bookcard.h"

BookCard::BookCard(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BookCard)
{
    ui->setupUi(this);
}

BookCard::~BookCard()
{
    delete ui;
}
