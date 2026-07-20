#include "editbookwidget.h"
#include "ui_editbookwidget.h"

EditBookWidget::EditBookWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EditBookWidget)
{
    ui->setupUi(this);
}

EditBookWidget::~EditBookWidget()
{
    delete ui;
}
