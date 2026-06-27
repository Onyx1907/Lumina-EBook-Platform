#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include <QStackedWidget>
#include "mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LoginWidget; }
QT_END_NAMESPACE

class LoginWidget : public QWidget {
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();

private slots:
    void on_register_pushButton_clicked();

    void on_forget_pushButton_clicked();

private:
    Ui::LoginWidget *ui;
};

#endif // LOGINWIDGET_H
