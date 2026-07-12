#include "bookinfocard.h"
#include "ui_bookinfocard.h"

BookInfoCard::BookInfoCard(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BookInfoCard)
{
    ui->setupUi(this);
}

BookInfoCard::~BookInfoCard()
{
    delete ui;
}
