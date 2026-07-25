#ifndef COMMENTITEMWIDGET_H
#define COMMENTITEMWIDGET_H

#include <QWidget>
#include "comment.h"

namespace Ui {
class CommentItemWidget;
}

class CommentItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CommentItemWidget(const Comment& comment, int cur_userID, QWidget *parent = nullptr);
    ~CommentItemWidget();

signals:
    void deleteRequested(int commentID);
    void editRequested(const Comment& commentData);

private:
    Ui::CommentItemWidget *ui;
    Comment m_comment;
};

#endif // COMMENTITEMWIDGET_H
