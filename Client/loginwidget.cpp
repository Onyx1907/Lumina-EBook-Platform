#include "loginwidget.h"
#include "ui_loginwidget.h"
#include <QMessageBox>
#include <QPushButton>
#include <QJsonObject>
#include "clientnetworkmanager.h"

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

    // ۲. متصل کردن کلیک دکمه‌ها به اسلات‌های داخلی همین کلاس
}

LoginWidget::~LoginWidget()
{
    delete ui; // مدیریت حافظه: حذف خودکار لایه گرافیکی اختصاص داده شده
}

void LoginWidget::on_register_pushButton_clicked()
{
    emit goToRegisterRequested();
}


void LoginWidget::on_forget_pushButton_clicked()
{
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

    if(password.length() < 6) {
        ui->error_label->setText("رمز عبور نمی‌تواند کمتر از ۶ کارکتر باشد");
        return;
    }

    ui->error_label->setText("");

    QJsonObject loginData;
    loginData["username"] = username;
    loginData["password"] = password;

    ClientNetworkManager::instance().sendRequest("LOGIN", loginData);

    ui->login_pushButton->setEnabled(false);
    ui->username_input->setEnabled(false);
    ui->password_input->setEnabled(false);
    ui->register_pushButton->setEnabled(false);
    ui->forget_pushButton->setEnabled(false);
    ui->error_label->setText("در حال برقراری ارتباط با سرور");
}



void LoginWidget::enableFormWithError(const QString& errorMsg) {
    ui->username_input->setEnabled(true);
    ui->password_input->setEnabled(true);
    ui->login_pushButton->setEnabled(true);
    ui->register_pushButton->setEnabled(true);
    ui->forget_pushButton->setEnabled(true);
    ui->error_label->setText(errorMsg); // ارور شبکه یا سرور رو اینجا چاپ میکنه
}
