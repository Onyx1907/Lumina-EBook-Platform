#ifndef USERHOMEWIDGET_H
#define USERHOMEWIDGET_H

#include <QWidget>

namespace Ui {
class UserHomeWidget;
}

class UserHomeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UserHomeWidget(QString cur_username, QWidget *parent = nullptr);
    ~UserHomeWidget();

private:
    Ui::UserHomeWidget *ui;

    QString username;
};

#endif // USERHOMEWIDGET_H
