#include "registerwidget.h"
#include "ui_registerwidget.h"
#include <QMessageBox>
#include <QPushButton>
#include "user.h"

RegisterWidget::RegisterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegisterWidget)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground);
}

RegisterWidget::~RegisterWidget()
{
    delete ui;
}

void RegisterWidget::on_back_pushButton_clicked()
{
    emit goToLoginRequested();
}


void RegisterWidget::on_register_pushButton_clicked()
{
    QString username = ui->username_input->text().trimmed();
    QString password = ui->pass_input->text();
    QString confirm_password = ui->pass2_input->text();
    QString security_question = ui->security_input->text().trimmed();
    QString security_answer = ui->answer_input->text().trimmed();
    QString role = ui->role_comboBox->currentText();

    if(username.isEmpty() || password.isEmpty() || confirm_password.isEmpty() ||
        security_question.isEmpty() || security_answer.isEmpty()) {
        ui->error_label->setText("لطفا همه فیلدها را پر کنید");
        return;
    }

    if(password != confirm_password) {
        ui->error_label->setText("رمز عبور و تکرار آن مطابقت ندارد");
        return;
    }

    if(password.length() < 6) {
        ui->error_label->setText("رمز عبور نمی‌تواند کمتر از ۶ کارکتر باشد");
        return;
    }

    ui->error_label->setText("");


}

