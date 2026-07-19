#include "publisherstatswidget.h"
#include "ui_publisherstatswidget.h"

PublisherStatsWidget::PublisherStatsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PublisherStatsWidget)
{
    ui->setupUi(this);
}

PublisherStatsWidget::~PublisherStatsWidget()
{
    delete ui;
}
