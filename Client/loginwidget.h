#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H
#include "regularuser.h"

#include <QWidget>
#include <QStackedWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class LoginWidget; }
QT_END_NAMESPACE

class LoginWidget : public QWidget {
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();

signals:
    //  سیگنال‌هایی که به مِین‌ویندو می‌گویند کاربر می‌خواهد جابجا شود
    void goToRegisterRequested();
    void goToForgotPasswordRequested();
    void goToUSerDashboard(User *user, bool is_first_login);


private slots:
    void on_register_pushButton_clicked();

    void on_forget_pushButton_clicked();

    void on_login_pushButton_clicked();

    void processNetworkData(const QString& action, const QJsonObject& data);

private:
    Ui::LoginWidget *ui;

    void enableFormWithError(const QString& errorMsg);

    QString current_username;

};

#endif // LOGINWIDGET_H
