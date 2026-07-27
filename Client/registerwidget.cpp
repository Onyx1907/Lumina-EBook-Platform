#include "registerwidget.h"
#include "ui_registerwidget.h"
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>
#include "user.h"

RegisterWidget::RegisterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegisterWidget)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground);


    connect(&ClientNetworkManager::instance(), &ClientNetworkManager::responseReceived,
            this, &RegisterWidget::processNetworkData);
}

RegisterWidget::~RegisterWidget()
{
    delete ui;
}

void RegisterWidget::on_back_pushButton_clicked()
{
    ui->answer_input->clear();
    ui->error_label->clear();
    ui->success_label->clear();
    ui->pass_input->clear();
    ui->pass2_input->clear();
    ui->role_comboBox->setCurrentIndex(0);
    ui->security_input->clear();
    ui->username_input->clear();

    emit goToLoginRequested();
}


void RegisterWidget::on_register_pushButton_clicked()
{
    QString username = ui->username_input->text().trimmed();
    QString password = ui->pass_input->text();
    QString confirm_password = ui->pass2_input->text();
    QString security_question = ui->security_input->text().trimmed();
    QString security_answer = ui->answer_input->text().trimmed();
    QString role;
    if(ui->role_comboBox->currentIndex() == 0){
        role = "User";
    }
    else if(ui->role_comboBox->currentIndex() == 1){
        role = "Publisher";
    }

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

    if(username.length() < 5){
        ui->error_label->setText("نام کاربری نمی‌تواند کمتر از ۵ کارکتر باشد");
        return;
    }

    ui->error_label->setText("در حال اتصال به سرور");

    if(ClientNetworkManager::instance().connectToServer()) {
        enableFormWithError("در حال ثبت نام", false);

        QJsonObject regData;
        regData["username"] = username;
        regData["password"] = password;
        regData["role"] = role;
        regData["security_question"] = security_question;
        regData["security_answer"] = security_answer;

        ClientNetworkManager::instance().sendRequest("REGISTER", regData);
    }
    else{
        enableFormWithError("عدم برقراری ارتباط با سرور", true);
    }
}


void RegisterWidget::processNetworkData(const QString& action, const QJsonObject& data){
    if(action != "REGISTER_RESPONSE"){
        return;
    }


    QString status = data.value("status").toString();
    QString message = data.value("message").toString();

    if(status == "SUCCESS") {
        enableFormWithError("", true);
        ui->success_label->setText("ثبت نام با موفقیت انجام شد... شما می‌توانید وارد شوید");;

        QTimer::singleShot(3000, this, [this](){
            on_back_pushButton_clicked();
        });
    }
    else if(status == "FAILED"){
        enableFormWithError(message, true);
    }
}


void RegisterWidget::enableFormWithError(const QString& errorMsg, bool b){
    ui->answer_input->setEnabled(b);
    ui->back_pushButton->setEnabled(b);
    ui->pass2_input->setEnabled(b);
    ui->pass_input->setEnabled(b);
    ui->register_pushButton->setEnabled(b);
    ui->role_comboBox->setEnabled(b);
    ui->security_input->setEnabled(b);
    ui->username_input->setEnabled(b);

    ui->error_label->setText(errorMsg);
}
