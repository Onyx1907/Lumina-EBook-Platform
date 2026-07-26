#include "alluserswidget.h"
#include "ui_alluserswidget.h"
#include "clientnetworkmanager.h"
#include <QTimer>
#include <QScrollBar>
#include "usercard.h"
#include <QJsonArray>
#include "datefilterdialog.h"

AllUsersWidget::AllUsersWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AllUsersWidget)
{
    ui->setupUi(this);

    ui->error_label->hide();

    ui->clear_toolButton->hide();

    ui->allUsers_pushButton->hide();

    ui->listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->listWidget->verticalScrollBar()->setSingleStep(15);

    connect(&ClientNetworkManager::instance(), &ClientNetworkManager::responseReceived,
            this, &AllUsersWidget::processNetworkData);
}

AllUsersWidget::~AllUsersWidget()
{
    delete ui;
}

void AllUsersWidget::loadUsers(){
    pendingScrollValue = ui->listWidget->verticalScrollBar()->value();

    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;

        ClientNetworkManager::instance().sendRequest("GET_ALL_USERS", data, true);
    }
    else{
        ui->error_label->show();
        ui->error_label->setText("خطا در برقراری اتصال");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
            ui->error_label->hide();
        });
    }
}

void AllUsersWidget::processNetworkData(const QString& action, const QJsonObject& data){
    qDebug() <<data;

    if(action != "GET_ALL_USERS_RESPONSE" &&
        action != "SEARCH_USERS_RESPONSE"){
        return;
    }

    if(data.value("message").toString() == "SUCCESS"){
        return;
    }

    if(action == "GET_ALL_USERS_RESPONSE"){
        fillResults(data);

        ui->allUsers_pushButton->hide();
    }
    else if(action == "SEARCH_USERS_RESPONSE"){
        fillResults(data);

        ui->allUsers_pushButton->show();
    }
}


void AllUsersWidget::fillResults(const QJsonObject& data){
    ui->listWidget->clear();

    QJsonArray usersArray = data["users"].toArray();

    for (const QJsonValue& value : usersArray) {
        QJsonObject userObj = value.toObject();

        UserCard* itemWidget = new UserCard(userObj);

        QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
        item->setSizeHint(itemWidget->sizeHint());
        ui->listWidget->setItemWidget(item, itemWidget);
    }

    QTimer::singleShot(0, this, [this]() {
        auto *bar = ui->listWidget->verticalScrollBar();

        bar->setValue(qMin(pendingScrollValue, bar->maximum()));
    });
}


void AllUsersWidget::on_date_toolButton_clicked()
{
    DateFilterDialog dialog(this);
    if(dialog.exec() == QDialog::Accepted){
        registerDate = dialog.selectedDate();

        ui->date_toolButton->setText(registerDate);
        ui->clear_toolButton->show();
    }
}


void AllUsersWidget::on_clear_toolButton_clicked()
{
    registerDate.clear();

    ui->date_toolButton->setText("همه تاریخ ها");

    ui->clear_toolButton->hide();
}


void AllUsersWidget::on_search_pushButton_clicked()
{
    QString role = "";
    int blocked = -1;

    switch (ui->blocked_comboBox->currentIndex()) {
    case 0:
        blocked = -1;
        break;
    case 1:
        blocked = 1;
        break;
    case 2:
        blocked = 0;
        break;
    }

    switch (ui->role_comboBox->currentIndex()) {
    case 0:
        role = "";
        break;
    case 1:
        role = "RegulerUser";
        break;
    case 2:
        role = "Publisher";
        break;
    }

    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;

        data["keyword"] = ui->keyword_lineEdit->text().trimmed();
        data["role"] = role;
        data["blocked"] = blocked;
        data["register_date"] = registerDate;

        ClientNetworkManager::instance().sendRequest("SEARCH_USERS", data, true);
    }
    else{
        ui->error_label->show();
        ui->error_label->setText("خطا در برقراری اتصال");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
            ui->error_label->hide();
        });
    }
}


void AllUsersWidget::on_allUsers_pushButton_clicked()
{
    loadUsers();
}

