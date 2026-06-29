#ifndef FORGOTPASSWORDWIDGET_H
#define FORGOTPASSWORDWIDGET_H

#include <QWidget>
#include <QStackedWidget>


namespace Ui {
class ForgotPasswordWidget;
}

class ForgotPasswordWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ForgotPasswordWidget(QWidget *parent = nullptr);
    ~ForgotPasswordWidget();

    void enableFormWithError(const QString& errorMsg);

signals:
    //  سیگنال‌هایی که به مِین‌ویندو می‌گویند کاربر می‌خواهد جابجا شود
    void goToLoginRequested();

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
