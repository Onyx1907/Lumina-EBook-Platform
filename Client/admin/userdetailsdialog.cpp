#include "userdetailsdialog.h"
#include "ui_userdetailsdialog.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>
#include <QJsonArray>

UserDetailsDialog::UserDetailsDialog(const QJsonObject &user, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::UserDetailsDialog)
{
    ui->setupUi(this);

    QString role = user.value("role").toString();

    ui->id_label->setText(
        "آیدی کاربر: " +
        QString::number(user.value("id").toInt()));

    ui->username_label->setText(
        "نام کاربری: " +
        user.value("username").toString());

    if(role == "Publisher")
        ui->name_label->setText(
            "نام انتشارات: " +
            user.value("name").toString());
    else
        ui->name_label->setText(
            "نام و نام خانوادگی: " +
            user.value("name").toString());

    ui->email_label->setText(
        "ایمیل: " +
        user.value("email").toString());

    QString persianRole;

    if(role == "Admin")
        persianRole = "ادمین";
    else if(role == "Publisher")
        persianRole = "ناشر";
    else
        persianRole = "کاربر عادی";

    ui->role_label->setText("نقش: " + persianRole);

    ui->state_label->setText(
        user.value("is_blocked").toInt()
            ? "وضعیت: مسدود"
            : "وضعیت: فعال");

    QDateTime date =
        QDateTime::fromString(
            user.value("registration_date").toString(),
            Qt::ISODate);

    ui->date_label->setText(
        "زمان ثبت‌نام: " +
        date.toString("yyyy/MM/dd - HH:mm:ss"));

    if(role == "Publisher")
    {
        ui->genres_label->hide();

        ui->books_label->setText(
            "تعداد کتاب‌های منتشرشده: " +
            QString::number(user.value("published_books_count").toInt()));
    }
    else
    {
        QJsonArray genresArray =
            QJsonDocument::fromJson(
                user.value("favorite_genres").toString().toUtf8()).array();

        QStringList genres;

        for(const QJsonValue &value : genresArray)
        {
            QString genre = value.toString();

            if(genre == "Fiction")
                genres << "ادبیات داستانی";
            else if(genre == "SciFi")
                genres << "علمی تخیلی";
            else if(genre == "Psychology")
                genres << "روانشناسی";
            else if(genre == "History")
                genres << "تاریخی";
            else if(genre == "Educational")
                genres << "علمی و تحصیلی";
            else if(genre == "Biography")
                genres << "زندگینامه";
        }

        ui->genres_label->setText(
            "ژانرهای مورد علاقه: " + genres.join("، "));

        ui->books_label->setText(
            "تعداد خریدها: " +
            QString::number(user.value("total_purchases").toInt()));
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

