#include "alluserswidget.h"
#include "ui_alluserswidget.h"
#include "clientnetworkmanager.h"
#include <QTimer>
#include <QScrollBar>
#include "usercard.h"
#include <QJsonArray>
#include "datefilterdialog.h"
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QEasingCurve>

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
        action != "SEARCH_USERS_RESPONSE" &&
        action != "DELETE_USER_RESPONSE" &&
        action != "SET_USER_ACTIVE_RESPONSE"){
        return;
    }

    if(!(data.value("message").toString().isEmpty())){
        ui->error_label->show();
        ui->error_label->setText(data.value("message").toString());
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
            ui->error_label->hide();
        });

        return;
    }

    if(action == "GET_ALL_USERS_RESPONSE"){
        fillResults(data);

        ui->allUsers_pushButton->hide();
    }
    else if(action == "SEARCH_USERS_RESPONSE"){
        fillResults(data, true);

        ui->allUsers_pushButton->show();
    }
    else if(action == "DELETE_USER_RESPONSE"){
        loadUsers();
    }
}


void AllUsersWidget::fillResults(const QJsonObject& data, bool is_new){
    ui->listWidget->clear();

    QJsonArray usersArray = data["users"].toArray();

    for (const QJsonValue& value : usersArray) {
        QJsonObject userObj = value.toObject();

        UserCard* itemWidget = new UserCard(userObj);

        QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
        item->setSizeHint(itemWidget->sizeHint());
        ui->listWidget->setItemWidget(item, itemWidget);

        connect(itemWidget, &UserCard::deleteRequested, this,
                &AllUsersWidget::sendDeleteRequest);

        connect(itemWidget, &UserCard::changeBlockedState, this,
                &AllUsersWidget::sendChangedBlockState);
    }

    if(!is_new){
        QTimer::singleShot(0, this, [this]() {
            auto *bar = ui->listWidget->verticalScrollBar();

            bar->setValue(qMin(pendingScrollValue, bar->maximum()));
        });
    }

    auto *effect = new QGraphicsOpacityEffect(ui->listWidget);
    ui->listWidget->setGraphicsEffect(effect);

    auto *anim = new QPropertyAnimation(effect, "opacity", this);
    effect->setOpacity(0);

    anim->setDuration(200);
    anim->setStartValue(0);
    anim->setEndValue(1);
    anim->setEasingCurve(QEasingCurve::OutCubic);

    anim->start(QAbstractAnimation::DeleteWhenStopped);
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
        blocked = 0;
        break;
    case 2:
        blocked = 1;
        break;
    }

    switch (ui->role_comboBox->currentIndex()) {
    case 0:
        role = "";
        break;
    case 1:
        role = "RegularUser";
        break;
    case 2:
        role = "Publisher";
        break;
    }

    if(registerDate.isEmpty() && ui->keyword_lineEdit->text().trimmed().isEmpty()
        && role.isEmpty() && blocked == -1){
        ui->error_label->show();
        ui->error_label->setText("حداقل یک فیلتر برای جستجو وارد کنید");
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
            ui->error_label->hide();
        });
        return;
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


void AllUsersWidget::sendDeleteRequest(int userID){

    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;

        data["user_id"] = userID;

        ClientNetworkManager::instance().sendRequest("DELETE_USER", data, true);
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

void AllUsersWidget::sendChangedBlockState(int userID, bool isActive){

    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;

        data["user_id"] = userID;
        data["active"] = isActive;

        ClientNetworkManager::instance().sendRequest("SET_USER_ACTIVE_STATE", data, true);
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
