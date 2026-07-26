#include "userdetailsdialog.h"
#include "ui_userdetailsdialog.h"
#include <QJsonObject>

UserDetailsDialog::UserDetailsDialog(const QJsonObject &user, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::UserDetailsDialog)
{
    ui->setupUi(this);

    QString role = user.value("role").toString();

    // آیدی
    ui->id_label->setText(
        "آیدی کاربر: " +
        QString::number(user.value("id").toInt()));

    // نام کاربری
    ui->username_label->setText(
        "نام کاربری: " +
        user.value("username").toString());

    // نام / نام انتشارات
    if(role == "Publisher")
        ui->name_label->setText(
            "نام انتشارات: " +
            user.value("name").toString());
    else
        ui->name_label->setText(
            "نام و نام خانوادگی: " +
            user.value("name").toString());

    // ایمیل
    ui->email_label->setText(
        "ایمیل: " +
        user.value("email").toString());

    // نقش
    QString persianRole;

    if(role == "Admin")
        persianRole = "ادمین";
    else if(role == "Publisher")
        persianRole = "ناشر";
    else
        persianRole = "کاربر عادی";

    ui->role_label->setText("نقش: " + persianRole);

    // وضعیت
    ui->state_label->setText(
        user.value("is_blocked").toInt()
            ? "وضعیت: مسدود"
            : "وضعیت: فعال");

    // تاریخ ثبت نام
    QDateTime date =
        QDateTime::fromString(
            user.value("registration_date").toString(),
            "yyyy-MM-dd HH:mm:ss");

    ui->date_label->setText(
        "تاریخ ثبت‌نام: " +
        date.toString("yyyy/MM/dd - HH:mm"));

    // اطلاعات مخصوص ناشر / کاربر عادی
    if(role == "Publisher")
    {
        ui->genres_label->hide();

        ui->books_label->setText(
            "تعداد کتاب‌های منتشرشده: " +
            QString::number(
                user.value("published_books_count").toInt()));
    }
    else
    {
        QStringList genres =
            user.value("favorite_genres")
                .toString()
                .split(',');

        for(QString &genre : genres)
        {
            genre = genre.trimmed();

            if(genre == "Fiction")
                genre = "داستانی";
            else if(genre == "SciFi")
                genre = "علمی و تخیلی";
            else if(genre == "Psychology")
                genre = "روان‌شناسی";
            else if(genre == "History")
                genre = "تاریخی";
            else if(genre == "Educational")
                genre = "علمی و تحصیلی";
            else if(genre == "Biography")
                genre = "زندگینامه";
            else
                genre = "نامشخص";
        }

        ui->genres_label->setText(
            "ژانرهای مورد علاقه: " +
            genres.join("، "));

        ui->books_label->setText(
            "تعداد خریدها: " +
            QString::number(
                user.value("total_purchases").toInt()));
    }
}

UserDetailsDialog::~UserDetailsDialog()
{
    delete ui;
}

void UserDetailsDialog::on_back_pushButton_clicked()
{
    accept();
}

