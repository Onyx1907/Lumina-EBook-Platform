#include "commentswidget.h"
#include "ui_commentswidget.h"

CommentsWidget::CommentsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CommentsWidget)
{
    ui->setupUi(this);
}

CommentsWidget::~CommentsWidget()
{
    delete ui;
}
