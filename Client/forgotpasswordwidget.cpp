#include "forgotpasswordwidget.h"
#include "ui_forgotpasswordwidget.h"

ForgotPasswordWidget::ForgotPasswordWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ForgotPasswordWidget)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_TranslucentBackground);

    ui->internalStackedWidget->setCurrentIndex(internalPage::VerificationStage);
    ui->answer_label->setEnabled(false);
    ui->answer_input->setEnabled(false);
    ui->check_answer_pushButton->setEnabled(false);
}

ForgotPasswordWidget::~ForgotPasswordWidget()
{
    delete ui;
}

void ForgotPasswordWidget::on_back_pushButton_clicked()
{
    emit goToLoginRequested();
}


void ForgotPasswordWidget::on_back_pushButton_2_clicked()
{
    emit goToLoginRequested();
}

