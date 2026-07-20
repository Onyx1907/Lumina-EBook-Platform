#include "publisherbookwidget.h"
#include "ui_publisherbookwidget.h"

PublisherBookWidget::PublisherBookWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PublisherBookWidget)
{
    ui->setupUi(this);
}

PublisherBookWidget::~PublisherBookWidget()
{
    delete ui;
}
