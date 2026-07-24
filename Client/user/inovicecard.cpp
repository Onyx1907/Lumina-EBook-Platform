#include "inovicecard.h"
#include "ui_inovicecard.h"
#include <QDateTime>

InoviceCard::InoviceCard(int bookID, QString date, QString title,
                                     QString author, int price,QWidget *parent)
    : QWidget(parent), m_bookID(bookID)
    , ui(new Ui::InoviceCard)
{
    ui->setupUi(this);

    ui->title_label->setText(title);
    ui->author_label->setText(author);
    ui->price_label->setText(QString::number(price));

    QDateTime dateTime = QDateTime::fromString(date, Qt::ISODate);
    ui->date_label->setText(dateTime.toString("yyyy-MM-DD - HH:mm:ss"));
}

InoviceCard::~InoviceCard()
{
    delete ui;
}
