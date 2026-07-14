#ifndef COMMENTSWIDGET_H
#define COMMENTSWIDGET_H

#include <QWidget>
#include "comment.h"


namespace Ui {
class CommentsWidget;
}

class CommentsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CommentsWidget(int userid, QWidget *parent = nullptr);
    ~CommentsWidget();

    void loadComments(int bookid);

private slots:
    void processNetworkData(const QString& action, const QJsonObject& data);

    void onCommentEditRequested(const Comment& comment);
    void onCommentDeleteRequested(int commentId);

    void on_submit_pushButton_clicked();

private:
    Ui::CommentsWidget *ui;
    int userID;
    int bookID = 0;
    int m_editingCommentId = -1;

    void updateListUi(const QJsonObject& response);
};

#endif // COMMENTSWIDGET_H
