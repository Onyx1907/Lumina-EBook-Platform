#include "profilewidget.h"
#include "ui_profilewidget.h"
#include "clientnetworkmanager.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QStringList>
#include <QJsonValue>


ProfileWidget::ProfileWidget(RegularUser *cur_user, QWidget *parent)
    : QWidget(parent), user(cur_user)
    , ui(new Ui::ProfileWidget)
{
    ui->setupUi(this);


    connect(&ClientNetworkManager::instance(), &ClientNetworkManager::responseReceived,
            this, &ProfileWidget::processNetworkData);
}

void ProfileWidget::loadProfile(){
    ui->username_label->setText(user->getUsername());

    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["username"] = user->getUsername();

        ClientNetworkManager::instance().sendRequest("GET_PROFILE", data);
    }
    else{
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
    }
}


ProfileWidget::~ProfileWidget()
{
    delete ui;
}


