#include "profilewidget.h"
#include "ui_profilewidget.h"
#include "clientnetworkmanager.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QStringList>
#include <QJsonValue>
#include <QTimer>


ProfileWidget::ProfileWidget(RegularUser *cur_user, QWidget *parent)
    : QWidget(parent), user(cur_user)
    , ui(new Ui::ProfileWidget)
{
    ui->setupUi(this);


    connect(&ClientNetworkManager::instance(), &ClientNetworkManager::responseReceived,
            this, &ProfileWidget::processNetworkData);
}

void ProfileWidget::loadProfile(){
    ui->confirmPass_lineEdit->setText("");
    ui->email_lineEdit->setText("");
    ui->name_lineEdit->setText("");
    ui->newPass_lineEdit->setText("");
    ui->pass_lineEdit->setText("");
    ui->username_lineEdit->setText("");

    ui->username_label->setText(user->getUsername());

    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["user_id"] = user->getId();

        ClientNetworkManager::instance().sendRequest("GET_PROFILE", data);
    }
    else{
        ui->error_label->setText("خطا در برقراری اتصال");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
        });
    }
}


void ProfileWidget::on_changeGenres_pushButton_clicked()
{
    emit goToGenreSelectionPage();
}


void ProfileWidget::processNetworkData(const QString& action, const QJsonObject& data)
{
    if (action != "GET_PROFILE_RESPONSE" &&
        action != "UPDATE_PROFILE_RESPONSE" &&
        action != "CHANGE_PASSWORD_RESPONSE") {
        return;
    }

    if (action == "GET_PROFILE_RESPONSE") {
        if (data.value("status").toString() == "SUCCESS") {

            QJsonObject profileObj = data.value("profile").toObject();

            qDebug() << profileObj;

            ui->name_label->setText(profileObj.value("name").toString());
            ui->email_label->setText(profileObj.value("email").toString());

            QJsonArray genresArray = profileObj.value("favorite_genres").toArray();
            QStringList genres;
            genres.reserve(genresArray.size());

            for (const QJsonValue &v : genresArray) {
                QString sv = v.toString();

                if (sv == "Biography")
                    genres << "زندگینامه";
                else if (sv == "Educational")
                    genres << "علمی و تحصیلی";
                else if (sv == "Psychology")
                    genres << "روانشناسی";
                else if (sv == "Fiction")
                    genres << "ادبیات داستانی";
                else if (sv == "History")
                    genres << "تاریخی";
                else if (sv == "SciFi")
                    genres << "علمی تخیلی";
                else
                    genres << sv;
            }

            const QString genresText = genres.isEmpty() ? "_" : genres.join("، ");
            ui->favoriteGenres_label->setText(genresText);
        }

        else if(data.value("status").toString() == "ERROR"){
            ui->error_label->setText(data.value("message").toString());
            QTimer::singleShot(3000, this, [this](){
                ui->error_label->setText("");
            });
        }
    }

    if (action == "UPDATE_PROFILE_RESPONSE"){
        ui->submitProfile_pushButton->setEnabled(true);

        if (data.value("status").toString() == "SUCCESS") {
            ui->prof_error_label->setText("");
            ui->prof_success_label->setText(data.value("message").toString());
            if(!new_username.isEmpty()){
                user->setUsername(new_username);
            }
            loadProfile();
            QTimer::singleShot(3000, this, [this](){
                ui->prof_success_label->setText("");
            });
        }

        else if(data.value("status").toString() == "FAILED"){
            ui->prof_success_label->setText("");
            ui->prof_error_label->setText(data.value("message").toString());
            QTimer::singleShot(3000, this, [this](){
                ui->prof_error_label->setText("");
            });
        }

        ui->email_lineEdit->setText("");
        ui->name_lineEdit->setText("");
        ui->username_lineEdit->setText("");
    }

    if (action == "CHANGE_PASSWORD_RESPONSE"){
        ui->submitPass_pushButton->setEnabled(true);

        if (data.value("status").toString() == "SUCCESS") {
            ui->pass_error_label->setText("");
            ui->pass_success_label->setText(data.value("message").toString());
            QTimer::singleShot(3000, this, [this](){
                ui->pass_success_label->setText("");
            });
        }

        else if(data.value("status").toString() == "ERROR"){
            ui->pass_success_label->setText("");
            ui->pass_error_label->setText(data.value("message").toString());
            QTimer::singleShot(3000, this, [this](){
                ui->pass_error_label->setText("");
            });
        }

        ui->confirmPass_lineEdit->setText("");
        ui->newPass_lineEdit->setText("");
        ui->pass_lineEdit->setText("");
    }
}


ProfileWidget::~ProfileWidget()
{
    delete ui;
}



void ProfileWidget::on_submitProfile_pushButton_clicked()
{
    new_username = ui->username_lineEdit->text().trimmed();
    QString email = ui->email_lineEdit->text().trimmed();
    QString name = ui->name_lineEdit->text().trimmed();

    ui->prof_success_label->setText("");

    if(new_username.length() > 0 && new_username.length() < 5){
        ui->prof_error_label->setText("نام کاربری نمی‌تواند کمتر از ۵ کارکتر باشد");
        QTimer::singleShot(3000, this, [this](){
            ui->prof_error_label->setText("");
        });
        return;
    }

    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["user_id"] = user->getId();
        data["username"] = new_username;
        data["name"] = name;
        data["email"] = email;

        qDebug() << data;

        ClientNetworkManager::instance().sendRequest("UPDATE_PROFILE", data);
        ui->submitProfile_pushButton->setEnabled(false);
    }
    else{
        ui->prof_error_label->setText("خطا در برقراری اتصال");
        QTimer::singleShot(3000, this, [this](){
            ui->prof_error_label->setText("");
        });
    }
}


void ProfileWidget::on_submitPass_pushButton_clicked()
{
    QString cur_pass = ui->pass_lineEdit->text();
    QString new_pass = ui->newPass_lineEdit->text();
    QString confirm_pass = ui->confirmPass_lineEdit->text();

    ui->pass_success_label->setText("");

    if(new_pass.length() < 6){
        ui->pass_error_label->setText("رمز عبور نمی‌تواند کمتر از ۶ کارکتر باشد");
        QTimer::singleShot(3000, this, [this](){
            ui->pass_error_label->setText("");
        });
        return;
    }
    if(new_pass != confirm_pass){
        ui->pass_error_label->setText("رمز عبور با تکرار آن مطابقت ندارد");
        QTimer::singleShot(3000, this, [this](){
            ui->pass_error_label->setText("");
        });
        return;
    }

    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["user_id"] = user->getId();
        data["old_password"] = cur_pass;
        data["new_password"] = new_pass;

        ClientNetworkManager::instance().sendRequest("CHANGE_PASSWORD", data);
        ui->submitPass_pushButton->setEnabled(false);
    }
    else{
        ui->pass_error_label->setText("خطا در برقراری اتصال");
        QTimer::singleShot(3000, this, [this](){
            ui->pass_error_label->setText("");
        });
    }

}

