#ifndef COMMENTSWIDGET_H
#define COMMENTSWIDGET_H

#include <QWidget>

namespace Ui {
class CommentsWidget;
}

class CommentsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CommentsWidget(QWidget *parent = nullptr);
    ~CommentsWidget();

private:
    Ui::CommentsWidget *ui;
};

#endif // COMMENTSWIDGET_H
