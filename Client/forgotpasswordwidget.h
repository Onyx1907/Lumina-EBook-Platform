#ifndef FORGOTPASSWORDWIDGET_H
#define FORGOTPASSWORDWIDGET_H

#include <QWidget>
#include <QStackedWidget>
#include "mainwindow.h"


namespace Ui {
class ForgotPasswordWidget;
}

class ForgotPasswordWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ForgotPasswordWidget(QWidget *parent = nullptr);
    ~ForgotPasswordWidget();

private slots:
    void on_back_pushButton_clicked();

    void on_back_pushButton_2_clicked();

private:
    Ui::ForgotPasswordWidget *ui;

    enum internalPage {
        VerificationStage = 0,
        ResetPasswordStage = 1
    };
};

#endif // FORGOTPASSWORDWIDGET_H
