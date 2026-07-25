#include "loginwidget.h"
#include "ui_loginwidget.h"
#include <QMessageBox>
#include <QPushButton>
#include <QJsonObject>
#include "clientnetworkmanager.h"
#include <QTimer>
#include "publisher.h"

LoginWidget::LoginWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWidget)
{
    ui->setupUi(this);

    QPixmap logoPixmap(":/resources/Lumina_Logo.png");
    QPixmap scaledLogo = logoPixmap.scaled(200, 200, Qt::KeepAspectRatio,
                                           Qt::SmoothTransformation);

    ui->logo_label->setPixmap(scaledLogo);

    ui->logo_label->setFixedSize(120, 120);
    ui->logo_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    // ۱. شیشه‌ای و شفاف کردن پس‌زمینه این صفحه برای دیدن ویدیوی MainWindow
    setAttribute(Qt::WA_TranslucentBackground);

    connect(&ClientNetworkManager::instance(), &ClientNetworkManager::responseReceived,
            this, &LoginWidget::processNetworkData);

}

LoginWidget::~LoginWidget()
{
    delete ui;
}

void LoginWidget::on_register_pushButton_clicked()
{
    ui->username_input->clear();
    ui->password_input->clear();
    ui->error_label->clear();

    emit goToRegisterRequested();
}


void LoginWidget::on_forget_pushButton_clicked()
{
    ui->username_input->clear();
    ui->password_input->clear();
    ui->error_label->clear();

    emit goToForgotPasswordRequested();
}


void LoginWidget::on_login_pushButton_clicked()
{
    QString username = ui->username_input->text().trimmed();
    QString password = ui->password_input->text();

    if(username.isEmpty() || password.isEmpty()) {
        ui->error_label->setText("لطفا نام کاربری و رمز عبور خود را وارد کنید");
        return;
    }

    if(password.length() < 5) {
        ui->error_label->setText("رمز عبور نمی‌تواند کمتر از ۵ کارکتر باشد");
        return;
    }


    ui->error_label->setText("در حال برقراری ارتباط با سرور");

    if(ClientNetworkManager::instance().connectToServer()){
        current_username = username;

        ui->login_pushButton->setEnabled(false);
        ui->username_input->setEnabled(false);
        ui->password_input->setEnabled(false);
        ui->register_pushButton->setEnabled(false);
        ui->forget_pushButton->setEnabled(false);

        QJsonObject loginData;
        loginData["username"] = username;
        loginData["password"] = password;

        ClientNetworkManager::instance().sendRequest("LOGIN", loginData);
    }
    else{
        enableFormWithError("عدم برقراری ارتباط... لطفا بعدا تلاش کنید");
    }
}


void LoginWidget::processNetworkData(const QString& action, const QJsonObject& data){
    if(action != "LOGIN_RESPONSE"){
        return;
    }

    QString status = data.value("status").toString();
    QString message = data.value("message").toString();

    if(status == "SUCCESS"){
        ui->success_label->setText("ورود با موفقیت انجام شد");

        QString role = data.value("user_role").toString();
        bool is_first_login = (data.value("first_login").toInt() == 1);
        int id = data.value("user_id").toInt();

        User *user;

        if(role == "RegularUser"){
            user = new RegularUser(id, current_username);
        }
        else if(role == "Publisher"){
            user = new Publisher(id, current_username, "");
        }

        QTimer::singleShot(3000, this, [this, user, is_first_login](){
            emit goToUSerDashboard(user, is_first_login);
        });

        ui->error_label->setText("");
    }
    else if(status == "FAILED"){
        enableFormWithError(message);
    }
}



void LoginWidget::enableFormWithError(const QString& errorMsg) {
    ui->username_input->setEnabled(true);
    ui->password_input->setEnabled(true);
    ui->login_pushButton->setEnabled(true);
    ui->register_pushButton->setEnabled(true);
    ui->forget_pushButton->setEnabled(true);
    ui->error_label->setText(errorMsg); // ارور شبکه یا سرور رو اینجا چاپ میکنه
}

