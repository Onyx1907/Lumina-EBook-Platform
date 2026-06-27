#include "loginwidget.h"
#include "ui_loginwidget.h"
#include <QMessageBox>
#include <QPushButton>

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
    QWidget *parent = this->parentWidget();
    if(parent){
        QStackedWidget *stacked = qobject_cast<QStackedWidget*>(parent);
        if(stacked){
            stacked->setCurrentIndex(MainWindow::Page::RegisterPageIndex);
        }
    }
}


void LoginWidget::on_forget_pushButton_clicked()
{
    QWidget *parent = this->parentWidget();
    if(parent){
        QStackedWidget *stacked = qobject_cast<QStackedWidget*>(parent);
        if(stacked){
            stacked->setCurrentIndex(MainWindow::Page::ForgotPasswordPageIndex);
        }
    }
}

