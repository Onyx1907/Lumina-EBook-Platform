#include "adminbookswidget.h"
#include "ui_adminbookswidget.h"

AdminBooksWidget::AdminBooksWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdminBooksWidget)
{
    ui->setupUi(this);
}

AdminBooksWidget::~AdminBooksWidget()
{
    delete ui;
}
