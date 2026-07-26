#include "commentswidget.h"
#include "ui_commentswidget.h"
#include "clientnetworkmanager.h"
#include "commentitemwidget.h"
#include <QTimer>
#include <QJsonArray>
#include <QScrollBar>

CommentsWidget::CommentsWidget(int userid, QWidget *parent, bool isPublisher)
    : QWidget(parent), userID(userid), m_isPublisher(isPublisher)
    , ui(new Ui::CommentsWidget)
{
    ui->setupUi(this);
    ui->error_label->hide();


    ui->listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->listWidget->verticalScrollBar()->setSingleStep(15);


    connect(&ClientNetworkManager::instance(), &ClientNetworkManager::responseReceived,
            this, &CommentsWidget::processNetworkData);

    connect(ui->rating_spinBox, &QSpinBox::valueChanged, this, &CommentsWidget::updateStarsLabel);
}

void CommentsWidget::loadComments(int bookid){
    bookID = bookid;

    pendingScrollValue = ui->listWidget->verticalScrollBar()->value();

    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["book_id"] = bookID;

        ClientNetworkManager::instance().sendRequest("GET_COMMENTS", data, true);
    }
    else{
        ui->error_label->setText("خطا در برقراری اتصال");
        ui->error_label->show();
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
            ui->error_label->hide();
        });

        ui->listWidget->clear();
    }

}

CommentsWidget::~CommentsWidget()
{
    delete ui;
}


void CommentsWidget::processNetworkData(const QString& action, const QJsonObject& data){
    if(action != "GET_COMMENTS_RESPONSE" &&
        action != "COMMENT_UPDATED" &&
        action != "ADD_COMMENT_RESPONSE" &&
        action != "EDIT_COMMENT_RESPONSE" &&
        action != "DELETE_COMMENT_RESPONSE"){
        return;
    }

    if(action == "GET_COMMENTS_RESPONSE"){
        updateListUi(data);
        return;
    }
    if(action == "COMMENT_UPDATED"){
        if(data.contains("book_id") && data["book_id"].toInt() == bookID){
            loadComments(bookID);
        }
    }
    else if(action == "ADD_COMMENT_RESPONSE"){
        if(data["status"].toString() != "SUCCESS"){
            ui->error_label->setText(data["message"].toString());
            ui->error_label->show();
            QTimer::singleShot(3000, this, [this](){
                ui->error_label->setText("");
                ui->error_label->hide();
            });
        }
        else{
            loadComments(bookID);
        }
    }
    else if("EDIT_COMMENT_RESPONSE"){
        if(data["status"].toString() != "SUCCESS"){
            ui->error_label->setText(data["message"].toString());
            ui->error_label->show();
            QTimer::singleShot(3000, this, [this](){
                ui->error_label->setText("");
                ui->error_label->hide();
            });
        }
        else{
            loadComments(bookID);
        }
    }
    else if("DELETE_COMMENT_RESPONSE"){
        if(data["status"].toString() != "SUCCESS"){
            ui->error_label->setText(data["message"].toString());
            ui->error_label->show();
            QTimer::singleShot(3000, this, [this](){
                ui->error_label->setText("");
                ui->error_label->hide();
            });
        }
        else{
            loadComments(bookID);
        }
    }
}


void CommentsWidget::updateListUi (const QJsonObject& response) {
    if (response["status"].toString() != "SUCCESS") return;

    ui->listWidget->clear();

    if(!m_isPublisher && userID != -1){
        ui->write_frame->show();
    }
    else{
        ui->write_frame->hide();
    }
    ui->comment_textEdit->clear();
    ui->rating_spinBox->setValue(5);
    m_editingCommentId = -1;

    QJsonArray commentsArray = response["comments"].toArray();

    QJsonObject userCommentJson;
    bool hasUserComment = false;

    for (const QJsonValue& value : commentsArray) {
        QJsonObject commentObj = value.toObject();
        commentObj["book_id"] = bookID;
        Comment comment(commentObj);

        if (comment.getUserID() == userID) {
            hasUserComment = true;
            userCommentJson = commentObj;
        } else {
            CommentItemWidget* itemWidget = new CommentItemWidget(comment, userID, this);

            QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
            item->setSizeHint(itemWidget->sizeHint());
            ui->listWidget->setItemWidget(item, itemWidget);

            if(userID == -1){
                connect(itemWidget, &CommentItemWidget::deleteRequested, this, &CommentsWidget::onCommentDeleteRequested);
            }
        }
    }

    if (hasUserComment) {
        ui->write_frame->hide();

        Comment userComment(userCommentJson);

        CommentItemWidget* userItemWidget = new CommentItemWidget(userComment, userID, this);

        connect(userItemWidget, &CommentItemWidget::editRequested, this, &CommentsWidget::onCommentEditRequested);
        connect(userItemWidget, &CommentItemWidget::deleteRequested, this, &CommentsWidget::onCommentDeleteRequested);

        QListWidgetItem* item = new QListWidgetItem();
        ui->listWidget->insertItem(0, item);
        item->setSizeHint(userItemWidget->sizeHint());
        ui->listWidget->setItemWidget(item, userItemWidget);
    }

    QTimer::singleShot(0, this, [this]() {
        auto *bar = ui->listWidget->verticalScrollBar();

        bar->setValue(qMin(pendingScrollValue, bar->maximum()));
    });
}


void CommentsWidget::onCommentEditRequested(const Comment& comment) {
    // ۱. نمایش فریم نوشتن
    ui->write_frame->show();

    // ۲. استخراج اطلاعات از شیء کامنت دریافتی و ریختن در فیلدها
    ui->comment_textEdit->setPlainText(comment.getText());
    ui->rating_spinBox->setValue(comment.getRating());

    // ۳. تنظیم پرچم ادیت با آیدی کامنت
    m_editingCommentId = comment.getId();

    ui->comment_textEdit->setFocus();
}

void CommentsWidget::onCommentDeleteRequested(int commentId) {
    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["comment_id"] = commentId;

        ClientNetworkManager::instance().sendRequest("DELETE_COMMENT", data, true);
    }
    else{
        ui->error_label->setText("خطا در برقراری اتصال");
        ui->error_label->show();
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
            ui->error_label->hide();
        });
    }
}

void CommentsWidget::on_submit_pushButton_clicked()
{
    QString text = ui->comment_textEdit->toPlainText().trimmed();
    int rating = ui->rating_spinBox->value();

    if(text.isEmpty()){
        ui->error_label->setText("متن نظر نمی‌تواند خالی باشد");
        ui->error_label->show();
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
            ui->error_label->hide();
        });
        return;
    }

    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;

        if(m_editingCommentId == -1){
            data["book_id"] = bookID;
            data["user_id"] = userID;
            data["text"] = text;
            data["rating"] = rating;

            ClientNetworkManager::instance().sendRequest("ADD_COMMENT", data, true);
        }
        else{
            data["comment_id"] = m_editingCommentId;
            data["text"] = text;
            data["rating"] = rating;
            ClientNetworkManager::instance().sendRequest("EDIT_COMMENT", data, true);
        }
    }
    else{
        ui->error_label->setText("خطا در برقراری اتصال");
        ui->error_label->show();
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
            ui->error_label->hide();
        });
    }
}


void CommentsWidget::on_back_pushButton_clicked()
{
    emit backToBookDatailPage();
}

void CommentsWidget::updateStarsLabel(int rating) {
    QString stars = QString("★").repeated(rating) + QString("☆").repeated(5 - rating);
    ui->rating_stars_label->setText(stars);
}
