#include "forgotpasswordwidget.h"
#include "ui_forgotpasswordwidget.h"
#include "clientnetworkmanager.h"
#include <QTimer>

ForgotPasswordWidget::ForgotPasswordWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ForgotPasswordWidget)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_TranslucentBackground);
    enableFormWithError("", true);
    ui->internalStackedWidget->setCurrentIndex(internalPage::VerificationStage);


    connect(&ClientNetworkManager::instance(), &ClientNetworkManager::responseReceived,
            this, &ForgotPasswordWidget::processNetworkData);
}

ForgotPasswordWidget::~ForgotPasswordWidget()
{
    delete ui;
}

void ForgotPasswordWidget::on_back_pushButton_clicked()
{
    ui->answer_input->clear();
    ui->error_label->clear();
    ui->error_label_2->clear();
    ui->newPass_again_input->clear();
    ui->newPass_input->clear();
    ui->username_input->clear();
    ui->internalStackedWidget->setCurrentIndex(internalPage::VerificationStage);

    emit goToLoginRequested();
}


void ForgotPasswordWidget::on_back_pushButton_2_clicked()
{
    ui->answer_input->clear();
    ui->error_label->clear();
    ui->error_label_2->clear();
    ui->newPass_again_input->clear();
    ui->newPass_input->clear();
    ui->username_input->clear();
    ui->internalStackedWidget->setCurrentIndex(internalPage::VerificationStage);

    emit goToLoginRequested();
}


void ForgotPasswordWidget::on_check_user_pushButton_clicked()
{
    QString username = ui->username_input->text().trimmed();


    if(username.isEmpty()){
        ui->error_label->setText("لطفا نام کاربری را وارد کنید");
        return;
    }

    if(ClientNetworkManager::instance().connectToServer()){
        currentUsername = username;

        enableFormWithError("در حال بررسی", false);

        QJsonObject data;
        data["step"] = "REQUEST_QUESTION";
        data["username"] = username;

        ClientNetworkManager::instance().sendRequest("FORGOT_PASSWORD", data);
    }
    else{
        enableFormWithError("خطا در برقرار اتصال", true);
    }
}


void ForgotPasswordWidget::on_change_pass_pushButton_clicked()
{
    QString answer = ui->answer_input->text().trimmed();
    QString newPass = ui->newPass_input->text();
    QString confirmPass = ui->newPass_again_input->text();

    if(answer.isEmpty() || newPass.isEmpty() || confirmPass.isEmpty()){
        ui->error_label_2->setText("لطفا تمام فیلدها را پر کنید");
        return;
    }

    if(newPass != confirmPass){
        ui->error_label_2->setText("رمز عبور با تکرار آن مطابقت ندارد");
        return;
    }

    if(newPass.length() < 6){
        ui->error_label_2->setText("رمز عبور باید بیشتر از ۶ کارکتر باشد");
        return;
    }

    if(ClientNetworkManager::instance().connectToServer()){
        enableFormWithError("در حال تغییر رمز", false);

        QJsonObject data;
        data["step"] = "ANSWER_AND_RESET";
        data["username"] = currentUsername;
        data["security_answer"] = answer;
        data["new_password"] = newPass;


        ClientNetworkManager::instance().sendRequest("FORGOT_PASSWORD", data);
    }
    else{
        enableFormWithError("خطا در برقرار اتصال", true);
    }
}


void ForgotPasswordWidget::processNetworkData(const QString& action, const QJsonObject& data){
    if(action != "FORGOT_PASSWORD_RESPONSE") return;

    QString status = data.value("status").toString();

    if(data.contains("security_question")){
        if(status == "SUCCESS"){
            QString question = data.value("security_question").toString();

            ui->internalStackedWidget->setCurrentIndex(internalPage::ResetPasswordStage);
            enableFormWithError("", true);
            ui->seurityQ_label->setText(question);
        }
        else{
            QString message = data.value("message").toString();
            enableFormWithError(message, true);
        }
    }
    else{
        QString message = data.value("message").toString();
        if(status == "SUCCESS"){
            enableFormWithError("رمز عبور با موفقیت تغییر کرد", true);
            QTimer::singleShot(4000, this, [this](){
                on_back_pushButton_clicked();
            });
        }
        else{
            enableFormWithError(message, true);
        }
    }
}


void ForgotPasswordWidget::enableFormWithError(const QString& errorMsg, bool b){
    ui->answer_input->setEnabled(b);
    ui->back_pushButton->setEnabled(b);
    ui->back_pushButton_2->setEnabled(b);
    ui->change_pass_pushButton->setEnabled(b);
    ui->check_user_pushButton->setEnabled(b);
    ui->newPass_again_input->setEnabled(b);
    ui->newPass_input->setEnabled(b);
    ui->username_input->setEnabled(b);
    ui->error_label->setText(errorMsg);
    ui->error_label_2->setText(errorMsg);
}

