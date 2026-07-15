#include "resultwidget.h"
#include "ui_resultwidget.h"
#include "bookinfocard.h"
#include <QAbstractItemView>
#include <QScrollBar>

ResultWidget::ResultWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ResultWidget)
{
    ui->setupUi(this);

    ui->listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->listWidget->verticalScrollBar()->setSingleStep(15);
}

ResultWidget::~ResultWidget()
{
    delete ui;
}


void ResultWidget::fillResults(const QVector<Book>& booksList){
    ui->listWidget->clear();

    for(const Book& book : booksList){
        QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
        BookInfoCard *card = new BookInfoCard(book, this);

        card->setCardMode();

        item->setSizeHint(card->sizeHint());
        ui->listWidget->setItemWidget(item, card);

        connect(card, &BookInfoCard::clicked, this, [this](Book* clickedBookptr){
           emit bookSelected(clickedBookptr);
        });
    }
}


void ResultWidget::on_back_pushButton_clicked()
{
    emit backPrevious();
}

