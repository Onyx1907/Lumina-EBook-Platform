#include "genreselectionwidget.h"
#include "ui_genreselectionwidget.h"
#include "clientnetworkmanager.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QAbstractButton>
#include <QTimer>


GenreSelectionWidget::GenreSelectionWidget(QString cur_username, QWidget *parent)
    : QWidget(parent), username(cur_username)
    , ui(new Ui::GenreSelectionWidget)
{
    ui->setupUi(this);

    ui->check_pushButton->setEnabled(false);

    connect(ui->genres_buttonGroup, &QButtonGroup::buttonClicked, this,
            [=](QAbstractButton *clickedButton){
        int checkedCount = 0;
        for(QAbstractButton* btn : ui->genres_buttonGroup->buttons()){
            if(btn->isChecked()){
                checkedCount++;
            }
        }

        if(checkedCount > 3){
            clickedButton->setChecked(false);
            return;
        }

        ui->check_pushButton->setEnabled(checkedCount >= 1 && checkedCount <= 3);
    });

    connect(&ClientNetworkManager::instance(), &ClientNetworkManager::responseReceived,
            this, &GenreSelectionWidget::processNetworkData);
}


void GenreSelectionWidget::on_check_pushButton_clicked()
{
    ui->check_pushButton->setEnabled(false);
    for(QAbstractButton *button : ui->genres_buttonGroup->buttons()){
        button->setEnabled(false);
    }

    if(ClientNetworkManager::instance().connectToServer()){
        QJsonArray genresArray;
        for (QAbstractButton *btn: ui->genres_buttonGroup->buttons()){
            if(btn->isChecked()){
                if(btn->objectName() == "biography_pushButton"){
                    genresArray.append("Biography");
                } else if(btn->objectName() == "educational_pushButton"){
                    genresArray.append("Educational");
                } else if(btn->objectName() == "psychology_pushButton"){
                    genresArray.append("Psychology");
                } else if(btn->objectName() == "ficition_pushButton"){
                    genresArray.append("Fiction");
                } else if(btn->objectName() == "history_pushButton"){
                    genresArray.append("History");
                } else if(btn->objectName() == "scifi_pushButton"){
                    genresArray.append("SciFi");
                }
            }
        }

        QJsonObject data;

        data["username"] = username;
        data["genres"] = genresArray;

        ClientNetworkManager::instance().sendRequest("SER_FAVORITE_GENRES", data);
    }
    else{
        ui->error_label->setText("خطا در برقراری اتصال");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
        });

        ui->check_pushButton->setEnabled(true);
        for(QAbstractButton *button : ui->genres_buttonGroup->buttons()){
            button->setEnabled(true);
        }
    }

}


void GenreSelectionWidget::processNetworkData(const QString& action, const QJsonObject& data){
    if(action != "SER_FAVORITE_GENRES_RESPONSE"){
        return;
    }

    QString status = data.value("status").toString();
    QString message = data.value("message").toString();

    if(status == "SUCCESS"){
        ui->error_label->setText("");
        ui->success_label->setText(message);
        QTimer::singleShot(3000, this, [this](){
            ui->success_label->setText("");
            emit goToHomePage();
        });

        //رفتن به صفحه اصلی برنامه
    }
    else if(status == "ERROR"){
        ui->success_label->setText("");
        ui->error_label->setText(message);
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
        });
    }

    ui->check_pushButton->setEnabled(true);
    for(QAbstractButton *button : ui->genres_buttonGroup->buttons()){
        button->setEnabled(true);
    }
}



GenreSelectionWidget::~GenreSelectionWidget()
{
    delete ui;
}
